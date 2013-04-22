// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__135FEE9F_C1BE_446C_84E5_6297B73D5F37__INCLUDED_)
#define AFX_STDAFX_H__135FEE9F_C1BE_446C_84E5_6297B73D5F37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0501
#endif

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>
#include <afxdisp.h>        // MFC Automation classes

// TODO: reference additional headers your program requires here
#define _HYLIB_DLL
#define _HYLIB_DECL_DLL

#pragma comment(lib, "Shlwapi.lib")

#include "_prolog.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__135FEE9F_C1BE_446C_84E5_6297B73D5F37__INCLUDED_)
