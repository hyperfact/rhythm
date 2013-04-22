
// CntrItem.h : CLogViewCntrItem ��Ľӿ�
//

#pragma once

class CLogViewDoc;
class CLogViewView;

class CLogViewCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CLogViewCntrItem)

// ���캯��
public:
	CLogViewCntrItem(REOBJECT* preo = NULL, CLogViewDoc* pContainer = NULL);
		// ע��: ���� pContainer Ϊ NULL ������ IMPLEMENT_SERIALIZE
		//  IMPLEMENT_SERIALIZE Ҫ������д���
		//  �����Ĺ��캯����OLE ��ͨ������
		//  �� NULL �ĵ�ָ�빹���

// ����
public:
	CLogViewDoc* GetDocument()
		{ return reinterpret_cast<CLogViewDoc*>(CRichEditCntrItem::GetDocument()); }
	CLogViewView* GetActiveView()
		{ return reinterpret_cast<CLogViewView*>(CRichEditCntrItem::GetActiveView()); }

// ʵ��
public:
	~CLogViewCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

