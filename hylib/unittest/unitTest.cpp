// unitTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "unitTest.h"

#include <cppunit/ui/mfc/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUnitTestApp

BEGIN_MESSAGE_MAP(CUnitTestApp, CWinApp)
	//{{AFX_MSG_MAP(CUnitTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnitTestApp construction

CUnitTestApp::CUnitTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CUnitTestApp object

CUnitTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUnitTestApp initialization

BOOL CUnitTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey(_T("hylib-unittest"));
	RunTests();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CUnitTestApp::RunTests()
{
	CPPUNIT_NS::MfcUi::TestRunner runner;
	
	runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry(TEST_SUITE_NAME).makeTest() );
	
	runner.run();
}
