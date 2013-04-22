#if !defined(AFX_TMPLDLG_H__EB7A3E00_DE9F_46D7_B88A_92C5E2B76452__INCLUDED_)
#define AFX_TMPLDLG_H__EB7A3E00_DE9F_46D7_B88A_92C5E2B76452__INCLUDED_

// tmpldlg.h : header file
//
////////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "_prolog.h"
#include <exception>
#include <string>
#include <vector>

#ifdef _USING_MFC

////////////////////////////////////////////////////////////////////////////////
_HYLIB_BEGIN
////////////////////////////////////////////////////////////////////////////////
// Prototypes:
class CDlgTemplate;

////////////////////////////////////////////////////////////////////////////////
class CDlgTemplate {
//******************************************************************************
public:
	CDlgTemplate();
	virtual ~CDlgTemplate();

//******************************************************************************
	void SetDlg(LPCSTR pszCaption, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy,
		WORD wMenu=0, WORD wClass=0, LPCSTR pszFontName=NULL, WORD wFontSize=0);
	BOOL AddDlgItem(WORD wClass, LPCSTR pszTitle, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy,
		int nID, WORD wExtraDataSize=0, void *pExtraData=NULL);
	BOOL AddDlgItem(LPCSTR pszClassName, LPCSTR pszTitle, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy,
		int nID, WORD wExtraDataSize=0, void *pExtraData=NULL);

//******************************************************************************
public:
	LPCDLGTEMPLATE Freeze();

//******************************************************************************
protected:
	struct DataEntry {
		DWORD	nLen;
		void	*pData;

		DataEntry() : nLen(0), pData(NULL) {}
	};
	typedef std::vector<DataEntry>	DataVec;
	typedef DataVec::iterator		DataIter;
	typedef	DataVec::const_iterator	DataCIter;
	
	BOOL		m_bFreeze;
	void		*m_pBuf;
	DataEntry	m_dlgTmpl;
	DataVec		m_vecItemTmpl;
};

_HYLIB_END
////////////////////////////////////////////////////////////////////////////////

#endif // _USING_MFC

#endif // !defined(AFX_TMPLDLG_H__EB7A3E00_DE9F_46D7_B88A_92C5E2B76452__INCLUDED_)
