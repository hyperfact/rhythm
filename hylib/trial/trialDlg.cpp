// trialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "trial.h"
#include "trialDlg.h"

#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrialDlg dialog

CTrialDlg::CTrialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTrialDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTrialDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTrialDlg, CDialog)
	//{{AFX_MSG_MAP(CTrialDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrialDlg message handlers

BOOL CTrialDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	//CWnd::CreateEx()
// 	m_wnd.Create(_T("LayeredWnd"), WS_POPUP|WS_VISIBLE, CRect(100,100,400,400), this, 0);
// 	::SetClassLong(m_wnd, GCL_HBRBACKGROUND, (LONG)(HBRUSH)GetStockObject(DKGRAY_BRUSH));	
// 	using namespace hy;
// 	LayOutWnd(&m_wnd, this, LOS_LEFT|LOS_TOP, 0, 0);

//	m_wnd.SetLayeredWindowAttributes(0, 0x80, LWA_ALPHA);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTrialDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC paintDC(this);
		CMemDC dc(&paintDC);

		HGDIOBJ hOldFont = dc->SelectObject(g_pResMgr->GetFont(_T("ËÎÌå"), 14, GB2312_CHARSET));
		dc->TextOut(0,0,_T("Hello world!"));

		dc->SelectObject(g_pResMgr->GetFont(_T("Î¢ÈíÑÅºÚ"), 20, GB2312_CHARSET));
		dc->TextOut(0,30,_T("Hello world!"));

		dc->SelectObject(g_pResMgr->GetFont(_T("ËÎÌå"), 14, GB2312_CHARSET, TRUE));
		dc->TextOut(0,60,_T("Hello world!"));

		dc->SelectObject(g_pResMgr->GetFont());
		dc->TextOut(0,90,_T("Hello world!"));

		dc->SelectObject(hOldFont);
		//CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTrialDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTrialDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	// TODO: Add your message handler code here
	using namespace hy;
	//if(m_wnd) LayOutWnd(&m_wnd, this, LOS_LEFT|LOS_TOP, 0, 0);
}

void CTrialDlg::OnOK() 
{
	// TODO: Add extra validation here
	static int nCount = 0;
	HYLOGSInfo << _T("Log:") << nCount++;
	//CDialog::OnOK();
}
