// StyleToolkit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "StyleToolkit.h"
#include "StyleToolkitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitApp

BEGIN_MESSAGE_MAP(CStyleToolkitApp, CWinApp)
	//{{AFX_MSG_MAP(CStyleToolkitApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitApp construction

CStyleToolkitApp::CStyleToolkitApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStyleToolkitApp object

CStyleToolkitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitApp initialization

BOOL CStyleToolkitApp::InitInstance()
{
	// Standard initialization
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CStyleToolkitDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CStyleToolkitApp::ExitInstance() 
{
	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return CWinApp::ExitInstance();
}
