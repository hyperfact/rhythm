
// LogServer.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "LogServer.h"
#include "LogServerDlg.h"
#include "ServerCore.h"
#include "PipeClient.h"
#include <tchar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogServerApp

BEGIN_MESSAGE_MAP(CLogServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLogServerApp 构造

CLogServerApp::CLogServerApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CLogServerApp 对象

CLogServerApp theApp;


// CLogServerApp 初始化

BOOL CLogServerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("LogServer by huaiyu"));

	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);

	CString sExePath;
	LPTSTR pPath = sExePath.GetBuffer(MAX_PATH);
	::GetModuleFileName(NULL, pPath, MAX_PATH);
	TCHAR *pSlash = _tcsrchr(pPath, _T('\\'));
	if (pSlash) { *pSlash = 0; }
	sExePath.ReleaseBuffer();

	CString sCfgFile(sExePath + _T("\\config.ini"));
	ServerConfig cfg;
	char sVal[256] = {0};
	::GetPrivateProfileString(_T("Server"), _T("Addr"), _T("0.0.0.0"), sVal, 256, sCfgFile);
	cfg.sAddr = sVal;
	cfg.wPort = ::GetPrivateProfileInt(_T("Server"), _T("Port"), 0, sCfgFile);

	TCHAR sPipe[MAX_PATH] = {0}; // _T("\\\\.\\pipe\\logpipe");
	::GetPrivateProfileString(_T("Pipe"), _T("Name"), _T(""), sPipe, MAX_PATH, sCfgFile);

	CServerCore sc;
	CPipeClient pc;

	sc.Init(cfg);
	sc.Start();

	pc.Init(sPipe, &sc);
	pc.Start();

	CLogServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	pc.Stop();
	sc.Stop();
	::WSACleanup();

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

