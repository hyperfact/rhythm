// StyleToolkit.h : main header file for the STYLETOOLKIT application
//

#if !defined(AFX_STYLETOOLKIT_H__8E12344F_E501_45C8_A702_B3BF01026585__INCLUDED_)
#define AFX_STYLETOOLKIT_H__8E12344F_E501_45C8_A702_B3BF01026585__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitApp:
// See StyleToolkit.cpp for the implementation of this class
//

class CStyleToolkitApp : public CWinApp
{
public:
	CStyleToolkitApp();

	ULONG_PTR m_gdiplusToken;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStyleToolkitApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStyleToolkitApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STYLETOOLKIT_H__8E12344F_E501_45C8_A702_B3BF01026585__INCLUDED_)
