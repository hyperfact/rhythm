/********************************************************************
	created:	2011/11/14
	created:	14:11:2011   15:28
	filename: 	Singleton.h
	file base:	Singleton
	file ext:	h
	author:		huaiyu
	
	purpose:	单件模式
				e.g.
				class MySingleton : public _HYLIB singleton<MySingleton> {
					HY_DECLARE_SINGLETON(MySingleton, 100)
				};
				#define g_pMySingleton MySingleton::inst()
				// then you can use the global-variable-like pointer "g_pMySingleton"
*********************************************************************/
#ifndef _SINGLETON_H_
#define _SINGLETON_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include "_prolog.h"

#include <vector>
#include <algorithm>
#include "util_tmpl.h"
#include "macro.h"

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable : 4018)
#endif // _MSC_VER

////////////////////////////////////////////////////////////////////////////////
_HYLIB_BEGIN

////////////////////////////////////////////////////////////////////////////////
// Prototypes:
template <class T>
	struct singleton;
struct singleton_mgr;

////////////////////////////////////////////////////////////////////////////////
struct _singleton_base : public noncopyable
{
public:
	virtual ~_singleton_base() {}
	
	typedef unsigned int longevity_t;
	enum {
		MIN_LONGEVITY = 0,
		MAX_LONGEVITY = ((longevity_t)-1)
	};
protected:
	explicit _singleton_base(longevity_t longevity=MIN_LONGEVITY) : _longevity(longevity) {}
private:
	const longevity_t _longevity;

//==============================================================================
	friend struct singleton_mgr;
};

//******************************************************************************
template <class T>
struct singleton : _singleton_base
{
	static T *inst() {
		if (NULL == _pInst) {
			_pInst = new T();
			singleton_mgr::inst().add_inst(_pInst);
		}
		return _pInst;
	}
	static void deinst() {
		if (NULL != _pInst) {
			singleton_mgr::inst().del_inst(_pInst);
			_pInst = NULL;
		}
	}

private:
	static T *_pInst;

protected:
	explicit singleton(longevity_t longevity=MIN_LONGEVITY) : _singleton_base(longevity) {}
public:
	virtual ~singleton() {}
};

template <class T>
T * singleton<T>::_pInst = NULL;

////////////////////////////////////////////////////////////////////////////////
struct singleton_mgr : public noncopyable
{
public:
	static singleton_mgr &inst() {
		static singleton_mgr _inst;
		return _inst;
	}

protected:
 	typedef _singleton_base *_Entry;
 	typedef std::vector<_Entry> _EntryVec;
	typedef _EntryVec::iterator _EntryIter;

	static bool more_longevity(_Entry e1, _Entry e2) {
		return e1->_longevity > e2->_longevity;
	}

public:
	void add_inst(_singleton_base *pInst) {
		_EntryIter iIns = std::upper_bound(_vecEnt.begin(), _vecEnt.end(), pInst,
			more_longevity);
 		_vecEnt.insert(iIns, pInst);
	}
	void del_inst(_singleton_base *pInst) {
		_EntryIter iDel = std::find(_vecEnt.begin(), _vecEnt.end(), pInst);
		if (iDel != _vecEnt.end()) {
			SafeDelete(*iDel);
			_vecEnt.erase(iDel);
		}
	}

protected:
	singleton_mgr() {}
public:
	~singleton_mgr() {
		for (_EntryVec::reverse_iterator iter = _vecEnt.rbegin();
			iter != _vecEnt.rend();
			++iter) {
			SafeDelete(*iter);
		}
		_vecEnt.clear();
	}

protected:
	_EntryVec _vecEnt;
};

_HYLIB_END

#ifdef _MSC_VER
#	pragma warning(pop)
#endif // _MSC_VER

////////////////////////////////////////////////////////////////////////////////
#define HY_DECLARE_SINGLETON(cls, longevity)	\
			friend struct _HYLIB singleton< cls >;	\
			protected: cls() : _HYLIB singleton<cls>(longevity)

#define HY_DECLARE_SINGLETON_0(cls) HY_DECLARE_SINGLETON(cls, 0)

////////////////////////////////////////////////////////////////////////////////
#endif // _SINGLETON_H_