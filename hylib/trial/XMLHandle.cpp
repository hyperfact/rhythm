// XMLHandle.cpp: implementation of the CXMLHandle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "trial.h"
#include "XMLHandle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace MSXML2;

////////////////////////////////////////////////////////////////////////////////
// struct _CImpl:
struct CXMLHandle::_CImpl {
	bool bValid;
	IXMLDOMDocumentPtr	pDoc;
	IXMLDOMElementPtr	pElem;

	_CImpl() {

	}
	
private:
	static struct __StaticInit {
		__StaticInit() {
			::CoInitialize(NULL);
		}
		~__StaticInit() {
			::CoUninitialize();
		}
	} __staticInit;
};
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXMLHandle::CXMLHandle(LPCTSTR fFileName)
 : m_pImpl(new _CImpl)
{
	::CoInitialize(NULL);
	Load(fFileName);
}

CXMLHandle::~CXMLHandle()
{
	::CoUninitialize();
}

BOOL CXMLHandle::Load( LPCTSTR szFileName )
{
	ASSERT(szFileName && szFileName[0]);

	HRESULT hr = m_pImpl->pDoc.CreateInstance(MSXML2::CLSID_DOMDocument);
	if (FAILED(hr)) { return FALSE; }

	VARIANT_BOOL bSuc = m_pImpl->pDoc->load(szFileName);
	if (!bSuc) { return FALSE; }

	//NODE_ELEMENT
	m_pImpl->pElem = m_pImpl->pDoc->GetdocumentElement();
	_bstr_t tagName = m_pImpl->pElem->GettagName();

	IXMLDOMElementPtr pElem = m_pImpl->pElem->selectSingleNode(_T("sysinfo"));

	_bstr_t tagName1 = pElem->GettagName();
	_variant_t attr1 = pElem->getAttribute(_T("id"));
	_variant_t attr2 = pElem->getAttribute(_T("name"));
	
	_bstr_t text = pElem->Gettext();

	//IXMLDOMNodeList	pLst = pElem->GetchildNodes();
	//int len = pLst.Getlength();

	_bstr_t text1 = pElem->selectSingleNode(_T("svrname"))->Gettext();
	_bstr_t text2 = pElem->selectSingleNode(_T("ip"))->Gettext();

	return TRUE;
}

BOOL CXMLHandle::Save( LPCTSTR szFileName )
{
	return FALSE;
}
