
// LogViewView.cpp : CLogViewView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "LogView.h"
#endif

#include "LogViewDoc.h"
#include "CntrItem.h"
#include "resource.h"
#include "LogViewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum {
	WMU_APPEND_TEXT = WM_USER+1,
};

// CLogViewView

IMPLEMENT_DYNCREATE(CLogViewView, CRichEditView)

BEGIN_MESSAGE_MAP(CLogViewView, CRichEditView)
	ON_WM_DESTROY()
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLogViewView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()

	ON_MESSAGE(WMU_APPEND_TEXT, &CLogViewView::OnMsgAppendText)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CLogViewView 构造/析构

CLogViewView::CLogViewView()
	: m_buf(4096)
{
	// TODO: 在此处添加构造代码

}

CLogViewView::~CLogViewView()
{
}

BOOL CLogViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CRichEditView::PreCreateWindow(cs);
}

void CLogViewView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();


	// 设置打印边距(720 缇 = 1/2 英寸)
	SetMargins(CRect(720, 720, 720, 720));
}


// CLogViewView 打印


void CLogViewView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLogViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}


void CLogViewView::OnDestroy()
{
	// 析构时停用此项；这在
	// 使用拆分视图时非常重要 
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
   CRichEditView::OnDestroy();
}


void CLogViewView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLogViewView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

void CLogViewView::AppendText( LPCTSTR sText )
{
	CSingleLock l(&m_lock, TRUE);

	size_t nLen = ::_tcslen(sText)+sizeof(TCHAR);
	if (0 == nLen) { return; }

	nLen = m_buf.write(sText, nLen);

	PostMessage(WMU_APPEND_TEXT, nLen, 0);
}

LRESULT CLogViewView::OnMsgAppendText( WPARAM wp, LPARAM lp )
{
	size_t nLenRd = (size_t)wp;
	hy::fixed_buf<> bufRd(4096);
	{
		CSingleLock l(&m_lock, TRUE);
		void *pWr = bufRd.wlock(nLenRd);
		nLenRd = m_buf.read(pWr, nLenRd);
		bufRd.wunlock(pWr, nLenRd);
	}

	CRichEditCtrl &ctrl = GetRichEditCtrl();
	ctrl.SetRedraw(FALSE);
	CHARRANGE crSaved = {0};
	ctrl.GetSel(crSaved);
	ctrl.SetSel(-1, -1);

	const void *pRd = bufRd.rlock(nLenRd);
	ctrl.ReplaceSel((LPCTSTR)pRd);
	bufRd.runlock(pRd, nLenRd);

	ctrl.SetSel(crSaved);
	ctrl.SetRedraw(TRUE);

	ctrl.Invalidate(TRUE);

	return S_OK;
}

// CLogViewView 诊断

#ifdef _DEBUG
void CLogViewView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CLogViewView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CLogViewDoc* CLogViewView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogViewDoc)));
	return (CLogViewDoc*)m_pDocument;
}

#endif //_DEBUG


// CLogViewView 消息处理程序

int CLogViewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	DWORD dwStyle = this->GetStyle();

	this->ModifyStyle(ES_AUTOVSCROLL, 0);
	//cs.style &= (~ES_AUTOVSCROLL);
	dwStyle = this->GetStyle();

	// 新建字体
	m_font.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		GB2312_CHARSET,            // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("宋体"));    
	ASSERT(m_font.GetSafeHandle() != NULL);
	SetFont(&m_font);

	return 0;
}
