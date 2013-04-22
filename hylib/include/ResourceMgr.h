// ResourceMgr.h: interface for the CResourceMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEMGR_H__904D724C_BC3B_4CE4_B2F0_596F31E602AE__INCLUDED_)
#define AFX_RESOURCEMGR_H__904D724C_BC3B_4CE4_B2F0_596F31E602AE__INCLUDED_

////////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Includes:
#include "_prolog.h"
#include <string>
#include <shlwapi.h>
#include <afxtempl.h>
#include "util_tmpl.h"

_HYLIB_BEGIN
////////////////////////////////////////////////////////////////////////////////

#ifdef _USING_MFC

class HYLIB_DECL CResourceMgr : public noncopyable
{
//******************************************************************************
// Singleton obtaining:
public:
	static CResourceMgr *Instance();

//******************************************************************************
// Types:
public:
	typedef CString PathType;
	typedef CString KeyType;
	typedef CMap<KeyType, LPCTSTR, HBITMAP, HBITMAP> BitmapMap;
	typedef CMap<KeyType, LPCTSTR, HFONT, HFONT> FontMap;

//******************************************************************************
// Initialization methods:
public:
	// 初始化
	void Init(LPCTSTR szResPath);

//******************************************************************************
// Access methods:
public:
	// 获取位图资源
	HBITMAP	GetBitmap(LPCTSTR szPath);

	// 获取字体
	HFONT	GetFont(LPCTSTR lpszFacename = _T(""),
					int nHeight = 0,
					BYTE nCharSet = ANSI_CHARSET,
					BYTE bUnderline = FALSE,
					BYTE bItalic = FALSE,
					int nWidth = 0,
					int nEscapement = 0,
					int nOrientation = 0,
					int nWeight = FW_NORMAL,
					BYTE cStrikeOut = 0,
					BYTE nOutPrecision = OUT_DEFAULT_PRECIS,
					BYTE nClipPrecision = CLIP_DEFAULT_PRECIS,
					BYTE nQuality = DEFAULT_QUALITY,
					BYTE nPitchAndFamily = DEFAULT_PITCH | FF_SWISS
					);

//******************************************************************************
// Con/Destructor(s):
private:
	CResourceMgr();
public:
	virtual ~CResourceMgr();

//******************************************************************************
// Tool methods:
protected:
	HBITMAP	_LoadBitmap(LPCTSTR szPath);

//******************************************************************************
// Fields:
protected:
	BOOL			m_bInit;		// 初始化标志
	CString			m_sResPath;		// 资源路径

	BitmapMap		m_mapBmp;		// 位图资源容器
	FontMap			m_mapFont;		// 字体对象容器
};

//******************************************************************************
// Inlines:
inline void CResourceMgr::Init( LPCTSTR szResPath )
{
	assert(szResPath != NULL);
	assert(::PathFileExists(szResPath));
	m_sResPath = szResPath;
	m_bInit = TRUE;
}

//******************************************************************************
#define g_pResMgr (_HYLIB CResourceMgr::Instance())

#endif // _USING_MFC

_HYLIB_END

////////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_RESOURCEMGR_H__904D724C_BC3B_4CE4_B2F0_596F31E602AE__INCLUDED_)
