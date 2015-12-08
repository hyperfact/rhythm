/*!
 * \file chained_buf.h
 * \date 2015/10/15 10:44
 *
 * \author hy
 * Contact: hyperfact@gmail.com
 *
 * \brief 
 *
 * TODO: long description
 * Use cases:
 *     1. support iovec/OverlappedIO
 *     2. write back, read front, additional write front(e.g. add message header), read back(cancel write)
 *     3. memory alloc policy, especially for small memory
 *     4. reserve space
 *     5. access contiguous space
 *     6. reference count, for multicast support
 *     7. 
 *
 * \note
*/
#ifndef _CHAINED_BUF_H_
#define _CHAINED_BUF_H_

#pragma once

#include "buffer.h"

#ifndef nullptr
#  define nullptr NULL
#endif // nullptr 

_HYLIB_BEGIN

template <class _Alloc = std::allocator<void> >
class chained_buf;

template <class _Alloc>
class _chained_buf_base : public _buf_base {
public:
	enum { MAX_TO_REALIGN_IN_EXPAND = 2048, };
	enum { MIN_ALLOC_SIZE = 256, };

	struct iovec {
		void	*buf;
		size_t	len;
	};
	struct _buf_chain {
		enum {
			F_NONE			= 0x00000000,

			F_RLOCK			= 0x00000001,
			F_WLOCK			= 0x00000002,
			F_PIN_R			= 0x00000004,
			F_PIN_W			= 0x00000008,

			F_IMMUTABLE		= 0x00000010,
			F_REFERENCE		= 0x00000020,
			F_MULTICAST		= 0x00000040,
		};
		_buf_chain *next;
		char *buf;
		size_t total, len, offset;
		unsigned flags;
		unsigned refcnt;

		_buf_chain() { init(); }
		void init() {
			next = this;
			buf = nullptr;
			total = len = offset = 0;
			flags = 0;
			refcnt = 0;
		}

		char *pbegin() { return (buf+offset); }
		const char *pbegin() const { return (buf+offset); }
		char *plast() { return (pbegin()+len); }
		const char *plast() const { return (pbegin()+len); }
		char *pend() { return (buf+total); }
		const char *pend() const { return (buf+total); }
		size_t size() const { return (len); }
		size_t capacity() const { return (total); }
		size_t avail() const { return pre_avail()+post_avail(); }
		size_t pre_avail() const { return (offset); }
		size_t post_avail() const { return (total-len-offset); }
		bool wlocked() const { return BT_IS_SET(flags, F_WLOCK); }
		bool rlocked() const { return BT_IS_SET(flags, F_RLOCK); }
		bool any_locked() const { return wlocked() || rlocked(); }
		bool all_locked() const { return wlocked() && rlocked(); }
		bool ptr_valid(const void *p) const {
			const char *pc = (const char *)p;
			return (buf<=pc && pc<=pend());
		}
		bool ptr_r_valid(const void *p) const {
			const char *pc = (const char *)p;
			return (pbegin()<=pc && pc<=plast());
		}
		bool ptr_w_valid(const void *p) const {
			const char *pc = (const char *)p;
			return (plast()<=pc && pc<=pend());
		}

		bool should_realign(size_t inc) const {
			return (!any_locked())
				&& ((inc <= (capacity()-size()))
				&& (size() < capacity()/2)
				&& (size() < MAX_TO_REALIGN_IN_EXPAND));
		}
		void clear() {
			if (any_locked())
				return;
			len = offset = 0;
		}
		void align() {
			if (any_locked())
				return;
			assert(buf != nullptr);
			::memmove_s(buf, capacity(), pbegin(), size());
			offset = 0;
		}
		size_t prepend(const void *data, size_t datalen) {
			if (any_locked())
				return 0;
			assert(buf != nullptr);
			size_t n = min(datalen, pre_avail());
			memcpy_s(buf, pre_avail(), data, n);
			return n;
		}
		size_t append_no_align(const void *data, size_t datalen) {
			if (wlocked())
				return 0;
			assert(buf != nullptr);
			size_t n = min(datalen, post_avail());
			memcpy_s(plast(), post_avail(), data, n);
			len += n;
			return n;
		}
		size_t append(const void *data, size_t datalen) {
			if (any_locked())
				return 0;
			assert(buf != nullptr);
			if (datalen <= post_avail()) {
				memcpy_s(plast(), post_avail(), data, datalen);
				len += datalen;
				return datalen;
			} else if (should_realign(datalen)) {
				align();
				memcpy_s(plast(), post_avail(), data, datalen);
				len += datalen;
				return datalen;
			} else {
				size_t n = post_avail();
				memcpy_s(plast(), post_avail(), data, n);
				len += n;
				return n;
			}
		}
		size_t peek(void *data, size_t datalen) const {
			if (rlocked())
				return 0;
			assert(buf != nullptr);
			size_t rd = min(datalen, len);
			memcpy_s(data, datalen, buf, rd);
			return rd;
		}

		void *wlock(size_t &llen) {
			if (wlocked())
				return nullptr;
			if (0 == avail())
				return nullptr;
			if (llen > post_avail()) {
				if (should_realign(llen))
					align();
				llen = min(llen, post_avail());
			}
			BT_SET(flags, F_WLOCK);
			return plast();
		}
		bool wunlock(void *p, size_t ullen) {
			if (!wlocked())
				return false;
			if (p != plast())
				return false;
			if (ullen > post_avail())
				return false;
			len += ullen;
			BT_CLEAR(flags, F_WLOCK);
			return true;
		}
		const void *rlock(size_t &llen) {
			if (rlocked())
				return nullptr;
			llen = min(llen, size());
			BT_SET(flags, F_RLOCK);
			return pbegin();
		}
		bool runlock(const void *p, size_t ullen) {
			if (!rlocked())
				return false;
			if (p != pbegin())
				return false;
			if (ullen > size())
				return false;

			drain(ullen);
			return true;
		}
		size_t drain(size_t dlen) {
			dlen = min(dlen, len);
			len -= dlen;
			offset += dlen;
			if (0 == len)
				offset = 0; // realign when empty
			BT_CLEAR(flags, F_RLOCK);
			return dlen;
		}

		bool copy(const _buf_chain &o) {
			if (this == &o)
				return true;
			assert(capacity() >= o.size());
			assert(0 == size());
			clear();
			append(o.pbegin(), o.size());
			return true;
		}

		void *extra() { return (this+1); }
	};

	typedef void (*ref_cleanup_cb)(const void *data, size_t datalen, void *param);
	struct _buf_chain_reference : public _buf_chain {
		ref_cleanup_cb cleanupfn;
		void *param;
	};

	struct _buf_chain_multicast : public _buf_chain {
		_chained_buf_base *source;
		_buf_chain *parent;
	};

	static _buf_chain &nil() {
		static _buf_chain inst;
		return inst;
	}

	static size_t max_size() {
		return (size_t)-1;
	}

	_buf_chain *_AllocChain(size_t expect_len) {
		size_t to_alloc = expect_len + sizeof(_buf_chain);
		if (to_alloc < MIN_ALLOC_SIZE)
			to_alloc = MIN_ALLOC_SIZE;
		// OPTMZ: 自定义内存分配器
		void *p = _alloc.allocate(to_alloc);
		if (nullptr == p)
			return nullptr;
		_buf_chain *chain = (_buf_chain *)p;
		chain->init();
		chain->total = to_alloc - sizeof(_buf_chain);
		chain->offset = chain->len = 0;
		chain->flags = 0;
		chain->next = chain;
		chain->buf = (char *)chain->extra();
		return chain;
	}
	void _FreeChain(_buf_chain *chain) {
		// OPTMZ: 自定义内存分配器
		_alloc.deallocate((char *)chain, 1);
	}

	static _buf_chain_reference *_AllocChainRef(void *ptr, size_t len,
		ref_cleanup_cb cleanupfn, void *param) {
		// TODO:
		return NULL;
	}
	static _buf_chain_multicast *_AllocChainMulticast(_chained_buf_base *src,
		_buf_chain *parent) {
		// TODO:
		return NULL;
	}

	_chained_buf_base(const _Alloc &alloc=_Alloc()) : _alloc(alloc) {

	}
protected:
	typedef typename _Alloc::rebind<char>::other _AlTy;
	_AlTy _alloc;
};

template <class _Alloc>
class chained_buf : public _chained_buf_base<_Alloc> {
protected:
	typedef chained_buf<_Alloc> _MyType;
	typedef _buf_chain _Node;
	typedef _Node *_NodePtr;
	typedef const _Node *_NodeCPtr;

	struct _AlignGuard {
		_AlignGuard(_MyType &obj, _NodePtr chain) : obj_(obj), chain_(chain) {
			offset_ = chain_->pre_avail();
		}
		~_AlignGuard () {
			size_t now = chain_->pre_avail();
		}
		_MyType &obj_;
		_NodePtr chain_;
		size_t offset_;
	};

public:
	chained_buf() {
		_Init();
	}
	~chained_buf() {
		_Tidy();
	}

	size_t size() const {
		return _Len();
	}

	size_t avail() const {
		// FIXME
		return _AvailAcc();
	}

	size_t capacity() const {
		return _Total();
	}

	bool empty() const {
		return 0 == size();
	}

	void assign(const _MyType &o) {
		// TODO:
	}

	bool reserve(size_t n) {
		_NodePtr chain = _AllocChain(n);
		_Insert(_Last(), chain);
		return true;
	}

	size_t peek(void *buf, size_t len) const {
		if (rlocked())
			return 0;

		if (_Prd() == _End())
			return 0;

		size_t remain = len;
		_NodeCPtr chain = _Prd();
		for (; chain!=_End() && remain>0; chain=chain->next) {
			remain -= chain->peek(buf, remain);
		}
		return len-remain;
	}

	size_t read(void *buf, size_t len) {
		if (rlocked())
			return 0;
		len = peek(buf, len);
		size_t dlen = _Drain(len);
		assert(dlen == len);
		(void)dlen;
		return len;
	}

	size_t drain(size_t len) {
		if (rlocked())
			return 0;
		return _Drain(len);
	}

	size_t write(const void *data, size_t datalen) {
		if (wlocked())
			return 0;

		size_t remain = datalen;
		if (_Pwt()!=_End() && _Pwt()->avail()>0) {
			size_t wt = _ChainAppend(_Pwt(), data, remain);
			if (wt > 0) {
				assert(wt <= remain);
				remain -= wt;
			}
			if (0 == remain)
				return datalen;
		}

		if (avail() < remain) {
			_Grow(remain - avail());
		}

		if (_Pwt() == _End()) {
			assert(_Beg() != _End());
			_Pwt() = _Beg();
		} else {
			assert(_Pwt()->next != _End());
			_Pwt() = _Pwt()->next;
		}

		_NodePtr chain = _Pwt();
		for (; chain!=_End() && remain>0; chain=chain->next) {
			size_t wt = _ChainAppend(chain, data, remain);
			remain -= wt;
			_Pwt() = chain;
		}
		assert(0 == remain);
		return datalen;
	}
	void *wlock(size_t &llen) {
		if (wlocked())
			return nullptr;

		if (_Pwt()!=_End() && llen<=_Pwt()->avail()) {
			if (llen <= _Pwt()->post_avail()) {
				BT_SET(_Flags(), STATE_WRITE_LOCKED);
				return _Pwt()->wlock(llen);
			} else if (_Pwt()->should_realign(llen)) {
				_Pwt()->align();
				BT_SET(_Flags(), STATE_WRITE_LOCKED);
				return _Pwt()->wlock(llen);
			}
		}

		_NodePtr chain = _AllocChain(llen);
		_Insert(_Pwt(), chain);
		_Pwt() = chain;
		BT_SET(_Flags(), STATE_WRITE_LOCKED);
		return chain->wlock(llen);
	}

	bool wunlock(void *p, size_t len) {
		if (!wlocked())
			return false;
		if (_Pwt() == _End())
			return false;
		if ((char *)p != _Pwt()->plast())
			return false;
		if (len > _Pwt()->post_avail())
			return false;
		if (!_Pwt()->wunlock(p, len))
			return false;
		_Len() += len;
		BT_CLEAR(_Flags(), STATE_WRITE_LOCKED);
		return true;
	}

	int wlock(size_t &len, iovec *vecs, int nvec) {
		if (wlocked())
			return 0;

		if (0 == len)
			return 0;

		size_t remain = len;
		int n = 0;
		_NodePtr chain = _Pwt();
		if (chain != _End()) {
			if (chain->avail() > 0) {
				size_t llen0 = min(remain, chain->avail());
				if (chain->post_avail()<llen0 && chain->should_realign(llen0))
					chain->align();
			}
			if (0 == chain->post_avail())
				_Pwt() = chain = chain->next;

			for (; remain>0 && chain!=_End(); chain=chain->next) {
				assert(chain->post_avail() > 0);
				if (0 == chain->post_avail())
					continue;
				size_t llen = min(remain, chain->post_avail());

				if (vecs != nullptr) {
					if (n < nvec) {
						vecs[n].buf = chain->wlock(llen);
						vecs[n].len = llen;
					} else
						break;
				}
				remain -= llen;
				++n;
			}
		}

		if (nullptr == vecs)
			return (remain > 0) ? n+1 : n;

		if (remain>0 && n<nvec) {
			chain = _AllocChain(remain);
			_Insert(_Pwt(), chain);
			if (0 == n)
				_Pwt() = chain;
			vecs[n].buf = chain->wlock(remain);
			vecs[n].len = remain;
			++n;
		}
		BT_SET(_Flags(), STATE_WRITE_LOCKED);
		return n;
	}
	bool wunlock(iovec *vecs, int nvec) {
		if (!wlocked())
			return false;
		if (_Pwt() == _End())
			return false;

		int n = 0;
		// verify pointer and len in vecs
		for (_NodePtr chain = _Pwt();
			chain!=_End() && vecs!=nullptr && n<nvec;
			chain=chain->next, ++n) {
			if (vecs[n].buf!=chain->plast() || vecs[n].len>chain->post_avail())
				return false;
		}

		n = 0;
		for (_NodePtr chain = _Pwt();
			chain != _End()/*&&BT_IS_SET(chain->flags, _Node::F_WLOCK)*/;
			chain = chain->next,++n) {
			if (vecs!=nullptr && n<nvec) {
				bool suc = chain->wunlock(vecs[n].buf, vecs[n].len);
				_Len() += vecs[n].len;
				assert(suc);
				(void)suc;
			} else {
				BT_CLEAR(chain->flags, _Node::F_WLOCK);
			}
		}
		BT_CLEAR(_Flags(), STATE_WRITE_LOCKED);
		return true;
	}

	const void *rlock(size_t &len) {
		if (rlocked())
			return nullptr;

		if (_Prd() == _End())
			return nullptr;

		len = min(len, size());
		if (0 == len)
			return nullptr;

		_NodePtr prd = _Prd();
		if (prd->size() >= len) {
			BT_SET(_Flags(), STATE_READ_LOCKED);
			return prd->rlock(len);
		} else {
			size_t remain = len - prd->size();
			if (prd->post_avail() >= remain) {
				// do nothing
			} else if (prd->avail()>remain && prd->should_realign(remain)) {
				prd->align();
			} else {
				_NodePtr chain = _AllocChain(len);
				chain->copy(*prd);
				prd->clear();
				_Erase(_Head());
				_Insert(_Head(), chain);
				prd = chain;
			}
			for (_NodePtr cur=prd->next; remain>0&&cur!=_End(); /**/) {
				size_t app = 0;
				if (remain >= cur->size()) {
					app = cur->size();
					prd->append(cur->pbegin(), app);
					
					remain -= app;
					cur = _Erase(prd);
				} else {
					app = remain;
					prd->append(cur->pbegin(), app);
					cur->drain(app);

					remain -= app;
					cur=cur->next;
				}
			}
		}

		BT_SET(_Flags(), STATE_READ_LOCKED);
		return prd->rlock(len);
	}
	bool runlock(const void *p, size_t len) {
		if (!rlocked())
			return false;

		if (_Prd() == _End())
			return false;

		if ((const char *)p != _Prd()->pbegin())
			return false;

		if (len > size()) {
			return false;
		}
		if (len > _Prd()->size())
			return false;

		if (!_Prd()->runlock(p, len))
			return false;
		
		if (0 == _Prd()->size())
			_Erase(_Head());

		_Len() -= len;
		BT_CLEAR(_Flags(), STATE_READ_LOCKED);
		return true;
	}
	int rlock(size_t &len, iovec *vecs, int nvec) {
		if (rlocked())
			return 0;

		if (_Prd() == _End())
			return 0;

		if (0 == len)
			return 0;

		size_t remain = len = min(len, size());
		int n = 0;
		for (_NodePtr chain=_Prd(); remain>0 && chain!=_End(); chain=chain->next) {
			size_t llen = min(remain, chain->size());
			if (vecs != nullptr) {
				if (n < nvec) {
					vecs[n].buf = (void *)chain->rlock(llen);
					vecs[n].len = llen;
				} else {
					break;
				}
			}

			++n;
			remain -= llen;
		}
		len -= remain;
		if (vecs != nullptr)
			BT_SET(_Flags(), STATE_READ_LOCKED);
		return n;
	}

	bool runlock(size_t len) {
		if (len > size())
			return false;
		size_t dlen = _Drain(len);
		assert(dlen == len);
		(void)dlen;
		BT_CLEAR(_Flags(), STATE_READ_LOCKED);
		return true;
	}

	_MyType &transplant(_MyType &from, size_t len) {
		if (any_locked() || from.any_locked())
			return *this;
		if (0==len || from.empty())
			return *this;
		len = min(len, from.size());
		if (from.size() == len) {
			_NodePtr chainB = from._Beg();
			_NodePtr chainL = from._Pwt();
			chainL->next = _Last()->next;
			_Last()->next = chainB;
			if (_Last() == _Pwt())
				_Last() = chainL;
			_Pwt() = chainL;
			_Total() += from._Total();
			assert(_Total() == _TotalAcc());
			_Len() += from._Len();
			assert(_Len() == _LenAcc());
			from._Init();
		} else {
			size_t remain = len;
			_NodePtr ch = from._Beg();
			for (; ch!=from._End()&&remain>=ch->size(); ) {
				_NodePtr next = from._Erase(from._Head(), false);
				size_t chlen = ch->size();
				from._Len() -= chlen;
				_Insert(_Pwt(), ch);
				_Pwt() = ch;
				_Len() += chlen;
				remain -= chlen;

				ch = next;
			}
			if (remain > 0) {
				size_t llen = remain;
				const void *p = ch->rlock(llen);
				write(p, llen);
				ch->runlock(p, llen);
				from._Len() -= remain;
			}
		}
		return *this;
	}

	bool wlocked() const {
		return BT_IS_SET(_Flags(), STATE_WRITE_LOCKED);
	}

	bool rlocked() const {
		return BT_IS_SET(_Flags(), STATE_READ_LOCKED);
	}

	bool any_locked() const {
		return wlocked() || rlocked();
	}

	bool all_locked() const {
		return wlocked() && rlocked();
	}

	bool clear() {
		assert(!any_locked());
		if (any_locked())
			return false;

		for (_NodePtr ch=_Beg(), next=nullptr; ch!=_End(); ch=next) {
			next = ch->next;
			_FreeChain(ch);
		}
		_Total() = 0;
		_Len() = 0;
		return true;
	}

protected:
	void _Init() {
		_head.init();
		_last_with_data = _last = _End();
		_Flags() = STATE_NONE;
	}
	void _Tidy() {
		clear();
	}

	void _Insert(_NodePtr where, _NodePtr chain) {
		chain->next = where->next;
		where->next = chain;
		if (_Last() == where)
			_Last() = chain;

		_Total() += chain->capacity();
		assert(_Total() == _TotalAcc());
	}
	_NodePtr _Erase(_NodePtr prev, bool free_chain=true) {
		_NodePtr chain = prev->next;
		assert(chain != _End());
		if (chain == _End())
			return _End();
		//assert(0 == chain->size());
		prev->next = chain->next;
		if (chain == _Last())
			_Last() = prev;
		if (chain == _Pwt())
			_Pwt() = prev;
		_Total() -= chain->capacity();
		assert(_Total() == _TotalAcc());
		//_Len() -= chain->size();
		//assert(_Len() == _LenAcc());
		if (free_chain)
			_FreeChain(chain);
		return prev->next;
	}

	size_t _Drain(size_t len) {
		size_t remain = len;
		_NodePtr chain = _Prd();
		_NodePtr prev = _Head();
		for (; chain!=_End() && remain>0; /**/) {
			remain -= _ChainDrain(chain, remain);

			if (0 == chain->size()) {
				chain = _Erase(prev);
			} else {
				prev = chain;
				chain = chain->next;
			}
		}
		return (len - remain);
	}

	size_t _ChainAppend(_NodePtr chain, const void *data, size_t datalen) {
		assert(chain != _End());
		size_t wt = chain->append(data, datalen);
		_Len() += wt;
		assert(_Len() == _LenAcc());
		return wt;
	}
	size_t _ChainDrain(_NodePtr chain, size_t len) {
		assert(chain != _End());
		size_t dr = chain->drain(len);
		_Len() -= dr;
		return dr;
	}

	bool _Grow(size_t inc) {
		// FIXME
		return reserve(avail()+inc);
	}

	size_t _TotalAcc() const {
		size_t sum = 0;
		for (_NodeCPtr n=_Beg(); n!=_End(); n=n->next)
			sum += n->capacity();
		return sum;
	}
	size_t _LenAcc() const {
		size_t sum = 0;
		for (_NodeCPtr n=_Beg(); n!=_End(); n=n->next)
			sum += n->size();
		return sum;
	}
	size_t _AvailAcc() const {
		size_t sum = 0;
		for (_NodeCPtr n=(_Pwt()!=_End()?_Pwt():_Beg()); n!=_End(); n=n->next)
			sum += n->post_avail();
		return sum;
	}

	size_t &_Total() { return _head.total; }
	size_t _Total() const { return _head.total; }
	size_t &_Len() { return _head.len; }
	size_t _Len() const { return _head.len; }
// 	size_t &_Avail() { return _head.offset; }
// 	size_t _Avail() const { return _head.offset; }
	unsigned &_Flags() { return _head.flags; }
	unsigned _Flags() const { return _head.flags; }

	_NodePtr &_Beg() { return _head.next; }
	_NodeCPtr _Beg() const { return _head.next; }
	_NodePtr &_Last() { return _last; }
	_NodeCPtr _Last() const { return _last; }
	_NodePtr _Head() { return &_head; }
	_NodeCPtr _Head() const { return &_head; }
	_NodePtr _End() { return &_head; }
	_NodeCPtr _End() const { return &_head; }
	_NodePtr &_Prd() { return _Beg(); }
	_NodeCPtr _Prd() const { return _Beg(); }
	_NodePtr &_Pwt() { return _last_with_data; }
	_NodeCPtr _Pwt() const { return _last_with_data; }

protected:
	_Node			_head;
	_NodePtr		_last;
	_NodePtr		_last_with_data;
};

_HYLIB_END

#endif // _CHAINED_BUF_H_
