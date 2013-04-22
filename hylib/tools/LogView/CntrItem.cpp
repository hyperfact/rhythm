
// CntrItem.cpp : CLogViewCntrItem 类的实现
//

#include "stdafx.h"
#include "LogView.h"

#include "LogViewDoc.h"
#include "LogViewView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogViewCntrItem 的实现

IMPLEMENT_SERIAL(CLogViewCntrItem, CRichEditCntrItem, 0)

CLogViewCntrItem::CLogViewCntrItem(REOBJECT* preo, CLogViewDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: 在此添加一次性构造代码
}

CLogViewCntrItem::~CLogViewCntrItem()
{
	// TODO: 在此处添加清理代码
}


// CLogViewCntrItem 诊断

#ifdef _DEBUG
void CLogViewCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CLogViewCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

