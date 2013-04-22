#if !defined(AFX_LAYEREDWND_H__C62B7C5D_A849_4114_B833_A02FD9ABCC61__INCLUDED_)
#define AFX_LAYEREDWND_H__C62B7C5D_A849_4114_B833_A02FD9ABCC61__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LayeredWnd.h : header file
//
#include "_prolog.h"
#include "LayeredWindow.h"
#include "util_tmpl.h"

_HYLIB_BEGIN

#ifdef _USING_MFC
/////////////////////////////////////////////////////////////////////////////
// CLayeredWnd window
class HYLIB_DECL CLayeredWnd : public CWnd
{
// Construction
public:
	CLayeredWnd();

//******************************************************************************
// Initialization method:
public:
	virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect,
		CWnd* pParentWnd, HMENU hMenu = NULL, CCreateContext* pContext = NULL);

// Attributes
public:

// Operations
public:
#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500) && (_MSC_VER<=1200)
	#if defined(_WINGDI_) && !defined(NOGDI)
		BOOL UpdateLayeredWindow(
			/*[in]*/ HDC hdcDst,
			/*[in]*/ POINT* pptDst,
			/*[in]*/ SIZE* psize,
			/*[in]*/ HDC hdcSrc,
			/*[in]*/ POINT* pptSrc,
			/*[in]*/ COLORREF crKey,
			/*[in]*/ BLENDFUNCTION* pblend,
			/*[in]*/ DWORD dwFlags) {
			ASSERT(::IsWindow(GetSafeHwnd()));
			return ::UpdateLayeredWindow(GetSafeHwnd(), hdcDst, pptDst, psize,
				hdcSrc, pptSrc, crKey, pblend, dwFlags);
		}
	#endif // defined(_WINGDI_) && !defined(NOGDI)

	#if (_WIN32_WINNT >= 0x0501)
		BOOL GetLayeredWindowAttributes(
		/*[out]*/ COLORREF* pcrKey,
		/*[out]*/ BYTE* pbAlpha,
		/*[out]*/ DWORD* pdwFlags) {
			ASSERT(::IsWindow(GetSafeHwnd()));
			return ::GetLayeredWindowAttributes(GetSafeHwnd(), pcrKey,
				pbAlpha, pdwFlags);
		}
	#endif /* _WIN32_WINNT >= 0x0501 */

	BOOL SetLayeredWindowAttributes(
			/*[in]*/ COLORREF crKey,
			/*[in]*/ BYTE bAlpha,
			/*[in]*/ DWORD dwFlags) {
			ASSERT(::IsWindow(GetSafeHwnd()));
			return ::SetLayeredWindowAttributes(GetSafeHwnd(), crKey, bAlpha, dwFlags);
	}
#endif // defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500) && (_MSC_VER<=1200)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayeredWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLayeredWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayeredWnd)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif // _USING_MFC

/////////////////////////////////////////////////////////////////////////////
_HYLIB_END

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LAYEREDWND_H__C62B7C5D_A849_4114_B833_A02FD9ABCC61__INCLUDED_)
