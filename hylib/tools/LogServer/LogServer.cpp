
// LogServer.cpp : ����Ӧ�ó��������Ϊ��
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


// CLogServerApp ����

CLogServerApp::CLogServerApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLogServerApp ����

CLogServerApp theApp;


// CLogServerApp ��ʼ��

BOOL CLogServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
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
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	pc.Stop();
	sc.Stop();
	::WSACleanup();

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

