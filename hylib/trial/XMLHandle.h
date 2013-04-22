// XMLHandle.h: interface for the CXMLHandle class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLHANDLE_H__57418C78_11B4_4B6E_97ED_9A91ABF563E3__INCLUDED_)
#define AFX_XMLHANDLE_H__57418C78_11B4_4B6E_97ED_9A91ABF563E3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <memory>

#import <msxml3.dll> named_guids

class CXMLHandle  
{
//******************************************************************************
// Ctor & Dtor:
public:
	CXMLHandle(LPCTSTR fFileName);
	virtual ~CXMLHandle();

//******************************************************************************
// Methods:
public:
	BOOL IsSuc() const;

	BOOL		Load(LPCTSTR szFileName);
	BOOL		Save(LPCTSTR szFileName);

	BOOL		LoadFromBuf();
	BOOL		GetBuf();

	CXMLHandle	&Root(IXMLElement **pRoot) const;
	
	CXMLHandle	&FirstElem() const;
	CXMLHandle	&CurElem() const;
	CXMLHandle	&NextElem();
	CXMLHandle	&Attribute();
	CXMLHandle	&Name();

//******************************************************************************
// Implementation:
protected:
	struct _CImpl;
	std::auto_ptr<_CImpl> m_pImpl;
};

#endif // !defined(AFX_XMLHANDLE_H__57418C78_11B4_4B6E_97ED_9A91ABF563E3__INCLUDED_)
