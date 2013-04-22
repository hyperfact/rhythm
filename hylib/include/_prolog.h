/********************************************************************
	created:	2011/07/18
	created:	18:7:2011   16:11
	filename: 	_prolog.h
	file base:	_prolog
	file ext:	h
	author:		huaiyu
	
	purpose:	一些项目配置信息,仅限项目内部包含.
	desc:		1. 引用:
					a. 直接引用源,随自己项目一块编译,无需定义宏;
					b. 静态链接到本库,无需定义宏;
					c. 动态链接到本库,定义宏:_HYLIB_DLL;
				2. 名字空间:
					默认名字空间为 ::hy
				
*********************************************************************/

#ifndef __PROLOG_H_
#define __PROLOG_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
// Declaration:
#ifdef _HYLIB_DLL			// use dll
#	ifdef _HYLIB_DECL_DLL	// export dll
#		define HYLIB_DECL __declspec(dllexport)
#	else // ndef _HYLIB_DECL_DLL // import dll
#		define HYLIB_DECL __declspec(dllimport)
#	endif // _HYLIB_DECL_DLL
#else // ndef _HYLIB_DLL	// directly reference
#	define HYLIB_DECL
#endif // _HYLIB_DLL

////////////////////////////////////////////////////////////////////////////////
// Namespace:
#if defined(__cplusplus)
#	ifndef _NAMESPACE_NAME
#		define _NAMESPACE_NAME	hy
#	endif // _NAMESPACE_NAME
#
#	define _HYLIB			_NAMESPACE_NAME::
#	define _HYLIB_BEGIN		namespace _NAMESPACE_NAME {
#	define _HYLIB_END		};
#	define _HYLIB_USING		using namespace _NAMESPACE_NAME;

#	define _HYLIB_DETAIL			detail::
#	define _HYLIB_DETAIL_BEGIN		namespace detail {
#	define _HYLIB_DETAIL_END		};
#	define _HYLIB_DETAIL_USING		using namespace detail;
#else /* !defined(__cplusplus) */
#	define _HYLIB
#	define _HYLIB_BEGIN
#	define _HYLIB_END
#	define _HYLIB_USING

#	define _HYLIB_DETAIL
#	define _HYLIB_DETAIL_BEGIN
#	define _HYLIB_DETAIL_END
#	define _HYLIB_DETAIL_USING
#endif /* __cplusplus */

// Semantics:
#ifndef __in
#define __in
#endif
#ifndef __out
#define __out
#endif
#ifndef __inout
#define __inout
#endif

////////////////////////////////////////////////////////////////////////////////
#ifndef _T
#define _T(t)	t
#endif

////////////////////////////////////////////////////////////////////////////////
#ifdef _AFX
#define _USING_MFC
#endif

#if defined(_MSC_VER) && _MSC_VER>1200		// msvc
#	define	__funcname__	__FUNCTION__
#elif defined(__GNUC__)						// gcc
#	define	__funcname__	__PRETTY_FUNCTION__
#else
#	define	__funcname__	"UNKNOWN"
#endif

////////////////////////////////////////////////////////////////////////////////
#ifdef _MSC_VER
#	ifndef int64_t
		typedef __int64				int64_t;
		typedef unsigned __int64	uint64_t;
#	endif // int64_t
#endif // _MSC_VER

////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && (_MSC_VER<=1200)
#	pragma warning(disable:4786)
#endif

////////////////////////////////////////////////////////////////////////////////
#endif // __prolog_h_
