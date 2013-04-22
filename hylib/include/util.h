#ifndef _UTIL_H_
#define _UTIL_H_

#pragma once
#include "_prolog.h"

#include "compare.h"
#include <WTypes.h>
#include <functional>
#include <IOSTREAM>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
// Math constants:
#ifndef M_PI
#	define M_PI	3.14159265358979323846
#endif

////////////////////////////////////////////////////////////////////////////////
// Functions:
_HYLIB_BEGIN

//******************************************************************************
int CycCounterInc(int &nCounter, int nMin, int nMax);
int CycCounterDec(int &nCounter, int nMin, int nMax);

//******************************************************************************
#ifdef _USING_MFC
void RectMoveToX(CRect &rc, int x);
void RectMoveToY(CRect &rc, int y);
void RectMoveToXY(CRect &rc, int x, int y);

enum LAY_OUT_STYLE {
	LOS_LEFT = 1,
	LOS_RIGHT = 1<<1,
	LOS_TOP = 1<<2,
	LOS_BOTTOM = 1<<3,
	LOS_HCENTER = 1<<4,
	LOS_VCENTER = 1<<5,
	LOS_CENTER = LOS_HCENTER|LOS_VCENTER
};
// 相对于指定区域(内部)摆放新区域
HYLIB_DECL void LayOutRect(__in const CRect &rcParent, __in const CSize &czChild,
				__out CRect &rcOut, __in DWORD nStyle = LOS_CENTER,
				__in int nMarginX = 0, __in int nMarginY = 0);

void LayOutRect(__in const CRect &rcParent, __in const CRect &rcChild,
				__out CRect &rcOut, __in DWORD nStyle = LOS_CENTER,
				__in int nMarginX = 0, __in int nMarginY = 0);

// 相对于指定窗口(内部)摆放
HYLIB_DECL void LayOutWnd(__in CWnd *pWnd, __in const CWnd *pParent=NULL, __in DWORD nAlignFlag = LOS_CENTER,
				 __in int iSpaceX = 0, __in int iSpaceY = 0,
				 __in BOOL bRepaint=TRUE);

// 相对于指定区域(外部)摆放新区域
HYLIB_DECL void LayOutRectEx(__inout CRect &rc, __in const CRect &rcRef, __in DWORD nAlignFlag = LOS_CENTER,
				__in int iSpaceX = 0, __in int iSpaceY = 0);

// 相对于指定窗口(外部)摆放
HYLIB_DECL void LayOutWndEx(__in CWnd *pWnd, __in const CWnd *pWndRef, __in DWORD nAlignFlag = LOS_CENTER,
				 __in int iSpaceX = 0, __in int iSpaceY = 0, __in BOOL bRepaint=TRUE);

// 根据锚点计算左上角坐标
HYLIB_DECL void TransformAnchor(__in int refX, __in int refY, __in const CSize &czRect,
					 __out CPoint &ptOut, __in UINT anchor=LOS_LEFT|LOS_TOP);

void TransformAnchor(__in int refX, __in int refY, __in const CSize &czRect,
					 __out CRect &rcOut, __in UINT anchor=LOS_LEFT|LOS_TOP);

// 定位窗口
void PosWnd(CWnd *pWnd, int x, int y, BOOL bRepaint=TRUE);
// 改变窗口大小
void SizeWnd(CWnd *pWnd, int iWidth, int iHeight, BOOL bRepaint=TRUE);
// 相对移动窗口
void OffsetWnd(CWnd *pWnd, int dx, int dy, BOOL bRepaint=TRUE);

// 全递归设置子窗口字体
HYLIB_DECL void SetChildFont(CWnd *pWnd, CFont *pFont, BOOL bRedraw = TRUE);

//******************************************************************************
// 绘制位图
HYLIB_DECL void DrawImage(HDC hdc, HBITMAP hBMP, int x, int y, int w, int h, int xSrc=0, int ySrc=0);

// 单位化矩阵
void XFormIdentity(XFORM &xForm);

// 计算平移矩阵
void XFormTranslate(float dx, float dy, XFORM &xForm);

// 计算旋转矩阵
void XFormRotate(float fAngle, XFORM &xForm);
// 绘制旋转图片
HYLIB_DECL void DrawImageRotationally(HDC hdc, HBITMAP hBMP, int x, int y, int w, int h,
						   float fAngle, int xSrc=0, int ySrc=0);

enum MirFlag {
	MF_HORI = 1,
	MF_VERT = 1<<1,
};
// 计算镜像矩阵
void XFormMirror(UINT nFlag, XFORM &xForm);
// 绘制镜像图片
HYLIB_DECL void DrawImageMirror(HDC hdc, HBITMAP hBMP, int x, int y, int w, int h,
					 UINT nFlag, int xSrc=0, int ySrc=0);

// 计算切变矩阵
void XFormShear(float xShear, float yShear, XFORM &xForm);
// 绘制切变图片
HYLIB_DECL void DrawImageShear(HDC hdc, HBITMAP hBMP, int x, int y, int w, int h,
		float xShear, float yShear, int xSrc=0, int ySrc=0);

#endif // _USING_MFC

//******************************************************************************
// 创建目录树
HYLIB_DECL void mkdir_tree(const char *szPath, bool bFile=false);

////////////////////////////////////////////////////////////////////////////////
//******************************************************************************

#ifdef _UNIT_TEST
struct _Test {
	_Test();
};
#endif // _UNIT_TEST

_HYLIB_END

////////////////////////////////////////////////////////////////////////////////
// 内联函数实现
#include "util.inl"

////////////////////////////////////////////////////////////////////////////////
#endif // _UTIL_H_
