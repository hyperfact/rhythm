// LayeredWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LayeredWnd.h"
#include "compare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

_HYLIB_BEGIN

#ifdef _USING_MFC
namespace {
	CString	_sLayeredWndClsName;
	LPCTSTR _RegisterLayeredWndCls() {
		if (_sLayeredWndClsName.IsEmpty()) {
			_sLayeredWndClsName = ::AfxRegisterWndClass(
				CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS);
			ASSERT(!_sLayeredWndClsName.IsEmpty());
		}
		return _sLayeredWndClsName;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CLayeredWnd

CLayeredWnd::CLayeredWnd()
{
}

CLayeredWnd::~CLayeredWnd()
{
}

BOOL CLayeredWnd::Create( LPCTSTR lpszWindowName, DWORD dwStyle,
						 const RECT& rect, CWnd* pParentWnd,
						 HMENU hMenu/* = NULL*/, CCreateContext* pContext /*= NULL*/ )
{
	ASSERT(BT_IS_CLEAR(dwStyle, WS_CHILD));

	BT_CLEAR(dwStyle, WS_CHILD);
	return CreateEx(WS_EX_LAYERED, _RegisterLayeredWndCls(), lpszWindowName, dwStyle,
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		pParentWnd?pParentWnd->GetSafeHwnd():NULL, hMenu, (LPVOID)pContext);
}

BEGIN_MESSAGE_MAP(CLayeredWnd, CWnd)
	//{{AFX_MSG_MAP(CLayeredWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayeredWnd message handlers

#endif // _USING_MFC

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END
