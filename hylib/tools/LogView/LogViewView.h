
// LogViewView.h : CLogViewView 类的接口
//

#pragma once
#include "../../include/buffer.h"

class CLogViewCntrItem;

class CLogViewView : public CRichEditView, public IRichEditOp
{
protected: // 仅从序列化创建
	CLogViewView();
	DECLARE_DYNCREATE(CLogViewView)

// 特性
public:
	CLogViewDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // 构造后第一次调用
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

// 实现
public:
	virtual ~CLogViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnDestroy();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnMsgAppendText(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	virtual void AppendText( LPCTSTR sText );

	CFont			m_font;
	CCriticalSection m_lock;
	hy::cyc_buf<>	m_buf;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // LogViewView.cpp 中的调试版本
inline CLogViewDoc* CLogViewView::GetDocument() const
   { return reinterpret_cast<CLogViewDoc*>(m_pDocument); }
#endif

