#ifndef _HY_STRINGS_H_
#define _HY_STRINGS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
////////////////////////////////////////////////////////////////////////////////
// Includes:
// includes go here
#include "_prolog.h"

#include <vector>
#include <string>

////////////////////////////////////////////////////////////////////////////////
_HYLIB_BEGIN

//******************************************************************************
// ÇÐ¸î×Ö·û´®
HYLIB_DECL int SplitStringStr(__in LPCTSTR szString, __in LPCTSTR szSepStr,
				__out std::vector<std::string> &vecOut);

HYLIB_DECL int SplitStringChr(__in LPCTSTR szString, __in LPCTSTR szCharSet,
				__out std::vector<std::string> &vecOut);

#ifdef _USING_MFC
class CStringW
{
public:
	CStringW(LPCSTR szStr) {
		ASSERT(szStr != NULL);
		int nLen = ::MultiByteToWideChar(CP_ACP, 0, szStr, -1, NULL, 0);
		m_pStrW = ::SysAllocStringLen(NULL, nLen);
		if (NULL == m_pStrW) { AfxThrowMemoryException(); }
		m_nLen = ::MultiByteToWideChar(CP_ACP, 0, szStr, -1, m_pStrW, nLen)-1;
	}
	
	CStringW(LPCWSTR szStr) {
		m_pStrW = ::SysAllocString(szStr);
		if (NULL == m_pStrW) { AfxThrowMemoryException(); }
		m_nLen = ::wcslen(m_pStrW);
	}
	
	CStringW(const CStringW &another) {
		m_pStrW = ::SysAllocString(another.m_pStrW);
		if (NULL == m_pStrW) { AfxThrowMemoryException(); }
		m_nLen = ::wcslen(m_pStrW);
	}
	
	virtual ~CStringW() {
		if (m_pStrW) { ::SysFreeString(m_pStrW); m_pStrW = NULL; }
	}
	
	operator LPCWSTR () const {
		return m_pStrW;
	}
	
	int GetLength() const { m_nLen; }
	
protected:
	LPWSTR	m_pStrW;
	int		m_nLen;
};
#endif // _USING_MFC

////////////////////////////////////////////////////////////////////////////////
_HYLIB_END

#endif // _HY_STRINGS_H_
