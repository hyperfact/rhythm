// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B8A74D90_0F52_4624_89E0_5D7B496EA804__INCLUDED_)
#define AFX_STDAFX_H__B8A74D90_0F52_4624_89E0_5D7B496EA804__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0501
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>
#include <afxdisp.h>        // MFC Automation classes

// TODO: reference additional headers your program requires here

#pragma comment(lib, "Shlwapi.lib")

#include "_prolog.h"

#ifndef TIXML_USE_STL
#	define TIXML_USE_STL
#endif // TIXML_USE_STL

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B8A74D90_0F52_4624_89E0_5D7B496EA804__INCLUDED_)
