#ifndef _UTIL_INL_
#define _UTIL_INL_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

_HYLIB_BEGIN
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
inline int CycCounterInc( int &nCounter, int nMin, int nMax )
{
	if (++nCounter > nMax)
	{
		nCounter = nMin;
	}
	return nCounter;
}

inline int CycCounterDec( int &nCounter, int nMin, int nMax )
{
	if (--nCounter < nMin)
	{
		nCounter = nMax;
	}
	return nCounter;
}

//******************************************************************************
#ifdef _USING_MFC
inline void RectMoveToX(CRect &rc, int x)
{
	rc.right = x + rc.Width();
	rc.left = x;
}

inline void RectMoveToY(CRect &rc, int y)
{
	rc.bottom = y + rc.Height();
	rc.top = y;
}

inline void RectMoveToXY(CRect &rc, int x, int y)
{
	RectMoveToX(rc, x);
	RectMoveToY(rc, y);
}

inline void PosWnd(CWnd *pWnd, int x, int y, BOOL bRepaint/*=TRUE*/)
{
	ASSERT(pWnd!=NULL && ::IsWindow(pWnd->GetSafeHwnd()));
	pWnd->SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);
}

inline void SizeWnd(CWnd *pWnd, int iWidth, int iHeight, BOOL bRepaint/*=TRUE*/)
{
	ASSERT(pWnd!=NULL && ::IsWindow(pWnd->GetSafeHwnd()));
	pWnd->SetWindowPos(NULL, 0, 0, iWidth, iHeight, SWP_NOZORDER|SWP_NOMOVE);
}

inline void OffsetWnd(CWnd *pWnd, int dx, int dy, BOOL bRepaint/*=TRUE*/)
{
	ASSERT(pWnd!=NULL && ::IsWindow(pWnd->GetSafeHwnd()));
	CRect rcWnd;
	pWnd->GetWindowRect(rcWnd);
	if (WS_CHILD & GetWindowLong(*pWnd, GWL_STYLE)){
		pWnd->GetParent()->ScreenToClient(rcWnd);
	}
	rcWnd.OffsetRect(dx, dy);
	pWnd->MoveWindow(rcWnd, bRepaint);
}

inline void LayOutRect(__in const CRect &rcParent, __in const CRect &rcChild,
					   __out CRect &rcOut, __in DWORD nStyle /*= LOS_CENTER*/,
					   __in int nMarginX/* = 0*/, __in int nMarginY/* = 0*/)
{
	LayOutRect(rcParent, rcChild.Size(), rcOut, nStyle, nMarginX, nMarginY);
}

inline void TransformAnchor(__in int refX, __in int refY, __in const CSize &czRect,
							__out CRect &rcOut, __in UINT anchor/*=LOS_LEFT|LOS_TOP*/)
{
	CPoint ptNew;
	TransformAnchor(refX, refY, czRect, ptNew, anchor);
	rcOut.SetRect(ptNew, ptNew+czRect);
}

inline void XFormIdentity( XFORM &xForm )
{
	xForm.eM11 = xForm.eM22 = 1.0f;
	xForm.eM12 = xForm.eM21 = xForm.eDx = xForm.eDy = 0.0f;
}

inline void XFormTranslate( float dx, float dy, XFORM &xForm )
{
	XFormIdentity(xForm);
	xForm.eDx = dx;
	xForm.eDy = dy;
}

inline void XFormRotate( float fAngle, XFORM &xForm )
{
	XFormIdentity(xForm);
	xForm.eM11 = xForm.eM22 = cos(fAngle);
	xForm.eM12 = sin(fAngle);
	xForm.eM21 = -xForm.eM12;
}

inline void XFormMirror( UINT nFlag, XFORM &xForm )
{
	XFormIdentity(xForm);
	xForm.eM11 = nFlag&MF_HORI ? -1.0f : 1.0f;
	xForm.eM22 = nFlag&MF_VERT ? -1.0f : 1.0f;
}

inline void XFormShear( float xShear, float yShear, XFORM &xForm )
{
	XFormIdentity(xForm);
	xForm.eM12 = yShear;
	xForm.eM21 = xShear;
}

#endif // _USING_MFC

_HYLIB_END
////////////////////////////////////////////////////////////////////////////////
#endif // _UTIL_INL_
