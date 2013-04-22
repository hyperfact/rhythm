// ResourceMgr.cpp: implementation of the CResourceMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ResourceMgr.h"

_HYLIB_BEGIN

#ifdef _USING_MFC
////////////////////////////////////////////////////////////////////////////////
CResourceMgr *CResourceMgr::Instance()
{
	static CResourceMgr inst;
	return &inst;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CResourceMgr::CResourceMgr()
	: m_sResPath(_T("")),
	  m_bInit(FALSE)
{
}

CResourceMgr::~CResourceMgr()
{
	{
		KeyType key;
		HBITMAP hBmp = NULL;
		POSITION pos = m_mapBmp.GetStartPosition();
		for (; pos!=NULL; m_mapBmp.GetNextAssoc(pos, key, hBmp))
		{
			::DeleteObject(hBmp);
		}
		m_mapBmp.RemoveAll();
	}

	{
		KeyType key;
		HFONT hFont = NULL;
		POSITION pos = m_mapFont.GetStartPosition();
		for (; pos!=NULL; m_mapFont.GetNextAssoc(pos, key, hFont))
		{
			::DeleteObject(hFont);
		}
		m_mapFont.RemoveAll();
	}
}

HBITMAP CResourceMgr::_LoadBitmap( LPCTSTR szPath )
{
#define EndChar(s) s[s.GetLength()-1]

	PathType sFullPath(m_sResPath);
	if (sFullPath.IsEmpty() ||
		(EndChar(sFullPath)!=_T('\\') && EndChar(sFullPath)!=_T('/')) ) {
		sFullPath += _T("\\"); }
	sFullPath += szPath;
	ASSERT(::PathFileExists(sFullPath));
	HBITMAP hBmp = (HBITMAP)::LoadImage(NULL, sFullPath, IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE);

	return hBmp;
	
#undef EndChar
}

HBITMAP CResourceMgr::GetBitmap( LPCTSTR szPath )
{
	ASSERT(m_bInit);

	// remove leading chars('/'&'\')
	szPath = ::_tcsspnp(szPath, _T("/\\"));
	ASSERT(szPath && szPath[0]);

	CString sPath(szPath);
	sPath.Replace(_T('/'), _T('\\')); // replace all '/' to '\'

	HBITMAP hBmp = NULL;
	if (m_mapBmp.Lookup(sPath, hBmp)) {
		ASSERT(hBmp != NULL);
		return hBmp;
	}

	hBmp = _LoadBitmap(sPath);
	if (NULL == hBmp) { return NULL; }
	m_mapBmp[sPath] = hBmp;

	return hBmp;
}

HFONT CResourceMgr::GetFont(LPCTSTR lpszFacename /*= _T("")*/,
							int nHeight /*= 0*/,
							BYTE nCharSet /*= ANSI_CHARSET*/,
							BYTE bUnderline /*= FALSE*/,
							BYTE bItalic /*= FALSE*/,
							int nWidth /*= 0*/,
							int nEscapement /*= 0*/,
							int nOrientation /*= 0*/,
							int nWeight /*= FW_NORMAL*/,
							BYTE cStrikeOut /*= 0*/,
							BYTE nOutPrecision /*= OUT_DEFAULT_PRECIS*/,
							BYTE nClipPrecision /*= CLIP_DEFAULT_PRECIS*/,
							BYTE nQuality /*= DEFAULT_QUALITY*/,
							BYTE nPitchAndFamily /*= DEFAULT_PITCH | FF_SWISS */
							)
{
	if (NULL==lpszFacename || NULL==lpszFacename[0]) {
		return (HFONT)::GetStockObject(SYSTEM_FONT);
	}

	HFONT hFont = NULL;

	// 查找已存在的字体
	CString sKey;
	sKey.Format(_T("Font:%s-%08X-%02X-%02X-%02X-%08X-%08X-%08X-%08X-%02X-%02X-%02X-%02X-%02X"),
		lpszFacename, nHeight, nCharSet, bUnderline, bItalic, nWidth, nEscapement,
		nOrientation, nWidth, cStrikeOut, nOutPrecision, nClipPrecision, nQuality,
		nPitchAndFamily);
	if (m_mapFont.Lookup(sKey, hFont)) {
		ASSERT(NULL != hFont);
		return hFont;
	}

	// 新建字体
	hFont = ::CreateFont(nHeight,
		nWidth,
		nEscapement,
		nOrientation,
		nWeight,
		bItalic,
		bUnderline,
		cStrikeOut,
		nCharSet,
		nOutPrecision,
		nClipPrecision,
		nQuality,
		nPitchAndFamily,
		lpszFacename);
	ASSERT(hFont != NULL);

	if (NULL == hFont) {
		return (HFONT)::GetStockObject(SYSTEM_FONT);
	}

	m_mapFont[sKey] = hFont;
	
	return hFont;
}
#endif // _USING_MFC

_HYLIB_END
////////////////////////////////////////////////////////////////////////////////
