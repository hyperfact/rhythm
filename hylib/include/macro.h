/********************************************************************
	created:	2011/08/26
	created:	26:8:2011   17:01
	filename: 	macro.h
	file base:	macro
	file ext:	h
	author:		huaiyu
	
	purpose:	
*********************************************************************/

#ifndef _MACRO_H_
#define _MACRO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
#include "_prolog.h"

////////////////////////////////////////////////////////////////////////////////
// Assertion:

////////////////////////////////////////////////////////////////////////////////
// Useful macros:
//******************************************************************************
// Memory & pointers:
#ifdef SafeDelete
#undef SafeDelete
#endif // SafeDelete
#define SafeDelete(p) if((p)!=NULL) { delete (p); (p)=NULL; }

#ifdef SafeDeleteArray
#undef SafeDeleteArray
#endif // SafeDeleteArray
#define SafeDeleteArray(p) if((p)!=NULL) { delete [](p); (p)=NULL; }

#ifdef SafeRelease
#undef SafeRelease
#endif // SafeRelease
#define SafeRelease(p) if((p)!=NULL) { (p)->Release(); (p)=NULL; }

//******************************************************************************
// Arrays:
#define ZERO_ARRAY(arr) ZeroMemory((arr), sizeof(arr))
#define ARR_LEN(arr) (sizeof(arr)/sizeof((arr)[0]))

//******************************************************************************
// Assertions:
#ifdef _DEBUG
#define ASSERT_RET(exp, ret)	if (!(exp)) { assert(exp); return (ret); }
#define ASSERT_RET_VOID(exp)	if (!(exp)) { assert(exp); return; }
#define ASSERT_THROW(exp, thr)	if (!(exp)) { assert(exp); throw (thr); }
#else // _DEBUG
#define ASSERT_RET(exp, ret)	if (!(exp)) { return (ret); }
#define ASSERT_RET_VOID(exp)	if (!(exp)) { return; }
#define ASSERT_THROW(exp, thr)	if (!(exp)) { throw (thr); }
#endif // _DEBUG

//******************************************************************************
// STL Containers:
#define CONT_DECL_ITER(cont) \
	typedef cont::iterator cont##Iter; \
	typedef cont::const_iterator cont##CIter;

//******************************************************************************
// Bytes align:
#define BytesAlign(p, bytes)	(((p)+(bytes)-1) & ~((bytes)-1))
#define DWORDAlign(p) BytesAlign(p, 4)

//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
#endif // _MACRO_H_
