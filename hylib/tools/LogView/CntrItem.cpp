
// CntrItem.cpp : CLogViewCntrItem ���ʵ��
//

#include "stdafx.h"
#include "LogView.h"

#include "LogViewDoc.h"
#include "LogViewView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLogViewCntrItem ��ʵ��

IMPLEMENT_SERIAL(CLogViewCntrItem, CRichEditCntrItem, 0)

CLogViewCntrItem::CLogViewCntrItem(REOBJECT* preo, CLogViewDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: �ڴ����һ���Թ������
}

CLogViewCntrItem::~CLogViewCntrItem()
{
	// TODO: �ڴ˴�����������
}


// CLogViewCntrItem ���

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

