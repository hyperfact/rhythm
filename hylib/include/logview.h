#if !defined(AFX_LOGVIEW_H__3E7C3A68_A56C_4BD1_A4E6_00B5A1995BF0__INCLUDED_)
#define AFX_LOGVIEW_H__3E7C3A68_A56C_4BD1_A4E6_00B5A1995BF0__INCLUDED_
// logview.h : header file
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "_prolog.h"
#include <afxcmn.h>
#include "util_tmpl.h"
#include "singleton.h"
#include <strstream>

#ifdef _USING_MFC

_HYLIB_BEGIN
/////////////////////////////////////////////////////////////////////////////
// CLogView dialog
class CLogView : public CDialog, public std::ostrstream, public singleton<CLogView>
{
// Singleton:
	friend struct singleton<CLogView>;

//******************************************************************************
// Construction
public:
	CLogView(CWnd* pParent = NULL);   // standard constructor

//******************************************************************************
// Dialog Data
	//{{AFX_DATA(CLogView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
protected:
	enum { IDC_RICHEDIT = 100 };
public:
	enum {
		FLG_NONE,
		FLG_READONLY		= 1,
		FLG_AUTO_SCROLL		= 1<<1,
	};

//******************************************************************************
public:
	CRichEditCtrl *Edit() { return &m_redit; }

	void AppendLog(const char *str);

	void AutoScroll(int nLines=INT_MAX);

	void SetMaxLine(unsigned nMaxLine) { m_nMaxLine = nMaxLine; }

//******************************************************************************
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogView)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

//******************************************************************************
// Implementation
protected:
	CRichEditCtrl	m_redit;
	unsigned		m_nMaxLine;
	unsigned		m_nFlag;

//******************************************************************************
	// Generated message map functions
	//{{AFX_MSG(CLogView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRERBtnClk(UINT nFlags, CPoint point);
	afx_msg void OnMenuCommand(UINT nMenuID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

//******************************************************************************
	friend CLogView &log_end_rec(CLogView &os);
};

#define g_pLogView (_HYLIB CLogView::inst())

//{{AFX_INSERT_LOCATION
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//}}AFX_INSERT_LOCATION

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // _USING_MFC

#endif // !defined(AFX_LOGVIEW_H__3E7C3A68_A56C_4BD1_A4E6_00B5A1995BF0__INCLUDED_)
