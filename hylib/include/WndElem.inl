/********************************************************************
	created:	2011/06/07
	created:	7:6:2011   13:40
	filename: 	WndElem.inl
	file base:	WndElem
	file ext:	inl
	author:		huaiyu
	
	purpose:	界面元素类的内联定义文件
*********************************************************************/
#ifndef _WND_ELEM_INL_
#define _WND_ELEM_INL_

#include "compare.h"
#include <algorithm>

_HYLIB_BEGIN

#ifdef _USING_MFC
////////////////////////////////////////////////////////////////////////////////
// class CWndElem
inline CWndElem::CWndElem()
	: m_nID(-1),
	  m_dwAttrib((DWORD)0UL),
	  m_dwState((DWORD)ES_VISIBLE),
	  m_rc(0, 0, 0, 0),
	  m_rcSrc(0, 0, 0, 0),
      m_nZOrder(0),
	  m_img(NULL)
{
}

inline CWndElem::~CWndElem()
{
	
}

inline void CWndElem::Move( int x, int y, int width, int height )
{
	m_rc.SetRect(x, y, x+width, y+height);
}

inline void CWndElem::Move( const CRect &rc )
{
	m_rc = rc;
}

inline void CWndElem::MoveTo( int x, int y )
{
	m_rc.SetRect(x, y, x+m_rc.Width(), y+m_rc.Height());
}

inline void CWndElem::MoveTo( CPoint pt )
{
	m_rc.SetRect(pt.x, pt.y, pt.x+m_rc.Width(), pt.y+m_rc.Height());	
}

inline void CWndElem::Offset( int dx, int dy )
{
	m_rc.OffsetRect(dx, dy);
}

inline void CWndElem::Offset( CSize szOffset )
{
	m_rc.OffsetRect(szOffset);
}

inline int CWndElem::GetWidth() const
{
	return m_rc.Width();
}

inline int CWndElem::GetHeight() const
{
	return m_rc.Height();	
}

inline void CWndElem::GetRect( CRect &rc/*out*/ ) const
{
	rc = m_rc;
}

inline const CRect & CWndElem::GetRect() const
{
	return m_rc;
}

inline int CWndElem::GetZOrder() const
{
	return m_nZOrder;
}


inline void CWndElem::SetAttrib( DWORD dwAttribFlag, BOOL bSet/*=TRUE*/ )
{
	bSet ? BT_SET(m_dwAttrib, dwAttribFlag) : BT_CLEAR(m_dwAttrib, dwAttribFlag);
}

inline void CWndElem::ClearAttrib( DWORD dwAttribFlag )
{
	BT_CLEAR(m_dwAttrib, dwAttribFlag);
}

inline DWORD CWndElem::GetAttrib() const
{
	return m_dwAttrib;
}

inline BOOL CWndElem::TestAttrib( DWORD dwAttribFlag ) const
{
	return BT_IS_SET(m_dwAttrib, dwAttribFlag);
}

inline void CWndElem::SetState( DWORD dwStateFlag, BOOL bSet/*=TRUE*/ )
{
	bSet ? BT_SET(m_dwState, dwStateFlag) : BT_CLEAR(m_dwState, dwStateFlag);
}

inline void CWndElem::ClearState( DWORD dwStateFlag )
{
	BT_CLEAR(m_dwState, dwStateFlag);
}

inline DWORD CWndElem::GetState() const
{
	return m_dwState;
}

inline BOOL CWndElem::TestState( DWORD dwStateFlag ) const
{
	return BT_IS_SET(m_dwState, dwStateFlag);
}

inline void CWndElem::SetVisible( BOOL bVisible )
{
	SetState(ES_VISIBLE, bVisible);
}

inline BOOL CWndElem::IsVisible() const
{
	return TestState(ES_VISIBLE);
}

inline BOOL CWndElem::IsOnCursorFocus() const
{
	return TestState(ES_CURSOR_ON);
}

inline void CWndElem::OnUpdate()
{
	// No implementation
}

inline void CWndElem::Enable( BOOL bEnable )
{
	SetState(EA_ENABLE, bEnable);
}

inline BOOL CWndElem::IsEnable() const
{
	return TestState(EA_ENABLE);
}

inline void CWndElem::EnableDrag( BOOL bEnable )
{
	SetState(EA_DRAGGABLE, bEnable);
}

inline BOOL CWndElem::IsDraggable() const
{
	return TestState(EA_DRAGGABLE);
}

inline BOOL CWndElem::IsDragging() const
{
	return TestState(ES_DRAGGING);
}

inline BOOL CWndElem::IsSelected() const
{
	return TestState(ES_SELECT);
}

inline BOOL CWndElem::HitTest( CPoint pt ) const
{
	return m_rc.PtInRect(pt);
}

inline BOOL CWndElem::FrameSelTest( const CRect &rc ) const
{
	return !(m_rc & rc).IsRectEmpty();
}

////////////////////////////////////////////////////////////////////////////////
// class CWndElemMgr:
inline CWndElemMgr::CWndElemMgr()
	 : m_ptOrg(0,0),
	   m_hwnd(NULL),
	   m_pCurFocusElem(NULL),
	   m_bDragging(FALSE),
	   m_ptDragStart(0,0),
	   m_bFrameSelecting(FALSE),
	   m_pfnOldWndProc(NULL)
{
}

inline CWndElemMgr::~CWndElemMgr()
{
}

inline size_t CWndElemMgr::GetElemCount() const
{
	return (int)m_lstElems.size();	
}

inline CWndElem * CWndElemMgr::GetCurFocusElem()
{
	return m_pCurFocusElem;
}

inline const CWndElem * CWndElemMgr::GetCurFocusElem() const
{
	return m_pCurFocusElem;
}

inline void CWndElemMgr::BringElemToTop(  CWndElem *pElem )
{
	ASSERT(std::find(m_lstElems.begin(), m_lstElems.end(), pElem) != m_lstElems.end());
	m_lstElems.remove(pElem);
	m_lstElems.push_back(pElem);
}

inline void CWndElemMgr::BringElemToBottom( CWndElem *pElem )
{
	ASSERT(std::find(m_lstElems.begin(), m_lstElems.end(), pElem) != m_lstElems.end());
	m_lstElems.remove(pElem);
	m_lstElems.push_front(pElem);
}

inline void CWndElemMgr::BringElemBefore( CWndElem *pElem, CWndElem *pElemRef )
{
	ASSERT(std::find(m_lstElems.begin(), m_lstElems.end(), pElem) != m_lstElems.end());
	m_lstElems.remove(pElem);
	m_lstElems.insert(std::find(m_lstElems.begin(), m_lstElems.end(), pElemRef), pElem);
}

inline void CWndElemMgr::DelElem( CWndElem *pElem )
{
	ASSERT(pElem != NULL);
	m_lstElems.remove(pElem);
}

inline void CWndElemMgr::DeferDelElem( CWndElem *pElem )
{
	pElem->SetState(CWndElem::ES_DELETING);	
}

inline void CWndElemMgr::SetOrgPoint( POINT pt )
{
	m_ptOrg = pt;
}

inline void CWndElemMgr::OnLButtonClick( UINT nFlags, CPoint point )
{
	if (m_pCurFocusElem != NULL) { m_pCurFocusElem->OnLButtonClick(nFlags, point); }
}

inline void CWndElemMgr::OnLButtonDblClick( UINT nFlags, CPoint point )
{
	if (m_pCurFocusElem != NULL) { m_pCurFocusElem->OnLButtonDblClick(nFlags, point); }
}

inline void CWndElemMgr::OnRButtonDown( UINT nFlags, CPoint point )
{
	if (m_pCurFocusElem != NULL) { m_pCurFocusElem->OnRButtonDown(nFlags, point); }
}

inline void CWndElemMgr::OnRButtonClick( UINT nFlags, CPoint point )
{
	if (m_pCurFocusElem != NULL) { m_pCurFocusElem->OnRButtonClick(nFlags, point); }
}

inline void CWndElemMgr::OnRButtonUp( UINT nFlags, CPoint point )
{
	if (m_pCurFocusElem != NULL) { m_pCurFocusElem->OnRButtonUp(nFlags, point); }
}

inline void CWndElemMgr::OnRButtonDblClick( UINT nFlags, CPoint point )
{
	if (m_pCurFocusElem != NULL) { m_pCurFocusElem->OnRButtonDblClick(nFlags, point); }
}

inline BOOL CWndElemMgr::IsInElemList( const CWndElem *pElem ) const
{
	return m_lstElems.end() != std::find(m_lstElems.begin(), m_lstElems.end(), pElem);
}

inline BOOL CWndElemMgr::IsInDraggingList( const CWndElem *pElem ) const
{
	return m_lstDragging.end() != std::find(m_lstDragging.begin(), m_lstDragging.end(), pElem);
}

inline BOOL CWndElemMgr::IsInFrameSelList( const CWndElem *pElem ) const
{
	return m_lstFrameSel.end() != std::find(m_lstFrameSel.begin(), m_lstFrameSel.end(), pElem);
}

inline const CRect & CWndElemMgr::GetSelFrame() const
{
	return m_rcFrameSel;
}

inline void CWndElemMgr::GetSelFrame( CRect &rcFrame ) const
{
	rcFrame	= m_rcFrameSel;
}

inline const CWndElemMgr::WndElemLst & CWndElemMgr::GetFrameSelList() const
{
	return m_lstFrameSel;
}

inline const CWndElemMgr::WndElemLst & CWndElemMgr::GetDraggingList() const
{
	return m_lstDragging;
}

inline void CWndElemMgr::AddToDragList( CWndElem *pElem )
{
	ASSERT(IsInElemList(pElem));
	ASSERT(!IsInDraggingList(pElem));
	m_lstDragging.push_back(pElem);
}

inline void CWndElemMgr::DelFromDragList( CWndElem *pElem )
{
	ASSERT(IsInElemList(pElem));
	ASSERT(IsInDraggingList(pElem));
	m_lstDragging.remove(pElem);
}

inline int CWndElemMgr::GetFrameSelElemCount() const
{
	return (int)m_lstFrameSel.size();
}

inline BOOL CWndElemMgr::FirstFrameSelElem( CWndElem *&pElem )
{
	m_iterFrameSel = m_lstFrameSel.begin();
	pElem = (m_iterFrameSel != m_lstFrameSel.end())
		? *m_iterFrameSel : NULL;
	return !m_lstFrameSel.empty();
}

inline BOOL CWndElemMgr::NextFrameSelElem( CWndElem *&pElem )
{
	BOOL bEnd = m_lstFrameSel.end() == m_iterFrameSel;
	pElem = bEnd ? NULL : *m_iterFrameSel++ ;
	return !bEnd;
}

inline int CWndElemMgr::GetDraggingElemCount() const
{
	return (int)m_lstDragging.size();
}

inline BOOL CWndElemMgr::FirstDraggingElem( CWndElem *&pElem )
{
	m_iterFrameSel = m_lstDragging.begin();
	pElem = (m_iterDragging != m_lstDragging.end())
		? *m_iterDragging : NULL;
	return !m_lstDragging.empty();
}

inline BOOL CWndElemMgr::NextDraggingElem( CWndElem *&pElem )
{
	BOOL bEnd = m_lstDragging.end() == m_iterDragging;
	pElem = bEnd ? NULL : *m_iterDragging++ ;
	return !bEnd;
}
#endif // _USING_MFC

_HYLIB_END

#endif // _WND_ELEM_INL_
