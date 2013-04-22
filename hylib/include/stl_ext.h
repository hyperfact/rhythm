#ifndef _STL_EXT_H_
#define _STL_EXT_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include "_prolog.h"

_STD_BEGIN

////////////////////////////////////////////////////////////////////////////////
// VC6以下的版本不支持的组件
#if defined(_MSC_VER) && (_MSC_VER<=1200)
//******************************************************************************
// 累加算法
template<class InputIterator, class Type>
Type accumulate(InputIterator _First,
				InputIterator _Last,
				Type _Val)
{
	for ( ; _First!=_Last; ++_First) {
		_Val = (Type)(_Val + *_First);
	}
	return _Val;
}

template<class InputIterator, class Type, class BinaryOperation>
Type accumulate(InputIterator _First, 
				InputIterator _Last, 
				Type _Val, 
				BinaryOperation _Binary_op)
{
	for ( ; _First!=_Last; ++_First) {
		_Val = _Binary_op(_Val, *_First);
	}
	return _Val;
}

//******************************************************************************
// ostream: operator<<(__int64)
template<class _E, class _Tr>
inline
basic_ostream<_E, _Tr> & __cdecl
	operator<<(basic_ostream<_E, _Tr>& os, int64_t nVal)
{
	enum { INT64_DIGIT = 20 };

	_E buf[INT64_DIGIT+2] = {0}; // +sign+'\0'
	_sntprintf(buf, ARR_LEN(buf), _T("%I64d"), nVal);
	return (os << buf);
}

#endif // defined(_MSC_VER) && _MSC_VER<=1200
////////////////////////////////////////////////////////////////////////////////

_STD_END

////////////////////////////////////////////////////////////////////////////////
#endif // _STL_EXT_H_