/********************************************************************
	created:	2010/10/09
	created:	9:10:2010   9:45
	filename: 	compare.h
	file base:	compare
	file ext:	h
	author:		huaiyu
	
	purpose:	
*********************************************************************/
#ifndef _COMPARE_H_
#define _COMPARE_H_

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Integer comparison
#define EQU(a, b)			((a)==(b))
#define EQU3_AND(a, b, c)	(((a)==(b)) && ((a)==(c)))
#define EQU3_OR(a, b, c)	(((a)==(b)) || ((a)==(c)))

#define BTW(x, a, b)		(((a)<(x)) && ((x)<(b)))
#define BTW_E(x, a, b)		(((a)<=(x)) && ((x)<=(b)))
#define BTW_LE(x, a, b)		(((a)<=(x)) && ((x)<(b)))
#define BTW_RE(x, a, b)		(((a)<(x)) && ((x)<=(b)))

////////////////////////////////////////////////////////////////////////////////
// Float comparison
#include <float.h>

#ifndef CMP_EPSILON
#define CMP_EPSILON			FLT_EPSILON
#endif

#define F_EQU(a, b)			((((b)-(CMP_EPSILON))<(a)) && ((a)<((b)+(CMP_EPSILON))))
#define F_EQU3_AND(a, b, c)	(F_EQU(a,b) && F_EQU(a,c))
#define F_EQU3_OR(a, b, c)	(F_EQU(a,b) || F_EQU(a,c))

#define F_GT(a, b)			((a) > ((b)+(CMP_EPSILON)))
#define F_LT(a, b)			((a) < ((b)-(CMP_EPSILON)))
#define F_GTE(a, b)			((a) > ((b)-(CMP_EPSILON)))
#define F_LTE(a, b)			((a) < ((b)+(CMP_EPSILON)))

#define F_BTW(x, a, b)		(F_LT(a, x) && F_LT(x, b))
#define F_BTW_E(x, a, b)	(F_LTE(a, x) && F_LTE(x, b))
#define F_BTW_LE(x, a, b)	(F_LTE(a, x) && F_LT(x, b))
#define F_BTW_RE(x, a, b)	(F_LT(a, x) && F_LTE(x, b))

////////////////////////////////////////////////////////////////////////////////
// Bit Test
#define BT_IS_SET(code, maskCode)		(((code)&(maskCode)) != 0)
#define BT_IS_CLEAR(code, maskCode)		(!BT_IS_SET((code), (maskCode)))
#define BT_IS_BIT_SET(code, bit)		BT_IS_SET((code), 1U<<(bit))
#define BT_IS_BIT_CLEAR(code, bit)		BT_IS_CLEAR((code), 1U<<(bit))

#define BT_SET(code, maskCode)			((code) |= (maskCode))
#define BT_CLEAR(code, maskCode)		((code) &= (~(maskCode)))
#define BT_BIT_SET(code, bit)			BT_SET((code), 1U<<(bit))
#define BT_BIT_CLEAR(code, bit)			BT_CLEAR((code), 1U<<(bit))

#define BT_SET_EXPR(code, maskCode)		((code) | (maskCode))
#define BT_CLEAR_EXPR(code, maskCode)	((code) & (~(maskCode)))
#define BT_BIT_SET_EXPR(code, bit)		BT_SET((code), 1U<<(bit))
#define BT_BIT_CLEAR_EXPR(code, bit)	BT_CLEAR((code), 1U<<(bit))

////////////////////////////////////////////////////////////////////////////////
// Pointer comparison:
#define PTR_EQU(a, b)				((char *)(a) == (char *)(b))
#define PTR_LT(a, b)				((char *)(a) < (char *)(b))
#define PTR_GT(a, b)				((char *)(a) > (char *)(b))
#define PTR_LE(a, b)				((char *)(a) <= (char *)(b))
#define PTR_GE(a, b)				((char *)(a) >= (char *)(b))
#define PTR_DISTANCE(a, b)			((char *)(b) - (char *)(a))

////////////////////////////////////////////////////////////////////////////////
#endif // _COMPARE_H_
