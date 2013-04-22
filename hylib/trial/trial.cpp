// trial.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "trial.h"
#include "trialDlg.h"

#include "util.h"
#include "XMLHandle.h"
#include "ResourceMgr.h"
#include <iomanip>
#include "Singleton.h"
#include "tmpldlg.h"
#include "logview.h"
#include <tchar.h>
#include <process.h>
//hy::_Test _test;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// struct SingletonTest1 : public hy::singleton<SingletonTest1>
// {
// 	SingletonTest1() : hy::singleton<SingletonTest1>() {
// 		TRACE("\nConstructing SingletonTest1...\n");
// 	}
// 	~SingletonTest1() {
// 		TRACE("\nDestructing SingletonTest1...\n");
// 	}
// };
// 
// struct SingletonTest2 : public hy::singleton<SingletonTest2>
// {
// 	SingletonTest2() : hy::singleton<SingletonTest2>() {
// 		TRACE("\nConstructing SingletonTest2...\n");
// 	}
// 	~SingletonTest2() {
// 		TRACE("\nDestructing SingletonTest2...\n");
// 	}
// };
// 
// struct SingletonTest3 : public hy::singleton<SingletonTest3>
// {
// 	SingletonTest3() : hy::singleton<SingletonTest3>(2) {
// 		TRACE("\nConstructing SingletonTest3...\n");
// 	}
// 	~SingletonTest3() {
// 		TRACE("\nDestructing SingletonTest3...\n");
// 	}
// };
// 
// struct SingletonTest4 : public hy::singleton<SingletonTest4>
// {
// 	HY_DECLARE_SINGLETON(SingletonTest4, 1);
// };

#ifndef	LOG_DISABLE
#	define LOG(x)	x
#else
#	define LOG(x)	(void(0))
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrialApp

struct Cls {
	void memFunc1() { HYLOGSInfo << "void Cls::memFun1()"; }
	int memFunc2(int) { HYLOGSInfo << "int Cls::memFun2(int)"; return 0; }
};

struct Func1 {
	void operator() () { HYLOGSInfo << "void Func1()()"; }
};
struct Func2 {
	int operator() (int) { HYLOGSInfo << "int Func2()(int)"; return 0; }
};

void func1() { HYLOGSInfo << "void ::func1()"; }
int func2(int) { HYLOGSInfo << "int ::func2(int)"; return 0; }

HANDLE _logTestThrdHdls[1];
unsigned __stdcall LogTestThrd(void *param) {
	while (1) {
		::Sleep(1000);

		DWORD dwTID = ::GetCurrentThreadId();
		LOG(HYLOGSGuardInfo());
		LOG(HYLOGSBlkL << _T("Thread:") << dwTID);
	}
	return 0;
}

// struct SNoHeapAlloc {
// 	void *operator new (size_t);
// 	void *operator new[] (size_t);
// } * a = new SNoHeapAlloc[1];

/////////////////////////////////////////////////////////////////////////////
// CTrialApp construction

CTrialApp::CTrialApp()
{
	_HYLIB_USING;
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance	

// 	//_CrtSetBreakAlloc(231);
// 
// 	Cls obj;
// 	Func1 fobj1;
// 	Func2 fobj2;
// 
// 	typedef Loki::Functor<void> VoidFtor;
//  	VoidFtor funcVoid(func1);
// 	funcVoid();
// 	funcVoid = VoidFtor(&obj, &Cls::memFunc1);
// 	funcVoid();
// 	funcVoid = VoidFtor(fobj1, Loki::Disambiguate());
// 	funcVoid();
// 
// 	typedef Loki::Functor<int, TYPELIST_1(int)> IntFtorInt;
// 	IntFtorInt funcIntInt(func2);
// 	funcIntInt(1);
// 	funcIntInt = IntFtorInt(&obj, &Cls::memFunc2);
// 	funcIntInt(2);
// 	funcIntInt = IntFtorInt(fobj2, Loki::Disambiguate());
// 	funcIntInt(3);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTrialApp object

CTrialApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTrialApp initialization
BOOL CTrialApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
	_HYLIB_USING;
	HYLOGSInfo << hy::log_config("config.xml");
	
// 	mkdir_tree("D:");
// 	mkdir_tree("D:", true);
// 
// 	mkdir_tree("D:\\abc\\def\\ghi\\");
// 	mkdir_tree("D:\\abc1\\def1\\ghi1");
// 	
// 	mkdir_tree("D:\\abc2\\def2\\ghi2\\", true);
// 	mkdir_tree("D:\\abc3\\def3\\ghi3", true);

// 	HYLOGSInfo << _T("log1");
// 	
// 	HYLOGSInfo << _T("log2");
// 	HYLOGSInfo << _T("log3");
// 	HYLOGSInfo << _T("log4");

// 	CDlgTemplate dt;
// 	AfxInitRichEdit();
// 	dt.SetDlg("Hello", WS_POPUP|WS_CAPTION|WS_CAPTION|WS_SYSMENU, 0, 0, 0, 100, 100);
// 	//dt.AddDlgItem("static", WS_CHILD|WS_VISIBLE|SS_LEFT, 0, 0, 0, 40, 20, 2000, 0x0082);
// 	dt.AddDlgItem(RICHEDIT_CLASS10A, "",
// 		WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL,
// 		0, 0, 0, 40, 20, 2000);
// 
// 	CDialog dlg;
// 	dlg.CreateIndirect(dt.Freeze());
// 	dlg.ShowWindow(SW_SHOW);

	//CXMLHandle h(_T("test.xml"));
	
// 	_HYLIB empty_istream ei;
// 	int i = 0;
// 	char sz[10] = {0};
// 	ei >> i >> sz;
// 	_HYLIB empty_ostream eo;
// 	eo << 0 << 0.0f << sz;

// #ifdef _DEBUG
// #	define LOG afxDump
// #else
// #	define LOG eo
// #endif
// 	LOG << 0 << 0.0f << _T("sads");

//	HYLOGInfo << _T("strlog:str intlog:") << 1 << _T("floatlog:") << 1.0f;
//	HYLOGWarn << _T("strlog:str intlog:") << 1 << _T("floatlog:") << 1.0f;
//	HYLOGErr << _T("strlog:str intlog:") << 1 << _T("floatlog:") << 1.0f;

// 	_LOG_HELPER(_HYLIB CDebugStream, _HYLIB CDebugStream(), _T("INFO-")_LOG_PREFIX)
// 		<< 123213 << " " << "huaiyu";

	// Test CResourceMgr
// 	g_pResMgr->Init(_T("D:\\"));
// 	g_pResMgr->GetBitmap(_T("1.bmp"));
// 	g_pResMgr->GetBitmap(_T("/1.bmp"));
// 	g_pResMgr->GetBitmap(_T("\\1.bmp"));
// 	g_pResMgr->GetBitmap(_T("//1.bmp"));
// 	g_pResMgr->GetBitmap(_T("//1/2/3.bmp"));

	//std::cout << std::setbase(8);

// 	SingletonTest1 *pST1 = SingletonTest1::inst();
// 	SingletonTest2 *pST2 = SingletonTest2::inst();
// 	SingletonTest3 *pST3 = SingletonTest3::inst();
// 	SingletonTest4 *pST4 = SingletonTest4::inst();
// 	hy::singleton_mgr &pSM = hy::singleton_mgr::inst();

// 	char buf[100] = {0};
// 	sprintf(buf, "%I64u", 0xFFFFFFFFFFFFFFFF);
// 	TRACE(buf);

	// TODO: ²âÊÔsprintf()/sscanf()µÄÊ¹ÓÃ
// 	char str[1024] = {0};
// 	char cVal1 = 0, cVal2 = 0;
// 	int nVal1 = 0, nVal2 = 0;
// 	char sVal1[1024] = {0}, sVal2[1024] = {0};
// 	double dVal1 = 0.0, dVal2 = 0.0;
// #define CLEAR() ZeroMemory(str, sizeof(str)); cVal1 = cVal2 = 0; nVal1 = nVal2 = 0;	\
// 	ZeroMemory(sVal1, sizeof(sVal1)); ZeroMemory(sVal2, sizeof(sVal2));	dVal1 = dVal2 = 0.0;
// 
// 	CLEAR();
// 	sprintf(str, "%d%d", 10, 10);
// 	sscanf(str, "%d%d", &nVal1, &nVal2);
// 
// 	CLEAR();
// 	sprintf(str, "%d %d", 10, 20);
// 	sscanf(str, "%d %d", &nVal1, &nVal2);
// 	
// 	CLEAR();
// 	sprintf(str, "%d %d", 10, 23);
// 	sscanf(str, "%d%2c%d", &nVal1, sVal1, &nVal2);
// 	
// 	CLEAR();
// 	sprintf(str, "%d %d", 10, 23);
// 	sscanf(str, "%s%s", sVal1, sVal2);
// 
// 	int end = 0;
// #undef CLEAR
	//HYLOGSInfo << hy::log_config("config.xml");

	HYLOGSConfig << hy::log_set_level(hy::LL_INFO)
		<< hy::log_show_all
		//<< hy::log_log_view
		<< hy::log_debugger
		<< hy::log_add_file("log.txt")
		//<< hy::log_add_pipe("logpipe")
		<< hy::log_add_handle(new log_entity_named_pipe_async<true>("logpipe"));
	//g_pLogView->SetMaxLine(30);
	//HYLOGSInfo << hy::log_no_debugger;

	{
		unsigned tid = 0;
		for (int i = 0; i<ARR_LEN(_logTestThrdHdls); ++i) {
			_logTestThrdHdls[i] = (HANDLE)::_beginthreadex(NULL, 0, LogTestThrd, NULL, 0, &tid);
		}
	}

// 	int nInt = sizeof(HYLOGSConfig);
// 	char ch=0, &rch=ch;
// 	nInt = sizeof(hy::line_logger<hy::logger_group>);
// 	nInt = sizeof(hy::scope_logger<hy::logger_group>);
// 	nInt = sizeof(std::vector<int>);
// 	nInt = sizeof(char &);
// 	nInt = sizeof(int &);
// 	nInt = sizeof(rch);
// 	nInt = sizeof(char (*)[2]);
// 	nInt = sizeof(char (&)[2]);
// 	struct ST {
// 		char &ref;
// 		char ch;
// 	};
// 	nInt = sizeof(ST);
// 	//nInt = sizeof(&ST::ref); // error
// 	nInt = sizeof(&ST::ch); // error
// 	nInt = 0;

// 	HYLOGSBeginInfo();
// 	HYLOGSBlkL << "log2(2)=" << hy::static_log2<2>::value;
// 	HYLOGSBlkL << "log2(3)=" << hy::static_log2<3>::value;
// 	HYLOGSBlkL << "log2(4)=" << hy::static_log2<4>::value;
// 	HYLOGSBlkL << "log2(5)=" << hy::static_log2<5>::value;
// 	HYLOGSBlkL << "log2(6)=" << hy::static_log2<6>::value;
// 	HYLOGSBlkL << "log2(7)=" << hy::static_log2<7>::value;
// 	HYLOGSBlkL << "log2(8)=" << hy::static_log2<8>::value;
// 	HYLOGSBlkL << "log2(9)=" << hy::static_log2<9>::value;
// 	HYLOGSBlkL << "log2(10)=" << hy::static_log2<10>::value;
// 	HYLOGSBlkL << "log2(15)=" << hy::static_log2<15>::value;
// 	HYLOGSBlkL << "log2(16)=" << hy::static_log2<16>::value;
// 	HYLOGSBlkL << "log2(17)=" << hy::static_log2<17>::value;
// 	HYLOGSBlkL << "log2(1023)=" << hy::static_log2<1023>::value;
// 	HYLOGSBlkL << "log2(1024)=" << hy::static_log2<1024>::value;
// 	HYLOGSBlkL << "log2(1025)=" << hy::static_log2<1025>::value;
// 	HYLOGSEnd();

// 	HYLOGSInfo << "log2(1)=" << hy::static_log2<1>::value << hy::endl
// 		<< "log2(2)=" << hy::static_log2<2>::value << hy::endl
// 		<< "log2(3)=" << hy::static_log2<3>::value << hy::endl
// 		<< "log2(4)=" << hy::static_log2<4>::value << hy::endl
// 		<< "log2(5)=" << hy::static_log2<5>::value << hy::endl
// 		<< "log2(6)=" << hy::static_log2<6>::value << hy::endl
// 		<< "log2(7)=" << hy::static_log2<7>::value << hy::endl
// 		<< "log2(8)=" << hy::static_log2<8>::value << hy::endl
// 		<< "log2(9)=" << hy::static_log2<9>::value << hy::endl
// 		<< "log2(10)=" << hy::static_log2<10>::value << hy::endl
// 		<< "log2(15)=" << hy::static_log2<15>::value << hy::endl
// 		<< "log2(16)=" << hy::static_log2<16>::value << hy::endl
// 		<< "log2(17)=" << hy::static_log2<17>::value << hy::endl
// 		<< "log2(1023)=" << hy::static_log2<1023>::value << hy::endl
// 		<< "log2(1024)=" << hy::static_log2<1024>::value << hy::endl
// 		<< "log2(1025)=" << hy::static_log2<1025>::value << hy::endl;

// 	HYLOGSInfo << _T("sizeof(long double)") << sizeof(long double);
// 
// 	HYLOGSInfo << _T("log info! ") << 1;
// 	HYLOGSInfo << _T("log info! ") << 2;

// 	HYLOGSInfo << _T("LOG INFO");
// 	HYLOGSWarn << _T("LOG WARN");
// 	HYLOGSErr << _T("LOG ERR");
// 
	hy::random &r = hy::random::inst();
	int i = 0;
// 	for (i=0; i<1000; ++i) {
// 		HYLOGSInfo << "int: " << r.nextInt() << hy::log_endl
// 			<< "char: " << (int)r.nextChar() << hy::log_endl
// 			<< "short: " << r.nextShort() << hy::log_endl
// 			<< "int64: " << r.nextInt64() << hy::log_endl
// 			<< "float: " << std::setprecision(FLT_DIG) << r.nextFloat() << hy::log_endl
// 			<< "double: " << std::setprecision(DBL_DIG) << r.nextDouble() << hy::log_endl;
// 	}
// 	{
// 		HYLOGSGuardInfo();
// 		for (i=0; i<10000; ++i) {
// 			HYLOGSBlk << (int)r.nextBool() << " ";
// 			if (99==i%100) { HYLOGSBlkL; }
// 		}
// 	}
// 	{
// 		for (i=0; i<10000; ++i) {
// 			ASSERT(r.nextUint(UINT_MAX) < UINT_MAX);
// 		}
// 	}
// 	{
// 		static const unsigned int MAX = 128;
// 		int arCount[MAX] = {0};
// 		for (i=0; i<MAX*1000; ++i) { ++arCount[(unsigned)r.nextUint(MAX)]; }
// 		HYLOGSBeginInfo();
// 		for (i=0; i<MAX; ++i) {
// 			HYLOGSBlkL << arCount[i];
// 		}
// 		HYLOGSEnd();
// 	}

//  	hy::CDebugStream::inst() << _T("\n")
// 		<< unsigned(short(-1)) << _T("\n")
// 		<< unsigned(-1) << _T("\n");
//  	hy::log_endl(hy::CDebugStream::inst());
// 
// 	for (i=0; i<10000; ++i) {
// 		HYLOGSInfo << r.nextBool(0.8f);
// 	}

// 	Loki::Tuple<TYPELIST_3(int, float, double)> tup;
// 	Loki::Field<0>(tup) = 1;
// 	Loki::Field<1>(tup) = 1.0f;
// 	Loki::Field<2>(tup) = 1.0;
// 	HYLOGSInfo << Loki::Field<0>(tup) << " "
// 		<< Loki::Field<1>(tup) << " "
// 		<< Loki::Field<2>(tup);

	CTrialDlg dlgMain;
	m_pMainWnd = &dlgMain;
	int nResponse = dlgMain.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CTrialApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	for (int i=0; i<ARR_LEN(_logTestThrdHdls); ++i)
	{
		::TerminateThread(_logTestThrdHdls[i], 0);
		CloseHandle(_logTestThrdHdls[i]);
		_logTestThrdHdls[i] = NULL;
	}

	hy::CLogView::deinst();
	
	return CWinApp::ExitInstance();
}

BEGIN_MESSAGE_MAP(CTrialApp, CWinApp)
	//{{AFX_MSG_MAP(CTrialApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()
