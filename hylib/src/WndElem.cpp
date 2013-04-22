// WndElem.cpp: implementation of the CWndElem class.
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WndElem.h"
#include <algorithm>

_HYLIB_BEGIN

#ifdef _USING_MFC
////////////////////////////////////////////////////////////////////////////////
namespace {
	inline bool ZOrderCmp(CWndElem *pElem1, CWndElem *pElem2)
	{
		return pElem1->GetZOrder() < pElem2->GetZOrder();
	}
}

////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
void CWndElem::OnPaint( CDC *pDC )
{
	if (m_img!=NULL && IsVisible())
	{
// 		hy::DrawImage(pDC->GetSafeHdc(), m_img, m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(),
// 			m_rcSrc.left, m_rcSrc.top/*, m_rcSrc.Width(), m_rcSrc.Height()*/);
	}
}

void CWndElem::Init( HBITMAP img, LPCRECT rc, LPCRECT rcSrc/*=NULL*/ )
{
	ASSERT(m_img != NULL);
	ASSERT(rc != NULL);
	SetState(ES_DELETING, FALSE);
	m_rc = rc;
	BITMAP bmp;
	GetObject(img, sizeof(bmp), &bmp);
	
	m_rcSrc = rcSrc ? rcSrc : CRect(CPoint(0,0), CSize(bmp.bmWidth, bmp.bmHeight));
}

////////////////////////////////////////////////////////////////////////////////
void CWndElemMgr::Init( HWND hwnd )
{
	ASSERT(hwnd!=NULL && ::IsWindow(hwnd));
	m_hwnd = hwnd;
	
	ASSERT(NULL == ::GetProp(m_hwnd, WND_ELEM_MGR_PTR));
	::SetProp(m_hwnd, WND_ELEM_MGR_PTR, (HANDLE)this);
	m_pfnOldWndProc = (WNDPROC)::SetWindowLong(m_hwnd, GWL_WNDPROC, LONG(&CWndElemMgr::_WndProc));

	// TODO:增加还原原窗口过程和删除Prop的处理
}

CWndElemMgr &CWndElemMgr::AddElem( CWndElem *pElem )
{
	ASSERT(pElem != NULL);
	ASSERT(std::find(m_lstElems.begin(), m_lstElems.end(), pElem)
		== m_lstElems.end());
	m_lstElems.push_back(pElem);
// 	std::find_if(m_lstElems.begin(), m_lstElems.end(),
// 		std::bind1st(std::ptr_fun(ZOrderCmp), pElem));
// 	for (WndElemIter iter = m_lstElems.begin();
// 		iter!=m_lstElems.end() && pElem->GetZOrder()<(*iter)->GetZOrder();
// 		++iter)
// 	{
// 	}

	return *this;
}

void CWndElemMgr::EndDeferDelElems()
{
	for (WndElemIter iter = m_lstElems.begin();
		iter != m_lstElems.end();
		/* NO STATEMENT */)
	{
		if ((*iter)->TestState(CWndElem::ES_DELETING))
		{
			(*iter)->SetState(CWndElem::ES_DELETING, FALSE);
			iter = m_lstElems.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CWndElemMgr::DrawElems( CDC *pDC )
{
	CPoint ptOld = pDC->SetViewportOrg(m_ptOrg);
	for (WndElemIter iter = m_lstElems.begin();
		iter != m_lstElems.end();
		++iter)
	{
// 		WndElemIter iterNext = iter;
// 		++iterNext;
// 		ASSERT(m_lstElems.end()==iterNext
// 			|| (*iter)->GetZOrder()<=(*iterNext)->GetZOrder());
		ASSERT(*iter != NULL);
		if ((*iter)->IsVisible())
		{
			(*iter)->OnPaint(pDC);
		}
	}
	pDC->SetViewportOrg(ptOld);
}

void CWndElemMgr::UpdateElems()
{
	for (WndElemIter iter = m_lstElems.begin();
	iter != m_lstElems.end();
	++iter)
	{
		// 		WndElemIter iterNext = iter;
		// 		++iterNext;
		// 		ASSERT(m_lstElems.end()==iterNext
		// 			|| (*iter)->GetZOrder()<=(*iterNext)->GetZOrder());
		
		(*iter)->OnUpdate();
	}
}

void CWndElemMgr::BringElemAfter( CWndElem *pElem, CWndElem *pElemRef )
{
	ASSERT(std::find(m_lstElems.begin(), m_lstElems.end(), pElem) != m_lstElems.end());
	m_lstElems.remove(pElem);
	WndElemIter iterDst = std::find(m_lstElems.begin(), m_lstElems.end(), pElemRef);
	if (iterDst != m_lstElems.end()) { ++iterDst; }
	m_lstElems.insert(iterDst, pElem);
}

CWndElem *CWndElemMgr::ElemHitTest( CPoint point )
{
	point.Offset(-m_ptOrg);
	for (WndElemLst::reverse_iterator iter = m_lstElems.rbegin();
		iter != m_lstElems.rend();
		++iter)
	{
		ASSERT((*iter) != NULL);
		if ((*iter)->IsEnable() && (*iter)->HitTest(point))
		{
			return (*iter);
		}
	}
	return NULL;
}

CWndElem * CWndElemMgr::SetCursorFocus( CWndElem *pElem )
{
	if (m_pCurFocusElem != NULL)
	{	// 旧焦点对象
		m_pCurFocusElem->ClearState(CWndElem::ES_CURSOR_ON);
		m_pCurFocusElem->OnMouseLeave();
	}
	if (pElem != NULL)
	{	// 新焦点对象
		pElem->SetState(CWndElem::ES_CURSOR_ON);
		pElem->OnMouseEnter();
	}
	
	CWndElem *old = m_pCurFocusElem;
	m_pCurFocusElem = pElem;
	return old;
}

void CWndElemMgr::OnMouseMove( UINT nFlags, CPoint point )
{
	// 拖拽中
	if (m_bDragging)
	{
		OnDragging(nFlags, point);
		return;
	}
	if(m_bFrameSelecting)
	{
		OnFrameSelecting(nFlags, point);
		return;
	}

// 	if (::DragDetect(m_hwnd, m_ptDragStart))
// 	{	// 开始拖拽
// 		SetCursorFocus(NULL);
// 		OnDragBegin(nFlags, m_ptDragStart);
// 		return;
// 	}
	
	// 光标焦点检测
	CWndElem *pElem = ElemHitTest(point);
	if (pElem != m_pCurFocusElem)
	{	// 焦点切换
		SetCursorFocus(pElem);
	}
	else
	{	// 光标悬停
		if (m_pCurFocusElem != NULL) {
			m_pCurFocusElem->OnMouseHover(nFlags, point);
		}
	}
}

void CWndElemMgr::OnLButtonDown( UINT nFlags, CPoint point )
{
//	if (!m_bDragging) { m_ptDragStart = point; }

	if (m_pCurFocusElem != NULL) {
		m_pCurFocusElem->OnLButtonDown(nFlags, point);
	}
}

void CWndElemMgr::OnLButtonUp( UINT nFlags, CPoint point )
{
// 	if (m_bDragging) {
// 		m_bDragging = FALSE;
// 		OnDragEnd(nFlags, point);
// 	}

	if (m_pCurFocusElem != NULL) {
		m_pCurFocusElem->OnLButtonUp(nFlags, point);
	}
}

void CWndElemMgr::OnDragBegin( UINT nFlags, CPoint point )
{
	m_bDragging = TRUE;
	SetCursorFocus(NULL);
	m_ptLast = m_ptDragStart = point;
	for (WndElemIter iter = m_lstDragging.begin();
		iter != m_lstDragging.end();
		++iter)
	{
		ASSERT(*iter);
		(*iter)->OnDragBegin(nFlags, point);
		(*iter)->SetState(CWndElem::ES_DRAGGING);
	}
}

void CWndElemMgr::OnDragging( UINT nFlags, CPoint point )
{
	CSize szOffset = point - m_ptLast;
	for (WndElemIter iter = m_lstDragging.begin();
		iter != m_lstDragging.end();
		++iter)
	{
		ASSERT(*iter);
		(*iter)->Offset(szOffset);
		(*iter)->OnDragOn(nFlags, point);
	}
	m_ptLast = point;
}

void CWndElemMgr::OnDragCancel( UINT nFlags, CPoint point )
{
	CSize szOffset = m_ptDragStart-m_ptLast;
	for (WndElemIter iter = m_lstDragging.begin();
	iter != m_lstDragging.end();
	++iter)
	{
		ASSERT(*iter);
		(*iter)->Offset(szOffset);
		(*iter)->OnDragEnd(nFlags, point);
		(*iter)->ClearState(CWndElem::ES_DRAGGING);
	}

	m_bDragging = FALSE;
	m_lstDragging.clear();
}

void CWndElemMgr::OnDragEnd( UINT nFlags, CPoint point )
{
	for (WndElemIter iter = m_lstDragging.begin();
		iter != m_lstDragging.end();
		++iter)
	{
		ASSERT(*iter);
		(*iter)->OnDragEnd(nFlags, point);
		(*iter)->ClearState(CWndElem::ES_DRAGGING);
	}
	m_bDragging = FALSE;
	m_lstDragging.clear();
}

void CWndElemMgr::OnFrameSelBegin( UINT nFlags, CPoint point )
{
	m_bFrameSelecting = TRUE;
	SetCursorFocus(NULL);
	m_lstFrameSel.clear();
	m_ptDragStart = point;
	m_rcFrameSel.SetRectEmpty();
}

void CWndElemMgr::OnFrameSelecting( UINT nFlags, CPoint point )
{
	m_lstFrameSel.clear();

	m_rcFrameSel.SetRect(
		__min(m_ptDragStart.x, point.x),
		__min(m_ptDragStart.y, point.y),
		__max(m_ptDragStart.x, point.x),
		__max(m_ptDragStart.y, point.y));
	for (WndElemLst::reverse_iterator iter = m_lstElems.rbegin();
		iter != m_lstElems.rend();
		++iter)
	{
		ASSERT(*iter);
		if (!(*iter)->IsEnable()) { continue; }
		if ((*iter)->FrameSelTest(m_rcFrameSel)) {
			if (!(*iter)->IsSelected()) { (*iter)->OnSelected(TRUE); }
			(*iter)->SetState(CWndElem::ES_SELECT);
			m_lstFrameSel.push_front(*iter);
		} else {
			if ((*iter)->IsSelected()) { (*iter)->OnSelected(FALSE); }
			(*iter)->ClearState(CWndElem::ES_SELECT);
		}
	}
}

void CWndElemMgr::OnFrameSelEnd( UINT nFlags, CPoint point )
{
	m_bFrameSelecting = FALSE;
}

LPCTSTR CWndElemMgr::WND_ELEM_MGR_PTR = _T("WndElemMgr");

LRESULT CWndElemMgr::_WndProc( HWND hwnd, UINT nMsg, WPARAM wp, LPARAM lp )
{
	CWndElemMgr *pThis = (CWndElemMgr *)::GetProp(hwnd, WND_ELEM_MGR_PTR);
	ASSERT(pThis != NULL);

	switch (nMsg)
	{
	case WM_MOUSEMOVE:
		{
			pThis->OnMouseMove((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_LBUTTONDOWN:
		{
			pThis->OnLButtonDown((UINT)wp, CPoint((DWORD)lp));
			pThis->OnLButtonClick((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_LBUTTONUP:
		{
			pThis->OnLButtonUp((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			pThis->OnLButtonDblClick((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_RBUTTONDOWN:
		{
			pThis->OnRButtonDown((UINT)wp, CPoint((DWORD)lp));
			pThis->OnRButtonClick((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_RBUTTONUP:
		{
			pThis->OnRButtonUp((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_RBUTTONDBLCLK:
		{
			pThis->OnRButtonDblClick((UINT)wp, CPoint((DWORD)lp));
		}
		break;
	case WM_DESTROY:
		{
			::RemoveProp(hwnd, WND_ELEM_MGR_PTR);
		}
		break;
	default:
		break;
	}

	if (pThis->m_pfnOldWndProc) { return ::CallWindowProc(pThis->m_pfnOldWndProc, hwnd, nMsg, wp, lp); }
	else { return ::DefWindowProc(hwnd, nMsg, wp, lp); }
}
#endif // _USING_MFC

_HYLIB_END
////////////////////////////////////////////////////////////////////////////////
