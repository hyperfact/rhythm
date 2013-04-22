// logview.cpp : implementation file
//

#include "stdafx.h"
#include "logview.h"
#include "util.h"
#include "tmpldlg.h"
#include <richedit.h>

#ifdef _USING_MFC

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

_HYLIB_BEGIN

namespace {
	struct _InitRichEdit : public noncopyable
	{
		static void Guard() {
			static _InitRichEdit _guard;
		}

	protected:
		HMODULE _hModule;
		_InitRichEdit() { _hModule = ::LoadLibrary(_T("Riched20.dll")); }
	public:
		~_InitRichEdit() { if (_hModule) { ::FreeLibrary(_hModule); } }
	};
};

enum {
	IDM_START = 100,
	
	IDM_READONLY = IDM_START,
	IDM_AUTO_SCROLL,
	IDM_ERASE_SEL,
	IDM_CLEAR,
	IDM_SAVE_AS,
	
	IDM_LAST = IDM_SAVE_AS,
};

/////////////////////////////////////////////////////////////////////////////
// CLogView dialog
CLogView::CLogView(CWnd* pParent /*=NULL*/)
	: m_nMaxLine(0)
	, m_nFlag(FLG_NONE)
{
	//{{AFX_DATA_INIT(CLogView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	_InitRichEdit::Guard();

	CDlgTemplate dt;
	dt.SetDlg("LogView", WS_VISIBLE|WS_POPUPWINDOW|WS_CAPTION|WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,
		0, 0, 0, 0, 0);
	dt.AddDlgItem(RICHEDIT_CLASSA, "",
		WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL|ES_READONLY,
		0, 0, 0, 0, 0, IDC_RICHEDIT);
	BT_SET(m_nFlag, FLG_READONLY|FLG_AUTO_SCROLL);
	CreateIndirect(dt.Freeze(), pParent);
	SetWindowPos(NULL, 0, 0, 400, 400, SWP_NOZORDER|SWP_NOMOVE|SWP_SHOWWINDOW);
}

void CLogView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RICHEDIT, m_redit);
}

BEGIN_MESSAGE_MAP(CLogView, CDialog)
	//{{AFX_MSG_MAP(CLogView)
	ON_WM_SIZE()
	ON_COMMAND_RANGE(IDM_START, IDM_LAST, OnMenuCommand)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogView message handlers

void CLogView::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_redit) { SizeWnd(&m_redit, cx, cy); }
}

void CLogView::OnNMRERBtnClk( UINT nFlags, CPoint point )
{
	CMenu menu;

	CPoint pt;
	::GetCursorPos(&pt);
	
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING|(BT_IS_SET(m_nFlag,FLG_READONLY)?MF_CHECKED:0),
		IDM_READONLY, _T("只读"));
	menu.AppendMenu(MF_STRING|(BT_IS_SET(m_nFlag,FLG_AUTO_SCROLL)?MF_CHECKED:0),
		IDM_AUTO_SCROLL, _T("自动滚动"));
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, IDM_ERASE_SEL, _T("删除选择"));
	menu.AppendMenu(MF_STRING, IDM_CLEAR, _T("清空"));
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING|MF_GRAYED, IDM_SAVE_AS, _T("另存为..."));

	menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_LEFTBUTTON, pt.x, pt.y, this);
}

void CLogView::OnMenuCommand( UINT nMenuID )
{
	switch (nMenuID)
	{
	case IDM_READONLY:
		{
			m_nFlag ^= FLG_READONLY;
			m_redit.SetReadOnly(BT_IS_SET(m_nFlag, FLG_READONLY));
		}
		break;
	case IDM_AUTO_SCROLL:
		{
			m_nFlag ^= FLG_AUTO_SCROLL;
		}
		break;
	case IDM_ERASE_SEL:
		{
			m_redit.ReplaceSel(_T(""), TRUE);
		}
		break;
	case IDM_CLEAR:
		{
			m_redit.SetSel(0, -1);
			m_redit.ReplaceSel(_T(""), TRUE);
		}
		break;
	case IDM_SAVE_AS:
		{

		}
		break;
	default:
		break;
	}
}

BOOL CLogView::PreTranslateMessage( MSG* pMsg )
{
	if (BTW_E(pMsg->message, WM_KEYFIRST, WM_KEYLAST)
		&& VK_ESCAPE==pMsg->wParam) {
		return TRUE;
	}
	if (EQU(pMsg->hwnd, m_redit) && EQU(pMsg->message, WM_RBUTTONUP)) {
		OnNMRERBtnClk((UINT)pMsg->wParam, CPoint(LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)));
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLogView::AutoScroll(int nLines/*=INT_MAX*/)
{
	if (BT_IS_SET(m_nFlag, FLG_AUTO_SCROLL)) {
		m_redit.LineScroll(nLines);
	}
}

void CLogView::AppendLog( const char *str )
{
	ASSERT(str);

	int nLinesOld = m_redit.GetLineCount();
	
	// append log
	m_redit.SetSel(-1, -1);
	m_redit.ReplaceSel(str);
	// check maxline
	unsigned nCurLines = m_redit.GetLineCount();
	int nScrollLines = nCurLines - nLinesOld;
	if (0!=m_nMaxLine && nCurLines > m_nMaxLine) {
		m_redit.SetSel(0, m_redit.LineIndex(nCurLines-m_nMaxLine));
		m_redit.ReplaceSel(_T(""));
		nScrollLines = nCurLines-m_nMaxLine;
	}
	
	AutoScroll(nScrollLines);
	
	// show window
	ShowWindow(SW_SHOW);
	FlashWindow(FALSE);
}

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END
#endif // _USING_MFC
