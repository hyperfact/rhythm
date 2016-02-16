// buffer.h: interface for the buffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BUFFER_H__3573931D_B863_431E_8617_73666BB05E9F__INCLUDED_)
#define AFX_BUFFER_H__3573931D_B863_431E_8617_73666BB05E9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_prolog.h"
#include "macro.h"
#include "compare.h"

#include <assert.h>
#include <xutility>

_HYLIB_BEGIN
////////////////////////////////////////////////////////////////////////////////
struct HalfGrowPolicy {
	static size_t grow_size(size_t cur, size_t inc) {
		inc = max(inc, cur/2); // grow by 50%
		return (cur+inc);
	}
};

////////////////////////////////////////////////////////////////////////////////
struct _buf_base
{
	enum State {
		STATE_NONE				= 0,
		STATE_READ_LOCKED		= 1,
		STATE_WRITE_LOCKED		= 1<<1,
	};

	static const size_t MAX_SIZE = (size_t)-1;
};

template <class B>
struct RLocker {
	typedef const void * pointer_type;

	RLocker(B &b, size_t llen=_buf_base::MAX_SIZE) : _buf(b), _llen(llen) {
		_ptr = b.rlock(_llen);
	}

	~RLocker() {
		if (_buf.rlocked())
			_buf.runlock(_ptr, 0);
	}

	void unlock(size_t len=_buf_base::MAX_SIZE) {
		if (len == _buf_base::MAX_SIZE)
			len = _llen;
		_buf.runlock(_ptr, len);
	}

	size_t size() const { return _llen; }

	operator pointer_type() const { return _ptr; }
private:
	B &_buf;
	pointer_type _ptr;
	size_t _llen;
};

template <class B>
struct WLocker {
	typedef void *pointer_type;

	WLocker(B &b, size_t llen=_buf_base::MAX_SIZE) : _buf(b), _llen(llen) {
		_ptr = b.wlock(_llen);
	}

	~WLocker() {
		if (_buf.wlocked())
			_buf.wunlock(_ptr, 0);
	}

	void unlock(size_t len=_buf_base::MAX_SIZE) {
		if (len == _buf_base::MAX_SIZE)
			len = _llen;
		_buf.wunlock(_ptr, len);
	}

	size_t size() const { return _llen; }

	operator pointer_type() { return _ptr; }
private:
	B &_buf;
	pointer_type _ptr;
	size_t _llen;
};

////////////////////////////////////////////////////////////////////////////////
template <bool _Elastic=true, class _GrowPolicy = HalfGrowPolicy>
struct cyc_buf : public _buf_base
{
protected:
	typedef cyc_buf<_Elastic, _GrowPolicy> _MyType;

public:
	cyc_buf() {
		_Init();
	}

	explicit cyc_buf(size_t size) {
		++size; // 环形缓冲区末尾需要一个空字节
		size = DWORDAlign(size);
		_total = size;
		_data = new char[_total];
		_pbeg = _pend = _data;
		_state = STATE_NONE;
	}

	cyc_buf(const cyc_buf &o) {
		_Init();
		assign(o);
	}

	~cyc_buf() {
		_Tidy();
	}

	_MyType &operator = (const _MyType &o) {
		if (this != &o)
			assign(o);
		return *this;
	}

	void _Init() {
		_total = 0;
		_data = NULL;
		_pbeg = NULL;
		_pend = NULL;
		_state = STATE_NONE;
	}

	void _Tidy() {
		SafeDeleteArray(_data);
	}

	void assign(const _MyType &o) {
		if (this == &o)
			return;
		ASSERT(!o.any_locked());
		_Tidy();
		if (o._data) {
			_total = o._total;
			_data = new char[_total];
			_pbeg = _data;
			_pend = _data + o.peek(_data, _total);
			_state = STATE_NONE;
		} else {
			_Init();
		}
	}

	size_t size() const {
		return (_pend>=_pbeg) ? (_pend-_pbeg) : (_pend-_pbeg+_total);
	}

	size_t avail() const {
		return (capacity() - size());
	}

	size_t capacity() const {
		return _total-1;
	}

	bool empty() const {
		return 0 == size();
	}

	bool reserve(size_t n) {
		++n; // 环形缓冲区末尾需要一个空字节
		n = DWORDAlign(n);

		if (any_locked()) { return false; }

		if (n <= _total) { return true; }
		char *pnew = new char[n];
		if (NULL == pnew) { return false; }

		size_t sz = 0;
		if (_data) {
			sz = size();
			if (_pend >= _pbeg) {
				::memcpy(pnew, _pbeg, sz);
			} else {
				size_t nTail = _last()-_pbeg;
				::memcpy(pnew, _pbeg, nTail);
				::memcpy(pnew+nTail, _data, sz-nTail);
			}
			SafeDeleteArray(_data);
		}
		
		_data = pnew;
		_total = n;
		_pbeg = _data;
		_pend = _data + sz;

		return true;
	}

	size_t peek(void *buf, size_t len) const {
		assert(_data);

		if (rlocked()) { return 0; }

		len = min(size(), len);
		size_t nTail = _last() - _pbeg;
		if (_pend>=_pbeg || nTail>=len) {
			::memcpy(buf, _pbeg, len);
		} else {
			::memcpy(buf, _pbeg, nTail);
			buf = (char *)buf + nTail;
			::memcpy(buf, _data, len-nTail);
		}
		return len;
	}

	size_t read(void *buf, size_t len) {
		assert(_data);

		if (rlocked()) { return 0; }

		len = peek(buf, len);
		_pbeg += len;
		if (_pbeg >= _last()) {
			_pbeg = _data + (_pbeg-_last());
		}
		return len;
	}

	size_t write(const void *buf, size_t len) {
		//assert(_data);
		
		if (wlocked()) { return 0; }

		if (avail() < len) {
			_grow(len - avail());
		}

		size_t nTail = _last()-_pend;
		if (nTail >= len) {
			::memcpy(_pend, buf, len);
			_pend += len;
			if (_pend >= _last())
				_pend = _data + (_pend - _last());
		} else {
			::memcpy(_pend, buf, nTail);
			buf = (char *)buf + nTail;
			::memcpy(_data, buf, len-nTail);
			_pend = _data + (len-nTail);
		}
		return len;
	}

	void *wlock(size_t &len) {
		//assert(_data);

		if (wlocked()) { return NULL; }

		if (MAX_SIZE == len)
			len = avail();

		if (avail() < len) {
			_grow(len - avail());
		}

		if ((size_t)(_last()-_pend) < len) {
			if (_Elastic) {
				_repos();
			} else {
				len = (size_t)(_last()-_pend);
			}
		}

		BT_SET(_state, STATE_WRITE_LOCKED);
		return _pend;
	}

	bool wunlock(void *p, size_t len) {
		assert(_data);

		if (!wlocked()) { return false; }

		if ((char *)p != _pend) {
			return false;
		}
		if (len > avail()) {
			return false;
		}
		if (len > (_last()-_pend)) {
			return false;
		}

		_pend += len;
		if (_pend >= _last())
			_pend = _data + (_pend - _last());
		BT_CLEAR(_state, STATE_WRITE_LOCKED);
		return true;
	}

	const void *rlock(size_t &len) {
		assert(_data);

		if (rlocked()) { return NULL; }

		len = min(len, size());

		if ((size_t)(_last()-_pbeg) < len) {
			if (_Elastic) {
				_repos();
			} else {
				len = (size_t)(_last()-_pbeg);
			}
		}

		BT_SET(_state, STATE_READ_LOCKED);
		return _pbeg;
	}
	
	bool runlock(const void *p, size_t len) {
		assert(_data);
		
		if (!rlocked()) { return false; }

		if ((const char *)p != _pbeg) {
			return false;
		}
		if (len > size()) {
			return false;
		}
		if (len > (size_t)(_last()-_pbeg)) {
			return false;
		}

		_pbeg += len;
		BT_CLEAR(_state, STATE_READ_LOCKED);
		return true;
	}

	bool wlocked() const {
		return BT_IS_SET(_state, STATE_WRITE_LOCKED);
	}

	bool rlocked() const {
		return BT_IS_SET(_state, STATE_READ_LOCKED);
	}

	bool any_locked() const {
		return wlocked() || rlocked();
	}

	bool all_locked() const {
		return wlocked() && rlocked();
	}

	bool clear() {
		if (any_locked()) { return false; }

		_pbeg = _pend = _data;
		return true;
	}

protected:
	char *_last() {
		assert(_data);
		return _data + _total;
	}
	const char *_last() const {
		assert(_data);
		return _data + _total;
	}
	
	void _grow(size_t inc) {
		reserve(_GrowPolicy::grow_size(capacity(), inc));
	}

	void _repos() {
		assert(_data);

		size_t siz = size();
		if (_pbeg <= _pend) {
			::memmove(_data, _pbeg, siz);
		} else {
			size_t unuse = (size_t)(_pbeg-_pend);
			size_t part1 = (size_t)(_last()-_pbeg);
			size_t part2 = (size_t)(_pend-_data);
			// 图示: '1':part1, '2'-part2, '-'-unuse
			if (part1 <= unuse) {
				// case 1: part1+part2
				// 222---11
				::memmove(_data+part1, _data, part2);			// --222-11
				::memmove(_data, _pbeg, part1);					// 11222---
			} else if (part2 <= unuse) {
				// p2 < u < p1
				// case 2: part1+part2
				// 22---111
				::memmove(_data+part2, _pbeg, part1);			// 22111---
				::memmove(_data+part2+part1, _data, part2);		// --11122-
				//::memmove(_data, _data+part2, part1+part2);	// 11122---
				_pbeg = _data + part2;
				_pend = _pbeg + siz;
				return;
			} else if (part1 <= (unuse*2) && part2<=part1) {
				// u < p2 < p1 < 2*u
				// case 3: part1+part2 + part1+par2
				// part2+unuse+part1-unuse == part1+part2
				// 2222222---11111
				::memmove(_data+unuse, _data, part2);			// ---222222211111
				::memmove(_data, _last()-unuse, unuse);			// 111222222211---
				::memmove(_pbeg+unuse, _pbeg, part1-unuse);		// 1112222222---11
				_pend = _pbeg;
				_pbeg += unuse;									// 2222222222---11
				assert((size_t)(_last()-_pbeg) <= unuse);
				_repos();										// (jmp to case 1)
				return;
			} else if (part2 <= (unuse*2) && part1<=part2) {
				// u < p1 < p2 < 2*u
				// case 4: part1+part2 + part1+part2
				// part1+unuse+part2-unuse == part1+part2
				// 22222---1111111
				::memmove(_data+part2, _pbeg, part1);			// 222221111111---
				::memmove(_data+part1+part2, _data, unuse);		// ---221111111222
				::memmove(_data, _data+unuse, part2-unuse);		// 22---1111111222
				_pbeg = _pend;
				_pend -= unuse;									// 22---1111111111
				assert((size_t)(_pend-_data) <= unuse);
				_repos();										// (jmp to case 2)
				return;
			} else {
				// case 5:
				char *pnew = new char[_total];
				::memcpy(pnew, _pbeg, part1);
				::memcpy(pnew+part1, _data, part2);
				SafeDeleteArray(_data);
				_data = pnew;
			}
		}

		_pend = _data + siz;
		_pbeg = _data;
	}

protected:
	char		*_data;
	size_t		_total;
	char		*_pbeg;
	char		*_pend;
	unsigned	_state;
};

typedef cyc_buf<> CCycBuf;

////////////////////////////////////////////////////////////////////////////////
template <class _GrowPolicy = HalfGrowPolicy>
struct fixed_buf : public _buf_base
{
protected:
	typedef fixed_buf<_GrowPolicy> _MyType;

public:
	fixed_buf() {
		_Init();
	}

	explicit fixed_buf(size_t size) {
		size = DWORDAlign(size);
		_total = size;
		_data = new char[size];
		_ppos = _data;
		_state = STATE_NONE;
	}

	fixed_buf(const _MyType &o) {
		_Init();
		assign(o);
	}

	_MyType &operator = (const _MyType &o) {
		if (this != &o)
			assign(o);
		return *this;
	}

	~fixed_buf() {
		_Tidy();
	}

	void _Init() {
		_total = 0;
		_data = NULL;
		_ppos = NULL;
		_state = STATE_NONE;
	}

	void _Tidy() {
		SafeDeleteArray(_data);
	}

	void assign(const _MyType &o) {
		if (this == &o)
			return;
		ASSERT(!o.any_locked());
		_Tidy();
		if (o._data) {
			_total = o._total;
			_data = new char[_total];
			_ppos = _data + o.peek(_data, _total);
			_state = STATE_NONE;
		} else {
			_Init();
		}
	}

	size_t size() const {
		return _ppos - _data;
	}

	size_t avail() const {
		return (capacity() - size());
	}

	size_t capacity() const {
		return _total;
	}
	
	bool empty() const {
		return 0 == size();
	}

	bool reserve(size_t n) {
		n = DWORDAlign(n);
		if (any_locked()) { return false; }

		if (n <= _total) { return true; }
		char *pnew = new char[n];
		if (NULL == pnew) { return false; }
		
		size_t sz = 0;
		if (_data) {
			sz = size();
			::memcpy(pnew, _data, sz);
			SafeDeleteArray(_data);
		}
		_data = pnew;
		_total = n;
		_ppos = pnew + sz;
		return true;
	}

	size_t peek(void *buf, size_t len) const {
		assert(_data != NULL);

		if (rlocked()) { return 0; }

		len = min(len, size());
		::memcpy(buf, _data, len);
		return len;
	}

	size_t read(void *buf, size_t len) {
		assert(_data != NULL);

		if (rlocked()) { return 0; }

		len = peek(buf, len);
		::memmove(_data, _data+len, size()-len);
		_ppos -= len;
		return len;
	}

	size_t write(const void *buf, size_t len) {
		//assert(_data != NULL);

		if (wlocked()) { return 0; }

		if (avail() < len) {
			_grow(len - avail());
		}

		len = min(len, capacity()-size());
		::memcpy(_ppos, buf, len);
		_ppos += len;
		return len;
	}

	void *wlock(size_t &len) {
		//assert(_data);
		
		if (wlocked()) { return NULL; }

		if (MAX_SIZE == len)
			len = avail();

		if (avail() < len) {
			_grow(len - avail());
		}

		BT_SET(_state, STATE_WRITE_LOCKED);
		return _ppos;
	}

	bool wunlock(void *p, size_t len) {
		assert(_data);

		if (!wlocked()) { return false; }

		if ((char *)p != _ppos) {
			return false;
		}
		if (len > avail()) {
			return false;
		}

		_ppos += len;
		BT_CLEAR(_state, STATE_WRITE_LOCKED);
		return true;
	}

	const void *rlock(size_t &len) {
		assert(_data);

		if (rlocked()) { return NULL; }

		len = min(size(), len);
		BT_SET(_state, STATE_READ_LOCKED);
		return _data;
	}

	bool runlock(const void *p, size_t len) {
		assert(_data);
		
		if (!rlocked()) { return false; }
		if (wlocked()) { return false; }
		
		if ((const char *)p != _data) {
			return false;
		}
		if (len > size()) {
			return false;
		}

		::memmove(_data, _data+len, size()-len);
		_ppos -= len;
		BT_CLEAR(_state, STATE_READ_LOCKED);
		return true;
	}

	bool wlocked() const {
		return BT_IS_SET(_state, STATE_WRITE_LOCKED);
	}
	
	bool rlocked() const {
		return BT_IS_SET(_state, STATE_READ_LOCKED);
	}
	
	bool any_locked() const {
		return wlocked() || rlocked();
	}
	
	bool all_locked() const {
		return wlocked() && rlocked();
	}

	bool clear() {
		if (any_locked()) { return false; }

		_ppos = _data;
		return true;
	}

	operator char *() {
		assert(_data);
		return _data;
	}

	operator const char *() const {
		assert(_data);
		return _data;
	}

protected:
	char *_last() {
		assert(_data);
		return _data + _total;
	}
	const char *_last() const {
		assert(_data);
		return _data + _total;
	}

	void _grow(size_t inc) {
		reserve(_GrowPolicy::grow_size(capacity(), inc));
	}

protected:
	char		*_data;
	size_t		_total;
	char		*_ppos;
	unsigned	_state;
};

typedef fixed_buf<> CFixedBuf;

template <class B1, class B2>
inline size_t buf_transfor(B1 &b1, B2 &b2, size_t len) {
	void *d = b2.wlock(len);
	size_t rd = b1.read(d, len);
	b2.wunlock(d, rd);
	return rd;
}

template <class B, class T>
inline B & buf_data_read(B &b, T &t) {
	b.read(&t, sizeof(t));
	return b;
}

template <class B, class T>
inline B & buf_data_write(B &b, const T &t) {
	b.write(&t, sizeof(t));
	return b;
}

template <class B, class T>
struct _buf_data_lock_guard {
	_buf_data_lock_guard(B &b) : b_(b) {
		size_t wt = sizeof(T);
		ptr_ = reinterpret_cast<T *>(b_.wlock(wt));
	}
	~_buf_data_lock_guard() {
		b_.wunlock(ptr_, sizeof(T));
	}
	T &get() {
		return *ptr_;
	}
	T & operator * () {
		return *ptr_;
	}
	T *operator -> () {
		return ptr_;
	}
	operator T *() {
		return ptr_;
	}
private:
	B &b_;
	T *ptr_;
};

#if 0
template <class B, class T>
inline _buf_data_lock_guard<B, T> _make_buf_data_lock_guard(B &b, T *dummy=NULL) {
	(void)dummy;
	return _buf_data_lock_guard<B, T>(b);
}
#define BUF_DATA_LOCK_GUARD(var, buf, type) _buf_data_lock_guard var((buf), static_cast<type *>(NULL))
#endif

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // !defined(AFX_BUFFER_H__3573931D_B863_431E_8617_73666BB05E9F__INCLUDED_)
