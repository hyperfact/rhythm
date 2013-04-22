
// CntrItem.h : CLogViewCntrItem 类的接口
//

#pragma once

class CLogViewDoc;
class CLogViewView;

class CLogViewCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CLogViewCntrItem)

// 构造函数
public:
	CLogViewCntrItem(REOBJECT* preo = NULL, CLogViewDoc* pContainer = NULL);
		// 注意: 允许 pContainer 为 NULL 以启用 IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE 要求类具有带零
		//  参数的构造函数。OLE 项通常是用
		//  非 NULL 文档指针构造的

// 特性
public:
	CLogViewDoc* GetDocument()
		{ return reinterpret_cast<CLogViewDoc*>(CRichEditCntrItem::GetDocument()); }
	CLogViewView* GetActiveView()
		{ return reinterpret_cast<CLogViewView*>(CRichEditCntrItem::GetActiveView()); }

// 实现
public:
	~CLogViewCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

