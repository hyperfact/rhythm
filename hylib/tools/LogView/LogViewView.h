
// LogViewView.h : CLogViewView ��Ľӿ�
//

#pragma once
#include "../../include/buffer.h"

class CLogViewCntrItem;

class CLogViewView : public CRichEditView, public IRichEditOp
{
protected: // �������л�����
	CLogViewView();
	DECLARE_DYNCREATE(CLogViewView)

// ����
public:
	CLogViewDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // ������һ�ε���
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CLogViewView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // LogViewView.cpp �еĵ��԰汾
inline CLogViewDoc* CLogViewView::GetDocument() const
   { return reinterpret_cast<CLogViewDoc*>(m_pDocument); }
#endif

