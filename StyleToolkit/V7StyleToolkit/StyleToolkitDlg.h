// StyleToolkitDlg.h : header file
//

#pragma once

#include "Style.h"
#include "StyleEdit.h"
#include "StyleProgress.h"
#include "StyleButton.h"
#include "StyleDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitDlg dialog

class CStyleToolkitDlg : public StyleDialog
{

public:

	CStyleToolkitDlg(CWnd* pParent = NULL);	

	void CreateStacks();
	void CreateRiverPane();
	void CreateTealPane();
	void CreateButtonPane();
	void CreateProgressBars();
	void CreateEditBoxes();
	void CreateTestStack();
	void CreateVistaStyle1Buttons();
	void CreateVistaStyle2Buttons();
	void CreateAquaStyleButtons();

	void CreateFrame();


	Style	m_Style;

	enum { IDD = IDD_STYLETOOLKIT_DIALOG };

	StyleProgress	m_cProgress1;
	StyleProgress	m_cProgress2;
	StyleProgress	m_cProgress3;
	StyleProgress	m_cProgress4;

	StyleButton		m_cPStep1;
	StyleButton		m_cPStep2;
	StyleButton		m_cPStep3;
	StyleButton		m_cPStep4;

	StyleButton		m_cVistaStyle1Norm;
	StyleButton		m_cVistaStyle1Checked;
	StyleButton		m_cVistaStyle1NormH;
	StyleButton		m_cVistaStyle1NormP;
	StyleButton		m_cVistaStyle1CheckedT;
	StyleButton		m_cVistaStyle1CheckedD;

	StyleButton		m_cAqua1;
	StyleButton		m_cAqua2;
	StyleButton		m_cAqua3;

	StyleButton		m_cVistaStyle2Norm;
	StyleButton		m_cVistaStyle2Hot;

	StyleEdit		m_cEdit1;
	StyleEdit		m_cEdit2;

	CStatic			m_cDiv1;
	CStatic			m_cDiv2;
	CStatic			m_cVDiv1;
	CStatic			m_cShield;
	CStatic			m_cStringFrame;

	void			SetBackgrounds(CDC* pDC, CRect rect);
	BOOL			m_bBackgroundsSet;
	Stack			VS2Std;
	Stack			VS2Hot;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	HICON m_hIcon;
	//{{AFX_MSG(CStyleToolkitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPstep1();
	afx_msg void OnPstep2();
	afx_msg void OnPstep3();
	afx_msg void OnPstep4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

