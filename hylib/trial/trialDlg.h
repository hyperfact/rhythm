// trialDlg.h : header file
//

#if !defined(AFX_TRIALDLG_H__FB39D42C_9235_4508_BF26_48E9DB00B76D__INCLUDED_)
#define AFX_TRIALDLG_H__FB39D42C_9235_4508_BF26_48E9DB00B76D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "LayeredWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CTrialDlg dialog

class CTrialDlg : public CDialog
{
// Construction
public:
	CTrialDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTrialDlg)
	enum { IDD = IDD_TRIAL_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	hy::CLayeredWnd m_wnd;

	// Generated message map functions
	//{{AFX_MSG(CTrialDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMove(int x, int y);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRIALDLG_H__FB39D42C_9235_4508_BF26_48E9DB00B76D__INCLUDED_)
