/********************************************************************
	created:	2011/06/24
	created:	24:6:2011   17:46
	file base:	WndElem
	file ext:	h
	author:		huaiyu
	
	purpose:	界面元素(图层)基类, 图层管理器
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
// 界面元素基类
class HYLIB_DECL CWndElem
{
//******************************************************************************
// Types:
public:
	enum ElemAttrib {
		EA_NONE			= 0,
		EA_ENABLE		= 1,		// 可控性
		EA_MOUSABLE		= 1<<1,		// 鼠标可控性
		EA_KEYABLE		= 1<<2,		// 键盘可控性
		EA_DRAGGABLE	= 1<<3,		// 可拖拽性

		EA_CUSTOM		= 1<<4,		// 自定义属性
	};
	enum ElemState {
		ES_NONE			= 0,
		ES_VISIBLE		= 1,		// 可视状态
		ES_CURSOR_ON	= 1<<1,		// 光标焦点状态
		ES_FOCUS		= 1<<2,		// 输入焦点状态
		ES_ACTIVITY		= 1<<3,		// 激活状态
		ES_SELECT		= 1<<4,		// 选中状态
		ES_DRAGGING		= 1<<5,		// 拖动状态
		ES_DELETING		= 1<<6,		// 延迟删除状态

		ES_CUSTOM		= 1<<7,		// 自定义状态
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
	// 初始化
	void Init(HBITMAP img, LPCRECT rc, LPCRECT rcSrc=NULL);

//******************************************************************************
// Access methods:
public:
	// 移动
	void Move(int x, int y, int width, int height);
	void Move(const CRect &rc);
	void MoveTo(int x, int y);
	void MoveTo(CPoint pt);
	void Offset(int dx, int dy);
	void Offset(CSize szOffset);

	// 获取大小
	int GetWidth() const;
	int GetHeight() const;
	void GetRect(CRect &rc/*out*/) const;
	const CRect &GetRect() const;

	// 获取深度(层次)
	int GetZOrder() const;

	// 设置可见性
	void SetVisible(BOOL bVisible);
	// 判断是否可见
	BOOL IsVisible() const;

	// 是否获得光标焦点
	BOOL IsOnCursorFocus() const;

	// 允许鼠标操作
	void Enable(BOOL bEnable);
	BOOL IsEnable() const;
	
	// 允许拖拽
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
	// 更新
	virtual void OnUpdate();
	// 绘制
	virtual void OnPaint(CDC *pDC);

	// 鼠标盘旋
	virtual void OnMouseHover(UINT nFlags, CPoint point) {}
	// 鼠标离开
	virtual void OnMouseLeave() {}
	// 鼠标进入
	virtual void OnMouseEnter() {}
	
	// 拖拽中
	virtual void OnDragOn(UINT nFlags, CPoint point) {}
	// 开始拖拽
	virtual void OnDragBegin(UINT nFlags, CPoint point) {}
	// 结束拖拽
	virtual void OnDragEnd(UINT nFlags, CPoint point) {}
	
	// 左键按下
	virtual void OnLButtonDown(UINT nFlags, CPoint point) {}
	// 左键单击
	virtual void OnLButtonClick(UINT nFlags, CPoint point) {}
	// 左键释放
	virtual void OnLButtonUp(UINT nFlags, CPoint point) {}
	// 左键双击
	virtual void OnLButtonDblClick(UINT nFlags, CPoint point) {}
	
	// 右键按下
	virtual void OnRButtonDown(UINT nFlags, CPoint point) {}
	// 右键单击
	virtual void OnRButtonClick(UINT nFlags, CPoint point) {}
	// 右键释放
	virtual void OnRButtonUp(UINT nFlags, CPoint point) {}
	// 右键双击
	virtual void OnRButtonDblClick(UINT nFlags, CPoint point) {}

	// 选中
	virtual void OnSelected(BOOL bSelect) {}

	// 光标检测
	virtual BOOL HitTest(CPoint pt) const;
	// 框选检测
	virtual BOOL FrameSelTest(const CRect &rc) const;

//******************************************************************************
// Fields:
protected:
	DWORD		m_dwAttrib;		// 属性字
	DWORD		m_dwState;		// 状态字
	int			m_nID;			// ID
	CRect		m_rc;			// 显示区域
	CRect		m_rcSrc;		// 图片源区域
	HBITMAP		m_img;			// 图片
	int			m_nZOrder;		// 层次

//******************************************************************************
// Friends:
	friend class CWndElemMgr;
};

////////////////////////////////////////////////////////////////////////////////
// 界面元素管理器
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

	// 添加元素
	CWndElemMgr &AddElem(CWndElem *pElem);
	// 删除元素
	void DelElem(CWndElem *pElem);
	// 延迟删除
	void DeferDelElem(CWndElem *pElem);
	// 删除延迟删除的图层
	void EndDeferDelElems();

	// 更新元素
	void UpdateElems();
	// 绘制元素
	void DrawElems(CDC *pDC);

	// 获取元素个数
	size_t GetElemCount() const;

	// 获取当前获得鼠标焦点的元素
	CWndElem *GetCurFocusElem();
	const CWndElem *GetCurFocusElem() const;
	
	// 鼠标事件处理程序
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonClick(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClick(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnRButtonClick(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDblClick(UINT nFlags, CPoint point);

	// 拖拽
	void OnDragBegin(UINT nFlags, CPoint point);
	void OnDragging(UINT nFlags, CPoint point);
	void OnDragEnd(UINT nFlags, CPoint point);
	void OnDragCancel(UINT nFlags, CPoint point);
	
	// 框选
	void OnFrameSelBegin(UINT nFlags, CPoint point);
	void OnFrameSelecting(UINT nFlags, CPoint point);
	void OnFrameSelEnd(UINT nFlags, CPoint point);

	// 元素深度(层次)调整函数
	void BringElemToTop(CWndElem *pElem);
	void BringElemToBottom(CWndElem *pElem);
	void BringElemBefore(CWndElem *pElem, CWndElem *pElemRef);
	void BringElemAfter(CWndElem *pElem, CWndElem *pElemRef);

	// 设置绘制原点
	void SetOrgPoint(POINT pt);

	// 光标检测
	CWndElem *ElemHitTest(CPoint point);

	// 获取选框
	const CRect &GetSelFrame() const;
	void GetSelFrame(CRect &rcFrame) const;

	const WndElemLst &GetFrameSelList() const;
	const WndElemLst &GetDraggingList() const;

	// 枚举框选元素
	int	 GetFrameSelElemCount() const;
	BOOL FirstFrameSelElem(CWndElem *&pElem);
	BOOL NextFrameSelElem(CWndElem *&pElem);

	// 枚举拖动元素
	int GetDraggingElemCount() const;
	BOOL FirstDraggingElem(CWndElem *&pElem);
	BOOL NextDraggingElem(CWndElem *&pElem);

	// 添加/删除到拖拽列表
	void AddToDragList(CWndElem *pElem);
	void DelFromDragList(CWndElem *pElem);

//******************************************************************************
// Tools:
protected:
	// 设置光标焦点
	CWndElem *SetCursorFocus(CWndElem *pElem);

	BOOL	IsInElemList(const CWndElem *pElem) const;
	BOOL	IsInDraggingList(const CWndElem *pElem) const;
	BOOL	IsInFrameSelList(const CWndElem *pElem) const;

//******************************************************************************
// Fields:
protected:
	HWND		m_hwnd;			// 窗口句柄
	WndElemLst	m_lstElems;		// Ordered by Z-Order
	CPoint		m_ptOrg;		// 绘制偏移
	CWndElem	*m_pCurFocusElem; // 当前获得光标的元素
	
	BOOL		m_bDragging;	// 拖拽状态
	CPoint		m_ptDragStart;	// 拖拽起点
	CPoint		m_ptLast;	// 拖拽时上一个WM_MOUSEMOVE消息的点
	WndElemLst	m_lstDragging;	// 拖拽列表
	WndElemIter	m_iterDragging;	// 拖拽列表迭代子

	BOOL		m_bFrameSelecting; // 框选
	CRect		m_rcFrameSel;	// 选框
	WndElemLst	m_lstFrameSel;	// 框选列表
	WndElemIter	m_iterFrameSel;	// 框选列表迭代子

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
