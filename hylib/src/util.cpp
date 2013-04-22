#include "stdafx.h"
#include "util.h"
#include <assert.h>
#include <io.h>
#include <direct.h>

_HYLIB_BEGIN

#ifdef _USING_MFC

void LayOutRect( __in const CRect &rcParent, __in const CSize &czChild,
				__out CRect &rcOut,
				__in DWORD nStyle /*= LOS_CENTER*/,
				__in int nMarginX /*= 0*/,
				__in int nMarginY /*= 0*/ )
{
	rcOut.SetRectEmpty();

	if (0 == nStyle)
	{
		nStyle = LOS_CENTER;
	}

	// Horizontal:
	if (nStyle & LOS_LEFT)
	{
		rcOut.left = rcParent.left + nMarginX;
		rcOut.right = rcOut.left + czChild.cx;
	}
	else if (nStyle & LOS_RIGHT)
	{
		rcOut.right = rcParent.right - nMarginX;
		rcOut.left = rcOut.right - czChild.cx;
	}
	else if (nStyle & LOS_HCENTER)
	{
		rcOut.left = rcParent.left + ((rcParent.Width()-czChild.cx)>>1);
		rcOut.right = rcOut.left + czChild.cx;
	}

	// Vertical:
	if (nStyle & LOS_TOP)
	{
		rcOut.top = rcParent.top + nMarginY;
		rcOut.bottom = rcOut.top + czChild.cy;
	} 
	else if (nStyle & LOS_BOTTOM)
	{
		rcOut.bottom = rcParent.bottom - nMarginY;
		rcOut.top = rcOut.bottom - czChild.cy;
	}
	else if (nStyle & LOS_VCENTER)
	{
		rcOut.top = rcParent.top + ((rcParent.Height()-czChild.cy)>>1);
		rcOut.bottom = rcOut.top + czChild.cy;
	}
}

void LayOutWnd( __in CWnd *pWnd, __in const CWnd *pParent/*=NULL*/,
			   __in DWORD nAlignFlag /*= LOS_CENTER*/,
			   __in int iSpaceX /*= 0*/,
			   __in int iSpaceY /*= 0*/,
			   __in BOOL bRepaint/*=TRUE*/ )
{
	ASSERT(pWnd!=NULL && ::IsWindow(*pWnd));
	if (NULL == pParent)
	{
		pParent = CWnd::FromHandle(GetDesktopWindow());
	}
	ASSERT(pParent!=NULL && ::IsWindow(*pParent));

	CRect rcWnd, rcParent;
	pWnd->GetWindowRect(rcWnd);
	pParent->GetClientRect(rcParent);
	LayOutRect(rcParent, rcWnd, rcWnd, nAlignFlag, iSpaceX, iSpaceY);
	
	BOOL bChild = BT_IS_SET(::GetWindowLong(*pWnd, GWL_STYLE), WS_CHILD);
	if (!bChild) { pParent->ClientToScreen(rcWnd); }
	pWnd->MoveWindow(rcWnd, bRepaint);
}

void LayOutRectEx( __inout CRect &rc, __in const CRect &rcRef,
				  __in DWORD nAlignFlag /*= LOS_CENTER*/,
				  __in int iSpaceX /*= 0*/,
				  __in int iSpaceY /*= 0*/ )
{
	if (0 == nAlignFlag)
	{
		nAlignFlag = LOS_CENTER;
	}

	// Horizontal:
	if (nAlignFlag & LOS_LEFT)
	{
		RectMoveToX(rc, rcRef.left+iSpaceX);
	}
	else if (nAlignFlag & LOS_RIGHT)
	{
		RectMoveToX(rc, rcRef.right+iSpaceX);
	}
	else if (nAlignFlag & LOS_HCENTER)
	{
		RectMoveToX(rc, rcRef.CenterPoint().x-(rc.Width()>>1));
	}

	// Vertical:
	if (nAlignFlag & LOS_TOP)
	{
		RectMoveToY(rc, rcRef.top+iSpaceY);
	} 
	else if (nAlignFlag & LOS_BOTTOM)
	{
		RectMoveToY(rc, rcRef.bottom+iSpaceY);
	}
	else if (nAlignFlag & LOS_VCENTER)
	{
		RectMoveToY(rc, rcRef.CenterPoint().y-(rc.Height()>>1));
	}
}

void LayOutWndEx( __in CWnd *pWnd, __in const CWnd *pWndRef,
				 __in DWORD nAlignFlag /*= LOS_CENTER*/,
				 __in int iSpaceX /*= 0*/,
				 __in int iSpaceY /*= 0*/,
				 __in BOOL bRepaint/*=TRUE*/ )
{
	ASSERT(pWnd!=NULL && ::IsWindow(*pWnd));
	ASSERT(pWndRef!=NULL && ::IsWindow(*pWndRef));

	CRect rcWnd, rcRef;
	pWnd->GetWindowRect(rcWnd);
	pWndRef->GetWindowRect(rcRef);
	LayOutRectEx(rcWnd, rcRef, nAlignFlag, iSpaceX, iSpaceY);
	if (const CWnd *pParent = pWnd->GetParent())
	{
		pParent->ScreenToClient(rcWnd);
	}
	pWnd->MoveWindow(rcWnd, bRepaint);
}

void TransformAnchor( __in int refX, __in int refY, __in const CSize &czRect,
					 __out CPoint &ptOut, __in UINT anchor/*=LOS_LEFT|LOS_TOP*/ )
{
	if (0 == anchor)
	{
		anchor = LOS_LEFT | LOS_TOP;
	}

	ptOut.x = refX;
	ptOut.y = refY;

	// Horizontal:
	if (anchor & LOS_LEFT)
	{
		// Do nothing
	}
	else if (anchor & LOS_RIGHT)
	{
		ptOut.Offset(-czRect.cx, 0);
	}
	else if (anchor & LOS_HCENTER)
	{
		ptOut.Offset(-czRect.cx/2, 0);
	}

	// Vertical:
	if (anchor & LOS_TOP)
	{
		// Do nothing
	}
	else if (anchor & LOS_BOTTOM)
	{
		ptOut.Offset(0, -czRect.cy);
	}
	else if (anchor & LOS_VCENTER)
	{
		ptOut.Offset(0, -czRect.cy/2);
	}
}

void DrawImage( HDC hdc, HBITMAP hBMP, int x, int y, int w, int h, int xSrc/*=0*/, int ySrc/*=0*/ )
{
	HDC hdcImg = ::CreateCompatibleDC(hdc);
	if (NULL == hdcImg) { return; }
	
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hdcImg, hBMP);
	::BitBlt(hdc, x, y, w, h, hdcImg, xSrc, ySrc, SRCCOPY);	
	::SelectObject(hdcImg, hOldBmp);
	::DeleteObject(hdcImg);
}

void DrawImageRotationally( HDC hdc, HBITMAP hBMP, int x, int y, int w, int h, float fAngle,
						   int xSrc/*=0*/, int ySrc/*=0*/)
{
	int nOldGM = ::SetGraphicsMode(hdc, GM_ADVANCED);
	XFORM xFormOld;
	::GetWorldTransform(hdc, &xFormOld);
	
	const float fCosA = cos(fAngle);
	const float fSinA = sin(fAngle);
	const XFORM xForm = {
		fCosA, fSinA,
			-fSinA, fCosA,
			(float)x, (float)y
	};
	::SetWorldTransform(hdc, &xForm);
	
	DrawImage(hdc, hBMP, 0, 0, w, h, xSrc, ySrc);
	
	::SetWorldTransform(hdc, &xFormOld);
	::SetGraphicsMode(hdc, nOldGM);
}

void DrawImageMirror( HDC hdc, HBITMAP hBMP, int x, int y, int w, int h, UINT nFlag, int xSrc/*=0*/, int ySrc/*=0*/ )
{
	int nOldGM = ::SetGraphicsMode(hdc, GM_ADVANCED);
	XFORM xFormOld;
	::GetWorldTransform(hdc, &xFormOld);
	
	const XFORM xForm = {
		nFlag&MF_HORI?-1.0f:1.0f, 0.0f,
			0.0f, nFlag&MF_VERT?-1.0f:1.0f,
			(float)x, (float)y
	};
	::SetWorldTransform(hdc, &xForm);
	
	DrawImage(hdc, hBMP, 0, 0, w, h, xSrc, ySrc);
	
	::SetWorldTransform(hdc, &xFormOld);
	::SetGraphicsMode(hdc, nOldGM);
}

void DrawImageShear( HDC hdc, HBITMAP hBMP, int x, int y, int w, int h, float xShear, float yShear, int xSrc/*=0*/, int ySrc/*=0*/ )
{
	int nOldGM = ::SetGraphicsMode(hdc, GM_ADVANCED);
	XFORM xFormOld;
	::GetWorldTransform(hdc, &xFormOld);
	
	const XFORM xForm = {
		1.0f, yShear,
			xShear, 1.0f,
			(float)x, (float)y
	};
	::SetWorldTransform(hdc, &xForm);
	
	DrawImage(hdc, hBMP, 0, 0, w, h, xSrc, ySrc);
	
	::SetWorldTransform(hdc, &xFormOld);
	::SetGraphicsMode(hdc, nOldGM);
}

//******************************************************************************
void SetChildFont(CWnd *pWnd, CFont *pFont, BOOL bRedraw/*=TRUE*/)
{
	if (::IsWindow(pWnd->GetSafeHwnd()))
	{
		pWnd->SetFont(pFont, bRedraw);
	}

	CWnd *pChild = pWnd->GetWindow(GW_CHILD);
	while (pChild != NULL)
	{
		SetChildFont(pChild, pFont, bRedraw);
		pChild = pChild->GetWindow(GW_HWNDNEXT);
	}
}
#endif // _USING_MFC

//******************************************************************************
void mkdir_tree(const char *szPath, bool bFile/*=false*/) {
	assert(szPath && szPath[0]);

	if (0 == ::_access(szPath, 0)) {
		return;
	}

	using namespace std;
	static const char del[] = "\\/";
	const char *pSlash = szPath;

	while (pSlash && (pSlash=::strpbrk(pSlash, del))) {
		string strSub(szPath, pSlash);
		if (0 != ::_access(strSub.c_str(), 0)) {
			_mkdir(strSub.c_str());
		}
		pSlash = _tcsspnp(pSlash, del);
	}
	if (!bFile && 0!=::_access(szPath, 0)) {
		_mkdir(szPath);
	}
}

_HYLIB_END
