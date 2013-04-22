#ifndef _UTIL_TMPL_H_
#define _UTIL_TMPL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include "_prolog.h"
#include <iostream>
#include <iomanip>
#include <functional>
#include <sstream>
#include <iosfwd>

_HYLIB_BEGIN

//******************************************************************************
// 不可复制语义
struct noncopyable {
protected:
	noncopyable() {}
private:
	noncopyable(const noncopyable &);
	const noncopyable& operator = (const noncopyable &);
};

//******************************************************************************
struct empty_ostream {
	template <class T>
		empty_ostream &operator << (T)
	{
		return *this;
	}

	static empty_ostream &inst() {
		static empty_ostream os;
		return os;
	}
};

struct empty_istream {
	template <class T>
		empty_istream &operator >> (T)
	{
		return *this;
	}

	static empty_istream &inst() {
		static empty_istream is;
		return is;
	}
};

struct empty_iostream : public empty_istream, public empty_ostream {
	static empty_iostream &inst() {
		static empty_iostream ios;
		return ios;
	}
};

#ifdef _WIN32
//******************************************************************************
// 算法时间测试
template <class _OStream=std::ostream>
class CProcTimeTest
{
	public:
		explicit CProcTimeTest(_OStream &os=std::cout) : m_os(os) {
			::QueryPerformanceCounter(&m_st);
		}
		virtual ~CProcTimeTest() {
			LARGE_INTEGER et, freq;
			::QueryPerformanceCounter(&et);
			::QueryPerformanceFrequency(&freq);
			m_os << "Procedure expends "
				<< (unsigned long)((et.QuadPart-m_st.QuadPart)*(LONGLONG)1000000/freq.QuadPart)
				<< " us\n";
		}
	protected:
		_OStream &m_os;
		LARGE_INTEGER m_st;
};
#endif // _WIN32

#ifdef _USING_MFC
// 调试器输出
typedef CProcTimeTest<CDumpContext> DebugPPT;
#endif // _USING_MFC

//******************************************************************************
// 成员访问
template <class _Cls, class _Mem>
struct mem_access : public std::binary_function<_Cls &, _Mem _Cls::*, _Mem &>
{
	_Mem & operator () (_Cls &obj, _Mem _Cls::*memPtr)
	{
		return obj.*memPtr;
	}
};

//******************************************************************************
// 成员谓词泛函
template <class _Cls, class _Mem, class _Pr>
struct mem_pred :
	public std::binary_function<_Cls, _Mem, bool>
{
	typedef _Cls class_type;
	typedef _Mem mem_type;
	typedef mem_type _Cls::* mem_ptr;
	typedef _Pr predicate_type;
	
	mem_pred(mem_ptr memPtr, _Pr pr=_Pr())
		: _memPtr(memPtr), _pr(pr) {
	}
	
	bool operator () (const class_type &obj, const mem_type &val) const
	{
		return _pr(obj.*_memPtr, val);
	}
	
protected:
	mem_ptr _memPtr;
	predicate_type _pr;
};

template <class _Cls, class _Mem, class _Pr>
inline
mem_pred<_Cls, _Mem, _Pr>
	mem_pred_fun(_Mem _Cls::* memPtr, _Pr pr=_Pr())
{
	return mem_pred<_Cls, _Mem, _Pr>(memPtr, pr);
}

//******************************************************************************
// 
template <class _Init, class _Uninit>
struct scope_guard : noncopyable
{
	scope_guard(_Init init=_Init(), _Uninit uninit=_Uninit())
		: _uninit(uninit)
	{
		init();
	}
	~scope_guard() {
		_uninit();
	}
private:
	_Uninit _uninit;
};

template <void (*_Init)(), void (*_Uninit)()>
struct scope_guard_fun : noncopyable
{
	scope_guard_fun() {
		_Init();
	}
	~scope_guard_fun() {
		_Uninit();
	}
};

//******************************************************************************
template <class _Os, class _Arg>
struct manip1_helper {
	typedef _Os &(*_ManipFun) (_Os &, _Arg);
	manip1_helper(_ManipFun fun, _Arg arg) : _manipFun(fun), _arg(arg) {}
	
protected:
	_ManipFun _manipFun;
	_Arg _arg;
		
	friend _Os &operator << (_Os &, _ManipFun);
};

//******************************************************************************
// 编译时log2(antilog):
//	algo: log2(n)=log2(2*(n/2))=1+log2(n/2)
template <unsigned antilog>
struct static_log2 {
	enum { value = 1 + static_log2<(antilog>>1)>::value };
};

template<>
struct static_log2<1> {
	enum { value = 0 };
};

template<>
struct static_log2<0>;

//******************************************************************************
template <class T>
inline T *nextField(void **pBuf, unsigned nDup = 1)
{
	assert(pBuf);
	T *pT = static_cast<T *>(*pBuf);
	*pBuf = pT + nDup;
	return pT;
}

template <class T>
inline const T *nextField(const void **pBuf, unsigned nDup = 1)
{
	assert(pBuf);
	T *pT = static_cast<T *>(*pBuf);
	*pBuf = pT + nDup;
	return pT;
}

//******************************************************************************
// 转储二进制流
inline std::string dump_bytes(const void *data, int len) {
	using namespace ::std;
	ostringstream oss;
	const char *pData = (const char *)data;
	const char * const pEnd = pData + len;
	oss.fill('0');
	oss << hex;
	for (; pData!=pEnd; ++pData) {
		oss << setw(2) << (unsigned)(unsigned char)*pData << " ";
	}
	oss << dec;
	oss.fill(' ');
	
	return oss.str();
}

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

////////////////////////////////////////////////////////////////////////////////
#endif // _UTIL_TMPL_H_
