// tmpldlg.cpp : implementation file
//

#include "stdafx.h"
#include "tmpldlg.h"

#ifdef _USING_MFC
#include "macro.h"
#include "util_tmpl.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

_HYLIB_BEGIN

////////////////////////////////////////////////////////////////////////////////
// CDlgTemplate:
CDlgTemplate::CDlgTemplate()
	: m_bFreeze(FALSE)
	, m_pBuf(NULL)
{
}

CDlgTemplate::~CDlgTemplate()
{
	SafeDeleteArray(m_dlgTmpl.pData);
	for (DataIter iter = m_vecItemTmpl.begin();
		iter != m_vecItemTmpl.end();
		++iter)
	{
		SafeDeleteArray(iter->pData);
	}
	m_vecItemTmpl.clear();

	SafeDeleteArray(m_pBuf);
}

void CDlgTemplate::SetDlg(LPCSTR pszCaption, DWORD dwStyle, DWORD dwExStyle,
						int x, int y, int cx, int cy,
						WORD wMenu/*=0*/, WORD wClass/*=0*/,
						LPCSTR pszFontName/*=NULL*/, WORD wFontSize/*=0*/
						)
{
	_HYLIB_USING;

	if (m_bFreeze) { return; }

	ASSERT(pszCaption);

	// clear old data:
	SafeDeleteArray(m_dlgTmpl.pData);
	m_dlgTmpl.nLen = 0;

	BOOL bSetFont = ((NULL!=pszFontName) && (NULL!=pszFontName[0]));
	int nCaptionLen = 1 + ::mbstowcs(NULL,pszCaption,0);
	int nFontLen = bSetFont
		? (1 + ::mbstowcs(NULL, pszFontName, 0))
		: (0);

	// Calc length:
	m_dlgTmpl.nLen = sizeof(DLGTEMPLATE)	/*dialog template*/
		+ 2*sizeof(WORD)					/*menu and class*/
		+ nCaptionLen * sizeof(WCHAR);		/*caption*/
	if (bSetFont)
	{	// font information
		m_dlgTmpl.nLen += sizeof(WORD) + sizeof(WCHAR)*nFontLen;
	}
	m_dlgTmpl.nLen = DWORDAlign(m_dlgTmpl.nLen);

	m_dlgTmpl.pData = new char[m_dlgTmpl.nLen];
	ZeroMemory(m_dlgTmpl.pData, m_dlgTmpl.nLen*sizeof(char));
	ASSERT(m_dlgTmpl.pData != NULL);

	void *pDst = m_dlgTmpl.pData;

	// dialog template header
	DLGTEMPLATE *pDlgTmpl = nextField<DLGTEMPLATE>(&pDst);
	pDlgTmpl->style = dwStyle;
	pDlgTmpl->dwExtendedStyle = dwExStyle;
	pDlgTmpl->cdit = 0;
	pDlgTmpl->x = x;
	pDlgTmpl->y = y;
	pDlgTmpl->cx = cx;
	pDlgTmpl->cy = cy;

	*nextField<WORD>(&pDst) = wMenu;		// menu
	*nextField<WORD>(&pDst) = wClass;	// class

	// caption
	::mbstowcs(nextField<WCHAR>(&pDst, nCaptionLen), pszCaption, nCaptionLen);

	// font information
	if (bSetFont) {
		*nextField<WORD>(&pDst) = wFontSize;
		::mbstowcs(nextField<WCHAR>(&pDst, nFontLen), pszFontName, nFontLen);
	}

	// 4 bytes align:
	pDst = (void *)(DWORDAlign((DWORD_PTR)pDst));

	ASSERT((char *)pDst == ((char *)m_dlgTmpl.pData+m_dlgTmpl.nLen));
}

BOOL CDlgTemplate::AddDlgItem( WORD wClass, LPCSTR pszTitle,
							  DWORD dwStyle, DWORD dwExStyle,
							  int x, int y, int cx, int cy, int nID,
							  WORD wExtraDataSize/*=0*/, void *pExtraData/*=NULL*/ )
{
	_HYLIB_USING;

	if (m_bFreeze) { return FALSE; }

	ASSERT(pszTitle);
	ASSERT(0==wExtraDataSize || pExtraData!=NULL);

	// Check ID unique
	for (DataCIter iter = m_vecItemTmpl.begin();
		iter != m_vecItemTmpl.end();
		++iter) {
		if (nID == static_cast<const DLGITEMTEMPLATE *>(iter->pData)->id) {
			return FALSE;
		}
	}

	// new item:	
	DataEntry item;

	int nTitleLen = 1 + ::mbstowcs(NULL, pszTitle, 0);

	item.nLen = sizeof(DLGITEMTEMPLATE)			/*dialog item template*/
		 		+ 2*sizeof(WORD)				/*class*/
		 		+ nTitleLen * sizeof(WCHAR)		/*title*/
		 		+ sizeof(WORD)+wExtraDataSize;	/*extra data*/
 	item.nLen = DWORDAlign(item.nLen);	// 4 bytes alignment

	item.pData = new char[item.nLen];
	ZeroMemory(item.pData, item.nLen*sizeof(char));
	ASSERT(item.pData != NULL);

	void *pDst = item.pData;

	// dialog item template
	DLGITEMTEMPLATE *pItemTmpl = nextField<DLGITEMTEMPLATE>(&pDst);
	pItemTmpl->style = dwStyle;
	pItemTmpl->dwExtendedStyle = dwExStyle;
	pItemTmpl->x = x;
	pItemTmpl->y = y;
	pItemTmpl->cx = cx;
	pItemTmpl->cy = cy;
	pItemTmpl->id = (WORD)nID;

	// class
	*nextField<WORD>(&pDst) = 0xFFFF;
	*nextField<WORD>(&pDst) = wClass;

	// title:
	::mbstowcs(nextField<WCHAR>(&pDst, nTitleLen), pszTitle, nTitleLen);

	// extra data:
	*nextField<WORD>(&pDst) = wExtraDataSize;
	::memcpy(nextField<char>(&pDst, wExtraDataSize), pExtraData, wExtraDataSize);

	// 4 bytes alignment
	pDst = (void *)(DWORDAlign((DWORD_PTR)pDst));

	ASSERT((char *)pDst == ((char *)item.pData+item.nLen));

	m_vecItemTmpl.push_back(item);
	return TRUE;
}

BOOL CDlgTemplate::AddDlgItem( LPCSTR pszClassName, LPCSTR pszTitle,
							  DWORD dwStyle, DWORD dwExStyle,
							  int x, int y, int cx, int cy, int nID,
							  WORD wExtraDataSize/*=0*/, void *pExtraData/*=NULL*/ )
{
	_HYLIB_USING;
	
	if (m_bFreeze) { return FALSE; }
	
	ASSERT(pszTitle);
	ASSERT(pszClassName);
	ASSERT(0==wExtraDataSize || pExtraData!=NULL);
	
	// Check ID unique
	for (DataCIter iter = m_vecItemTmpl.begin();
	iter != m_vecItemTmpl.end();
	++iter) {
		if (nID == static_cast<const DLGITEMTEMPLATE *>(iter->pData)->id) {
			return FALSE;
		}
	}
	
	// new item:	
	DataEntry item;
	
	int nTitleLen = 1 + ::mbstowcs(NULL, pszTitle, 0);
	int nClassLen = 1 + ::mbstowcs(NULL, pszClassName, 0);
	
	item.nLen = sizeof(DLGITEMTEMPLATE)	/*dialog item template*/
		+ nClassLen *sizeof(WCHAR)		/*class*/
		+ nTitleLen * sizeof(WCHAR)		/*title*/
		+ sizeof(WORD)+wExtraDataSize;	/*extra data*/
	item.nLen = DWORDAlign(item.nLen);	// 4 bytes alignment
	
	item.pData = new char[item.nLen];
	ZeroMemory(item.pData, item.nLen*sizeof(char));
	ASSERT(item.pData != NULL);
	
	void *pDst = item.pData;
	
	// dialog item template
	DLGITEMTEMPLATE *pItemTmpl = nextField<DLGITEMTEMPLATE>(&pDst);
	pItemTmpl->style = dwStyle;
	pItemTmpl->dwExtendedStyle = dwExStyle;
	pItemTmpl->x = x;
	pItemTmpl->y = y;
	pItemTmpl->cx = cx;
	pItemTmpl->cy = cy;
	pItemTmpl->id = (WORD)nID;
	
	// class
	::mbstowcs(nextField<WCHAR>(&pDst, nClassLen), pszClassName, nClassLen);
	
	// title:
	::mbstowcs(nextField<WCHAR>(&pDst, nTitleLen), pszTitle, nTitleLen);
	
	// extra data:
	*nextField<WORD>(&pDst) = wExtraDataSize;
	::memcpy(nextField<char>(&pDst, wExtraDataSize), pExtraData, wExtraDataSize);
	
	// 4 bytes alignment
	pDst = (void *)(DWORDAlign((DWORD_PTR)pDst));
	
	ASSERT((char *)pDst == ((char *)item.pData+item.nLen));
	
	m_vecItemTmpl.push_back(item);
	return TRUE;
}

LPCDLGTEMPLATE CDlgTemplate::Freeze()
{
	if (m_bFreeze) { return static_cast<LPCDLGTEMPLATE>(m_pBuf); }
	
	_HYLIB_USING;
	try
	{
		// set number of items
		static_cast<LPDLGTEMPLATE>(m_dlgTmpl.pData)->cdit = m_vecItemTmpl.size();

		// sum length
		int nBufLen = m_dlgTmpl.nLen;
		DataCIter iter = m_vecItemTmpl.begin();
		for (; iter!=m_vecItemTmpl.end(); ++iter) {
			nBufLen += iter->nLen;
		}
		ASSERT(0 == (nBufLen&3));

		ASSERT(nBufLen != 0);
		ASSERT(NULL == m_pBuf);
		m_pBuf = new char[nBufLen];
		ASSERT(NULL != m_pBuf);
		ZeroMemory(m_pBuf, nBufLen*sizeof(char));

		char *pDst = (char *)m_pBuf;
		::memcpy(pDst, m_dlgTmpl.pData, m_dlgTmpl.nLen);
		pDst += m_dlgTmpl.nLen;

		for (iter = m_vecItemTmpl.begin(); iter!=m_vecItemTmpl.end(); ++iter)
		{
			::memcpy(pDst, iter->pData, iter->nLen);
			pDst += iter->nLen;
		}

		ASSERT(pDst == ((char *)m_pBuf+nBufLen));

		m_bFreeze = TRUE;
	}
	catch (const CException &e)
	{
		(void)e;
		return NULL;
	}
	
	return static_cast<LPCDLGTEMPLATE>(m_pBuf);
}

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // _USING_MFC