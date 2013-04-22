/********************************************************************
	created:	2011/06/24
	created:	24:6:2011   17:46
	file base:	WndElem
	file ext:	h
	author:		huaiyu
	
	purpose:	����Ԫ��(ͼ��)����, ͼ�������
*********************************************************************/

#if !defined(AFX_WNDELEM_H__B5EDC267_FF50_4E71_A7A9_339F92D35A7C__INCLUDED_)
#define AFX_WNDELEM_H__B5EDC267_FF50_4E71_A7A9_339F92D35A7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////////////
#include "_prolog.h"
#include <list>

_HYLIB_BEGIN

#ifdef _USING_MFC
////////////////////////////////////////////////////////////////////////////////
// Prototypes:
class CWndElem;
class CWndElemMgr;

////////////////////////////////////////////////////////////////////////////////
// ����Ԫ�ػ���
class HYLIB_DECL CWndElem
{
//******************************************************************************
// Types:
public:
	enum ElemAttrib {
		EA_NONE			= 0,
		EA_ENABLE		= 1,		// �ɿ���
		EA_MOUSABLE		= 1<<1,		// ���ɿ���
		EA_KEYABLE		= 1<<2,		// ���̿ɿ���
		EA_DRAGGABLE	= 1<<3,		// ����ק��

		EA_CUSTOM		= 1<<4,		// �Զ�������
	};
	enum ElemState {
		ES_NONE			= 0,
		ES_VISIBLE		= 1,		// ����״̬
		ES_CURSOR_ON	= 1<<1,		// ��꽹��״̬
		ES_FOCUS		= 1<<2,		// ���뽹��״̬
		ES_ACTIVITY		= 1<<3,		// ����״̬
		ES_SELECT		= 1<<4,		// ѡ��״̬
		ES_DRAGGING		= 1<<5,		// �϶�״̬
		ES_DELETING		= 1<<6,		// �ӳ�ɾ��״̬

		ES_CUSTOM		= 1<<7,		// �Զ���״̬
	};

	typedef void (CWndElem::*MouseEventFunc)(UINT nFlags, CPoint point);

//******************************************************************************
// Con/Destructor(s):
public:
	CWndElem();
	virtual ~CWndElem();

//******************************************************************************
// Initialization Methods:
public:
	// ��ʼ��
	void Init(HBITMAP img, LPCRECT rc, LPCRECT rcSrc=NULL);

//******************************************************************************
// Access methods:
public:
	// �ƶ�
	void Move(int x, int y, int width, int height);
	void Move(const CRect &rc);
	void MoveTo(int x, int y);
	void MoveTo(CPoint pt);
	void Offset(int dx, int dy);
	void Offset(CSize szOffset);

	// ��ȡ��С
	int GetWidth() const;
	int GetHeight() const;
	void GetRect(CRect &rc/*out*/) const;
	const CRect &GetRect() const;

	// ��ȡ���(���)
	int GetZOrder() const;

	// ���ÿɼ���
	void SetVisible(BOOL bVisible);
	// �ж��Ƿ�ɼ�
	BOOL IsVisible() const;

	// �Ƿ��ù�꽹��
	BOOL IsOnCursorFocus() const;

	// ����������
	void Enable(BOOL bEnable);
	BOOL IsEnable() const;
	
	// ������ק
	void EnableDrag(BOOL bEnable);
	BOOL IsDraggable() const;
	
	BOOL IsDragging() const;

	BOOL IsSelected() const;

//******************************************************************************
// Attribution & State methods:
public:
	// Attribution access:
	void SetAttrib(DWORD dwAttribFlag, BOOL bSet=TRUE);
	void ClearAttrib(DWORD dwAttribFlag);
	DWORD GetAttrib() const;
	BOOL TestAttrib(DWORD dwAttribFlag) const;
	
	// State access:
	void SetState(DWORD dwStateFlag, BOOL bSet=TRUE);
	void ClearState(DWORD dwStateFlag);
	DWORD GetState() const;
	BOOL TestState(DWORD dwStateFlag) const;

//******************************************************************************
// Overridables:
protected:
	// ����
	virtual void OnUpdate();
	// ����
	virtual void OnPaint(CDC *pDC);

	// �������
	virtual void OnMouseHover(UINT nFlags, CPoint point) {}
	// ����뿪
	virtual void OnMouseLeave() {}
	// ������
	virtual void OnMouseEnter() {}
	
	// ��ק��
	virtual void OnDragOn(UINT nFlags, CPoint point) {}
	// ��ʼ��ק
	virtual void OnDragBegin(UINT nFlags, CPoint point) {}
	// ������ק
	virtual void OnDragEnd(UINT nFlags, CPoint point) {}
	
	// �������
	virtual void OnLButtonDown(UINT nFlags, CPoint point) {}
	// �������
	virtual void OnLButtonClick(UINT nFlags, CPoint point) {}
	// ����ͷ�
	virtual void OnLButtonUp(UINT nFlags, CPoint point) {}
	// ���˫��
	virtual void OnLButtonDblClick(UINT nFlags, CPoint point) {}
	
	// �Ҽ�����
	virtual void OnRButtonDown(UINT nFlags, CPoint point) {}
	// �Ҽ�����
	virtual void OnRButtonClick(UINT nFlags, CPoint point) {}
	// �Ҽ��ͷ�
	virtual void OnRButtonUp(UINT nFlags, CPoint point) {}
	// �Ҽ�˫��
	virtual void OnRButtonDblClick(UINT nFlags, CPoint point) {}

	// ѡ��
	virtual void OnSelected(BOOL bSelect) {}

	// �����
	virtual BOOL HitTest(CPoint pt) const;
	// ��ѡ���
	virtual BOOL FrameSelTest(const CRect &rc) const;

//******************************************************************************
// Fields:
protected:
	DWORD		m_dwAttrib;		// ������
	DWORD		m_dwState;		// ״̬��
	int			m_nID;			// ID
	CRect		m_rc;			// ��ʾ����
	CRect		m_rcSrc;		// ͼƬԴ����
	HBITMAP		m_img;			// ͼƬ
	int			m_nZOrder;		// ���

//******************************************************************************
// Friends:
	friend class CWndElemMgr;
};

////////////////////////////////////////////////////////////////////////////////
// ����Ԫ�ع�����
class HYLIB_DECL CWndElemMgr
{
//******************************************************************************
// Types:
public:
	typedef std::list<CWndElem *> WndElemLst;
	typedef WndElemLst::iterator WndElemIter;
	typedef WndElemLst::const_iterator WndElemCIter;

//******************************************************************************
// Con/Destructor(s):
public:
	CWndElemMgr();
	virtual ~CWndElemMgr();

//******************************************************************************
// Methods:
public:
	void Init(HWND hwnd);

	// ���Ԫ��
	CWndElemMgr &AddElem(CWndElem *pElem);
	// ɾ��Ԫ��
	void DelElem(CWndElem *pElem);
	// �ӳ�ɾ��
	void DeferDelElem(CWndElem *pElem);
	// ɾ���ӳ�ɾ����ͼ��
	void EndDeferDelElems();

	// ����Ԫ��
	void UpdateElems();
	// ����Ԫ��
	void DrawElems(CDC *pDC);

	// ��ȡԪ�ظ���
	size_t GetElemCount() const;

	// ��ȡ��ǰ�����꽹���Ԫ��
	CWndElem *GetCurFocusElem();
	const CWndElem *GetCurFocusElem() const;
	
	// ����¼��������
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonClick(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClick(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRButtonClick(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDblClick(UINT nFlags, CPoint point);

	// ��ק
	void OnDragBegin(UINT nFlags, CPoint point);
	void OnDragging(UINT nFlags, CPoint point);
	void OnDragEnd(UINT nFlags, CPoint point);
	void OnDragCancel(UINT nFlags, CPoint point);
	
	// ��ѡ
	void OnFrameSelBegin(UINT nFlags, CPoint point);
	void OnFrameSelecting(UINT nFlags, CPoint point);
	void OnFrameSelEnd(UINT nFlags, CPoint point);

	// Ԫ�����(���)��������
	void BringElemToTop(CWndElem *pElem);
	void BringElemToBottom(CWndElem *pElem);
	void BringElemBefore(CWndElem *pElem, CWndElem *pElemRef);
	void BringElemAfter(CWndElem *pElem, CWndElem *pElemRef);

	// ���û���ԭ��
	void SetOrgPoint(POINT pt);

	// �����
	CWndElem *ElemHitTest(CPoint point);

	// ��ȡѡ��
	const CRect &GetSelFrame() const;
	void GetSelFrame(CRect &rcFrame) const;

	const WndElemLst &GetFrameSelList() const;
	const WndElemLst &GetDraggingList() const;

	// ö�ٿ�ѡԪ��
	int	 GetFrameSelElemCount() const;
	BOOL FirstFrameSelElem(CWndElem *&pElem);
	BOOL NextFrameSelElem(CWndElem *&pElem);

	// ö���϶�Ԫ��
	int GetDraggingElemCount() const;
	BOOL FirstDraggingElem(CWndElem *&pElem);
	BOOL NextDraggingElem(CWndElem *&pElem);

	// ���/ɾ������ק�б�
	void AddToDragList(CWndElem *pElem);
	void DelFromDragList(CWndElem *pElem);

//******************************************************************************
// Tools:
protected:
	// ���ù�꽹��
	CWndElem *SetCursorFocus(CWndElem *pElem);

	BOOL	IsInElemList(const CWndElem *pElem) const;
	BOOL	IsInDraggingList(const CWndElem *pElem) const;
	BOOL	IsInFrameSelList(const CWndElem *pElem) const;

//******************************************************************************
// Fields:
protected:
	HWND		m_hwnd;			// ���ھ��
	WndElemLst	m_lstElems;		// Ordered by Z-Order
	CPoint		m_ptOrg;		// ����ƫ��
	CWndElem	*m_pCurFocusElem; // ��ǰ��ù���Ԫ��
	
	BOOL		m_bDragging;	// ��ק״̬
	CPoint		m_ptDragStart;	// ��ק���
	CPoint		m_ptLast;	// ��קʱ��һ��WM_MOUSEMOVE��Ϣ�ĵ�
	WndElemLst	m_lstDragging;	// ��ק�б�
	WndElemIter	m_iterDragging;	// ��ק�б������

	BOOL		m_bFrameSelecting; // ��ѡ
	CRect		m_rcFrameSel;	// ѡ��
	WndElemLst	m_lstFrameSel;	// ��ѡ�б�
	WndElemIter	m_iterFrameSel;	// ��ѡ�б������

//******************************************************************************
// Intercept input messages:
protected:
	static LPCTSTR WND_ELEM_MGR_PTR;
	static LRESULT _WndProc(HWND hwnd, UINT nMsg, WPARAM wp, LPARAM lp);
	WNDPROC		m_pfnOldWndProc;
};
#endif // _USING_MFC

_HYLIB_END

////////////////////////////////////////////////////////////////////////////////
// inline implements
#include "WndElem.inl"

#endif // !defined(AFX_WNDELEM_H__B5EDC267_FF50_4E71_A7A9_339F92D35A7C__INCLUDED_)
