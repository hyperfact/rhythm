#ifndef _LAYEREDWINDOW_H_
#define _LAYEREDWINDOW_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
// includes go here
#include "_prolog.h"

////////////////////////////////////////////////////////////////////////////////
// WS_EX_LAYERED
#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500) && (_MSC_VER<=1200)

#	define WS_EX_LAYERED           0x00080000

#	if defined(_WINGDI_) && !defined(NOGDI)
		BOOL WINAPI UpdateLayeredWindow(
			/*[in]*/ HWND hwnd,
			/*[in]*/ HDC hdcDst,
			/*[in]*/ POINT* pptDst,
			/*[in]*/ SIZE* psize,
			/*[in]*/ HDC hdcSrc,
			/*[in]*/ POINT* pptSrc,
			/*[in]*/ COLORREF crKey,
			/*[in]*/ BLENDFUNCTION* pblend,
			/*[in]*/ DWORD dwFlags);
#	endif // defined(_WINGDI_) && !defined(NOGDI)

#	if (_WIN32_WINNT >= 0x0501)
		BOOL WINAPI GetLayeredWindowAttributes(
			/*[in]*/ HWND hwnd,
			/*[out]*/ COLORREF* pcrKey,
		    /*[out]*/ BYTE* pbAlpha,
			/*[out]*/ DWORD* pdwFlags);
#	endif /* _WIN32_WINNT >= 0x0501 */

	BOOL WINAPI SetLayeredWindowAttributes(
		/*[in]*/ HWND hwnd,
		/*[in]*/ COLORREF crKey,
		/*[in]*/ BYTE bAlpha,
		/*[in]*/ DWORD dwFlags);

#	define LWA_COLORKEY            0x00000001
#	define LWA_ALPHA               0x00000002

#	define ULW_COLORKEY            0x00000001
#	define ULW_ALPHA               0x00000002
#	define ULW_OPAQUE              0x00000004

#	define ULW_EX_NORESIZE         0x00000008

#endif /* defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500) && (_MSC_VER<=1200) */

#endif // _LAYEREDWINDOW_H_
