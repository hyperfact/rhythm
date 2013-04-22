#include "StdAfx.h"
#include "strings.h"

_HYLIB_BEGIN

//******************************************************************************
int SplitStringStr( __in LPCTSTR szString, __in LPCTSTR szSepStr,
				   __out std::vector<std::string> &vecOut )
{
	using std::string;

	assert(szString != NULL);
	assert(szSepStr != NULL);

	vecOut.clear();

	const int nSepLen = ::_tcslen(szSepStr);
	if (0 == nSepLen) {
		if (szString[0]) { vecOut.push_back(szString); }
		return (int)vecOut.size();
	}
	
	const TCHAR *pFind = NULL;
	while (pFind = ::_tcsstr(szString, szSepStr))
	{
		if (szString != pFind) {
			vecOut.push_back(string(szString, pFind)); }
		szString = pFind+nSepLen;
	}
	if (szString && szString[0]) { vecOut.push_back(szString); }

	return (int)vecOut.size();
}

int SplitStringChr( __in LPCTSTR szString, __in LPCTSTR szCharSet,
				  __out std::vector<std::string> &vecOut )
{
	using std::string;
	
	assert(szString != NULL);
	assert(szCharSet != NULL);
	
	vecOut.clear();

	if (0 == szCharSet[0]) {
		if (szString[0]) { vecOut.push_back(szString); }
		return (int)vecOut.size();
	}
	
	const TCHAR *pFind = NULL;
	szString = ::_tcsspnp(szString, szCharSet);
	while (szString && (pFind=::_tcspbrk(szString, szCharSet)))
	{
		if (szString != pFind) {
			vecOut.push_back(string(szString, pFind)); }
		szString = ::_tcsspnp(pFind, szCharSet);
	}
	if (szString && szString[0]) { vecOut.push_back(szString); }
	
	return (int)vecOut.size();
}

#ifdef _UNIT_TEST

void Test_SplitStringStr() {
	std::vector<std::string> vec;
	{
		SplitStringStr(_T(""), _T(""), vec);
		SplitStringStr(_T(""), _T("[,]"), vec);
		SplitStringStr(_T("abcdefghi"), _T(""), vec);
		SplitStringStr(_T("abcdefghi"), _T("[,]"), vec);
		SplitStringStr(_T("[,][,]"), _T("[,]"), vec);
		SplitStringStr(_T("abc[,]def[,]ghi"), _T("[,]"), vec);
		SplitStringStr(_T("[,]abc[,]def[,]ghi"), _T("[,]"), vec);
		SplitStringStr(_T("abc[,]def[,]ghi[,]"), _T("[,]"), vec);
		SplitStringStr(_T("[,]abc[,]def[,]ghi[,]"), _T("[,]"), vec);
		SplitStringStr(_T("[,][,]abc[,][,][,]def[,]ghi[,][,]"), _T("[,]"), vec);
	}
}

void Test_SplitStringChr()
{
	std::vector<std::string> vec;
	{
		SplitStringChr(_T(""), _T(""), vec);
		SplitStringChr(_T(""), _T(",; "), vec);
		SplitStringChr(_T("abcdefghi"), _T(""), vec);
		SplitStringChr(_T("abcdefghi"), _T(",; "), vec);
		SplitStringChr(_T(", , ;;, "), _T(",; "), vec);
		SplitStringChr(_T("abc,def ghi"), _T(",; "), vec);
		SplitStringChr(_T(",;abc ,;def,; ghi"), _T(",; "), vec);
		SplitStringChr(_T("abc, def ;,ghi ;,"), _T(",; "), vec);
		SplitStringChr(_T(" ;,abc ,; def  ghi,;;"), _T(",; "), vec);
		SplitStringChr(_T("; ; ;abc ,; ,; def ,;, ;ghi, ;, ;, "), _T(",; "), vec);
	}
}

////////////////////////////////////////////////////////////////////////////////
#endif // _UNIT_TEST

_HYLIB_END
