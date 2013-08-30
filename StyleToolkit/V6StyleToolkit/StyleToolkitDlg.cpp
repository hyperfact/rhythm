// StyleToolkitDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StyleToolkit.h"
#include "StyleToolkitDlg.h"

#include "Templates.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitDlg dialog

CStyleToolkitDlg::CStyleToolkitDlg(CWnd* pParent /*=NULL*/)
	: StyleDialog(CStyleToolkitDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bBackgroundsSet = FALSE;
}

void CStyleToolkitDlg::DoDataExchange(CDataExchange* pDX)
{
	StyleDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1,			m_cProgress1);
	DDX_Control(pDX, IDC_PROGRESS2,			m_cProgress2);
	DDX_Control(pDX, IDC_PROGRESS3,			m_cProgress3);
	DDX_Control(pDX, IDC_PROGRESS4,			m_cProgress4);
	DDX_Control(pDX, IDC_PSTEP1,			m_cPStep1);
	DDX_Control(pDX, IDC_PSTEP2,			m_cPStep2);
	DDX_Control(pDX, IDC_PSTEP3,			m_cPStep3);
	DDX_Control(pDX, IDC_PSTEP4,			m_cPStep4);
	DDX_Control(pDX, IDC_DIVIDER1,			m_cDiv1);
	DDX_Control(pDX, IDC_DIVIDER2,			m_cDiv2);
	DDX_Control(pDX, IDC_VDIV1,				m_cVDiv1);
	DDX_Control(pDX, IDC_VISTSTYLE1NORM,	m_cVistaStyle1Norm);
	DDX_Control(pDX, IDC_VISTSTYLE1CHECK,	m_cVistaStyle1Checked);
	DDX_Control(pDX, IDC_VISTSTYLE1HOVER,	m_cVistaStyle1NormH);
	DDX_Control(pDX, IDC_VISTSTYLE1PRESS,	m_cVistaStyle1NormP);
	DDX_Control(pDX, IDC_VISTSTYLE1CHECKT,	m_cVistaStyle1CheckedT);
	DDX_Control(pDX, IDC_VISTSTYLE1CHECKTD, m_cVistaStyle1CheckedD);
	DDX_Control(pDX, IDC_AQUA1,				m_cAqua1);
	DDX_Control(pDX, IDC_AQUA2,				m_cAqua2);
	DDX_Control(pDX, IDC_AQUA3,				m_cAqua3);
	DDX_Control(pDX, IDC_STRINGFRAME,		m_cStringFrame);
	DDX_Control(pDX, IDC_VISTSTYLE2NORM,	m_cVistaStyle2Norm);
	DDX_Control(pDX, IDC_VISTSTYLE2HOT,		m_cVistaStyle2Hot);
	DDX_Control(pDX, IDC_EDIT1,				m_cEdit1);
	DDX_Control(pDX, IDC_EDIT2,				m_cEdit2);
}

BEGIN_MESSAGE_MAP(CStyleToolkitDlg, StyleDialog)
	//{{AFX_MSG_MAP(CStyleToolkitDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PSTEP1, OnPstep1)
	ON_BN_CLICKED(IDC_PSTEP2, OnPstep2)
	ON_BN_CLICKED(IDC_PSTEP3, OnPstep3)
	ON_BN_CLICKED(IDC_PSTEP4, OnPstep4)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStyleToolkitDlg message handlers

BOOL CStyleToolkitDlg::OnInitDialog()
{
	// configure StyleDialog before calling init
	SetControls(ALL_CONTROLS);
	SetTitle("Style Toolkit");

	StyleDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	m_cPStep1.LoadStdImage(IDR_PSTEP1, _T("PNG"));
	m_cPStep2.LoadStdImage(IDR_PSTEP2, _T("PNG"));
	m_cPStep3.LoadStdImage(IDR_PSTEP3, _T("PNG"));
	m_cPStep4.LoadStdImage(IDR_PSTEP4, _T("PNG"));

	// create the frame
	CreateFrame();

	// create the stacks
	CreateStacks();

	
	return TRUE; 
}

void CStyleToolkitDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		StyleDialog::OnSysCommand(nID, lParam);
	}
}

void CStyleToolkitDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		StyleDialog::OnPaint();
	}
}

HCURSOR CStyleToolkitDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


BOOL CStyleToolkitDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	
	GetFrameRect(rect);

	StyleDialog::OnEraseBkgnd(pDC);

	m_Style.PaintStyle(pDC, rect);

	if(!m_bBackgroundsSet)
	{
		MemDC pDevC(pDC, rect, TRUE);
		SetBackgrounds(pDevC, rect);
	}

	return TRUE;
}

void CStyleToolkitDlg::CreateStacks()
{
	/////////////////////////////////
	////////// BACKGROUND ///////////
	/////////////////////////////////

	CRect rect;	GetClientRect(rect);
	Stack BackPane(rect); 

	// the background fill
	BackPane.FillSolid(rect, Black);
	// this supresses some anti alias behavior
	BackPane.SetOuterBorder(1, Black);

	// add this stack to the style 
	m_Style.AddStack(BackPane);

	/////////////////////////////////

	// tri gradient with top bar example
	CreateTealPane();

	// progress bar examples
	CreateProgressBars();

	// button examples
	CreateButtonPane();

	// the style1 buttons
	CreateVistaStyle1Buttons();

	// the aqua style buttons
	CreateAquaStyleButtons();

	// background image example
	CreateRiverPane();

	// the style2 buttons
	CreateVistaStyle2Buttons();

	// edit boxes example
	CreateEditBoxes();

}


void CStyleToolkitDlg::OnPstep1() 
{
	m_cProgress1.StepIt();
}

void CStyleToolkitDlg::OnPstep2() 
{
	m_cProgress2.StepIt();
}

void CStyleToolkitDlg::OnPstep3() 
{
	m_cProgress3.StepIt();
}

void CStyleToolkitDlg::OnPstep4() 
{
	m_cProgress4.StepIt();
}


#define GETCLIENTRECT(rc, control) CRect rc; control.GetClientRect(rc); rc.OffsetRect(-rc.left, -rc.top);
#define GETSCREENRECT(rc, control) CRect rc; control.GetWindowRect(rc); ScreenToClient(rc);
#define GETCONTROLRECT(rc, control) CRect rc; control.GetWindowRect(rc); rc.OffsetRect(-rc.left, -rc.top);


/////////////////////////////////////////////
/////////// TealPane ////////////////////////
/////////////////////////////////////////////
void CStyleToolkitDlg::CreateTealPane()
{
	// define the rectangle
	CRect rect; GetClientRect(rect);  CRect div1; m_cDiv1.GetWindowRect(div1); 
	ScreenToClient(div1); rect.bottom = div1.top; rect.left++; rect.top++;

	// create the stack
	Stack TealPane(rect);

	// set the OuterBorder params
	TealPane.SetOuterBorder(1, Black);

	// set the Inner Border params
	TealPane.SetInnerBorder(2, RGB(135, 192, 185), RGB(109, 165, 168));

	// 3 color gradient
	TealPane.FillGrad3(HORIZ, RGB(9, 74, 116), RGB(32, 121, 140), RGB(5, 111, 90)); 

	// add a top edge effect
	TealPane.FillBar(rect, TOP_EDGE, 20, Clr(180,RGB(135, 192, 185)), Clr(0,White));

	// add the shield image
	TealPane.AddImage(CPoint(15, 36), IDR_SHIELD, _T("PNG"), 255);

	// create display strings
	CString str1(_T("Progress Styles"));	CPoint pt1(150, 25);
	CString str2(_T("Indicator Styles"));	CPoint pt2(380, 25);

	// add the strings 
	TealPane.AddString(str1, pt1, RGB(148, 215, 255), 14, FONT_REG, L"Swiss");
	TealPane.AddString(str2, pt2, RGB(148, 215, 255), 14, FONT_REG, L"Swiss");

	// add to style
	m_Style.AddStack(TealPane);
}


//////////////////////////////////////////////////////////////////////////////////
///////// Create Progress Bars ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void CStyleToolkitDlg::CreateProgressBars()
{
	///////////////////////////////////////////////////////////////////////////////
	// Progress 1 - Uses default control stacks - not much to do here

	m_cProgress1.SetRange(0, 100);
	m_cProgress1.SetPos(50);		// this call will pre-create the stacks

	// prepaint this control
	CRect tr; m_cProgress1.GetWindowRect(tr); ScreenToClient(tr);
	tr.DeflateRect(1,1);

	Stack st0 = m_cProgress1.m_Style.m_Stacks[0];
	Stack st1 = m_cProgress1.m_Style.m_Stacks[1];
	st0.SetPosition(tr.left, tr.top);
	tr.DeflateRect(1,1);
	st1.SetPosition(tr.left, tr.top);
	m_Style.AddStack(st0);
	m_Style.AddStack(st1);

	///////////////////////////////////////////////////////////////////////////////
	// Progress 2

	// define some colors
	COLORREF BorderColor = RGB(0xB2, 0xB2, 0xB2);
	COLORREF GrayColor = RGB(0xC9, 0xC9, 0xC9);
	COLORREF ProgGreen = RGB(0,0xd3,0x28);

	// Get the progress control rect
	GETCLIENTRECT(pc2rect, m_cProgress2);

	//=============== Background Layers ===================/
	Stack ProgressBack(pc2rect);

	// define the border params
	ProgressBack.SetShape(ROUNDRECT, 8);
	ProgressBack.SetOuterBorder(1, RGB(64, 160, 185));
	ProgressBack.SetMiddleBorder(1, Clr(64,White), Clr(0,White));

	// add the gray laver
	ProgressBack.FillSolid(pc2rect, GrayColor);

	// now it can be deflated, use a temporary version since it 
	// will be needed later
	CRect tmprc(pc2rect); tmprc.DeflateRect(1,2); 
	
	ProgressBack.FillBar(tmprc, LEFT_EDGE, 10, Clr(40,Black), Clr(0,White));
	ProgressBack.FillBar(tmprc, RIGHT_EDGE, 10, Clr(0,White), Clr(40,Black));
	ProgressBack.FillBar(tmprc, TOP_EDGE, 5, Clr(120,White), Clr(40,White));
	
	float profile[3] = { 0.0f, 0.8f, 1.0f };
	ProgressBack.FillBar(tmprc, BOTTOM_EDGE, 5, Clr(15,White), Clr(120,White),
														Clr(15,White), profile);

	//=============== Foreground Layers ===================/
	int StepLayerIdx;

	// this stack doesn't have a border, so deflate the rect
	// to make the background's border visible
	pc2rect.DeflateRect(1, 1); 
	Stack ProgressFore(pc2rect);

	// make it a round rect
	ProgressFore.SetShape(ROUNDRECT, 8);

	// make this border semi transparent
	ProgressFore.SetOuterBorder(1, Clr(160,White), Clr(0,White));

	// add the gradient layer
	ProgressFore.FillGrad3(pc2rect, HORIZ, ProgGreen, Yellow, Red);

	// add the effects layers
	StepLayerIdx = ProgressFore.FillBar(pc2rect, LEFT_EDGE, 19, Clr(40,Black), Clr(0,White));
	ProgressFore.FillBar(pc2rect, RIGHT_EDGE, 20, Clr(0,White), Clr(40,Black));
	ProgressFore.FillBar(pc2rect, TOP_EDGE, 6, Clr(160,White), Clr(100,White));
	ProgressFore.FillBar(pc2rect, BOTTOM_EDGE, 6, Clr(15,White), Clr(50,White));

	// add the background
	m_cProgress2.AddStack(ProgressBack);

	// layer zero added with this function is the one that will step
	m_cProgress2.AddStepStack(ProgressFore);

	// over ride to layer one as the step layer
	m_cProgress2.SetStepLayer(StepLayerIdx);

	// add some text
	Stack TextStack(pc2rect);
	TextStack.AddString("Used Disk Space", CPoint(35, 7), Blue);

	m_cProgress2.AddStack(TextStack);
	m_cProgress2.SetPos(90);

	///////////////////////////////////////////////////////////////////////////////
	// Progress 3

	// Get the progress control rect
	GETCLIENTRECT(pc3rect, m_cProgress3);

	COLORREF topclr = RGB(0xba,0xba,0xba);
	COLORREF mdlclr = RGB(0xf0,0xf0,0xf0);
	COLORREF btmclr = RGB(0xe3,0xe3,0xe3);

	Stack ProgressBack3(pc3rect);
	ProgressBack3.SetOuterBorder(1, Gray);
	ProgressBack3.FillGrad3(VERT, Clr(255, topclr), Clr(255, mdlclr), 
														Clr(255, btmclr), 0.65f);

	pc3rect.InflateRect(-3, -3);

	Stack ProgressTop3(pc3rect);
	ProgressTop3.SetShape(ROUNDRECT, 11);
	ProgressTop3.SetOuterBorder(1, RGB(128, 128, 185));
	ProgressTop3.FillSolid(pc3rect, Blue);

	COLORREF lightblue = RGB(0x9f,0xe6,0xff);

	ProgressTop3.FillBar(TOP_EDGE, 10, Clr(225, lightblue), Clr(16,White));

	float profile3[3] = { 0.0f, 0.9f, 1.0f};
	ProgressTop3.FillBar(BOTTOM_EDGE, 14, CLEAR, Clr(200,lightblue), Clr(40,lightblue), 
																					profile3);

	m_cProgress3.AddStack(ProgressBack3);
	m_cProgress3.AddStepStack(ProgressTop3);

	m_cProgress3.SetPos(70);

	///////////////////////////////////////////////////////////////////////////////
	// Progress 4

	// Get the progress control rect
	GETCLIENTRECT(pc4rect, m_cProgress4);

	// the background is transparent, but
	// the stack is needed to hold the background information
	Stack ProgressBack4(pc4rect);

	pc4rect.InflateRect(-1, -1);

	Stack ProgressTop4(pc4rect);
	ProgressTop4.SetOuterBorder(1, RGB(64, 160, 185));

	ProgressTop4.FillGrad3(pc4rect, HORIZ, Red, Yellow, Clr(255, RGB(0,192,0)));

	StepLayerIdx = ProgressTop4.FillWrap(pc4rect, TILE, 14, Clr(128,White), Clr(0,0), 
																			Clr(128,Black));
	
	m_cProgress4.AddStack(ProgressBack4);
	m_cProgress4.AddStepStack(ProgressTop4);
		
	// over ride to layer one as the step layer
	m_cProgress4.SetStepLayer(StepLayerIdx);

	m_cProgress4.SetPos(90);
}

/////////////////////////////////////////////
/////////// Button Pane /////////////////////
/////////////////////////////////////////////
void CStyleToolkitDlg::CreateButtonPane()
{
	GETSCREENRECT(rect1, m_cDiv1);
	GETSCREENRECT(rect2, m_cDiv2);
	GETSCREENRECT(rect3, m_cVDiv1);

	////////////////////////////////////////////////
	//  Left Side Background

	CRect srect = rect1; srect.bottom = rect2.top; 
	srect.right = rect3.left;

	// allow the background edge
	srect.DeflateRect(1,1);

	Stack ButtonPane(srect);

	// set as rounded rectangle 
	//ButtonPane.SetShape(TRANSITION, 8);

	COLORREF brdr = 0x00b0b0b0;

	// set the OuterBorder size and color
	ButtonPane.SetOuterBorder(5, brdr);
	ButtonPane.SetMiddleBorder(1, Black);

	// add the tlclr space image
	ButtonPane.AddImage(CPoint(0, 0), CPoint(0, 0), IDR_METAL, _T("JPG"));

	// create a display string
	CString str1(_T("Vista Style 1")); CPoint pt1(84, 156);

	// add the string 
	ButtonPane.AddString(str1, pt1, Silver, 16, FONT_REG, L"Swiss");

	// add to stack
	m_Style.AddStack(ButtonPane);

	////////////////////////////////////////////////////////////////////////////////
	// Right Side background

	CRect wrect = rect1; wrect.bottom = rect2.top; wrect.left = rect3.left;

	wrect.DeflateRect(1, 1); wrect.bottom--;

	RaisedFrame rf(wrect, CLEAR, Orange, 10);
	
	Stack RightSide(wrect);

	RightSide.FillSolid(wrect, Orange);

	RightSide.FillGrad3(DIAGF, Clr(64, White), CLEAR, Clr(16,Black));

	// create a display string
	CString str2(_T("Aqua\nStyles")); CPoint pt2(26, 125);

	// add the string 
	RightSide.AddString(str2, pt2, Brown, 18, FONT_BOLD, L"Swiss");

	// add to stack
	m_Style.AddStack(RightSide);

	// add to stack
	m_Style.AddStack(rf.RFrame);
}

/////////////////////////////////////////////
/////////// Vista Style 1 Buttons ///////////
/////////////////////////////////////////////
void CStyleToolkitDlg::CreateVistaStyle1Buttons()
{
	// all these buttons are the same size, so only need to do this once
	GETCONTROLRECT(rc, m_cVistaStyle1Norm);
	// allow room for the pressed state, since some of them are auto generated
	rc.right--; rc.bottom--;

	VistaButton1 vbs1(rc);

	//========= Normal Button ===========//
	Stack Top1;
	Top1.SetRect(rc);
	Top1.AddAlignString("Std", ALIGN_CENTER, CPoint(0,0), White, 16);
	Top1.AddImage(CPoint(8, 8), IDR_DB16, _T("PNG"));
	
	// add the standard style
	Style std; std.AddStack(vbs1.VBStd + Top1);
	m_cVistaStyle1Norm.LoadStdStyle(std);

	// add the hot style
	Style hot; hot.AddStack(vbs1.VBHot + Top1);
	m_cVistaStyle1Norm.LoadHotStyle(hot);

	// add the pressed style
	Style press; press.AddStack(vbs1.VBPress + Top1);
	m_cVistaStyle1Norm.LoadPressedStyle(press);

	//========= Toggle Button ===========//
	Stack Top2;
	Top2.SetRect(rc);
	Top2.AddAlignString("Alt", ALIGN_CENTER, CPoint(0,0), White, 16);
	Top2.AddImage(CPoint(8, 8), IDR_ADD16, _T("PNG"));

	// add the standard style
	Style std2; std2.AddStack(vbs1.VBStd + Top2);
	m_cVistaStyle1Checked.LoadStdStyle(std2);

	// add the hot style
	Style hot2; hot2.AddStack(vbs1.VBHot + Top2);
	m_cVistaStyle1Checked.LoadHotStyle(hot2);

	// add the pressed style
	Style press2; press2.AddStack(vbs1.VBPress + Top2);
	m_cVistaStyle1Checked.LoadPressedStyle(press2);

	// add the alt style
	Style alt2; alt2.AddStack(vbs1.VBAlt + Top2);
	m_cVistaStyle1Checked.LoadAltStyle(alt2);
	m_cVistaStyle1Checked.LoadAltPressedStyle(alt2);
	m_cVistaStyle1Checked.EnableToggle(TRUE);


	//=========================================================================//
	// ----- These are just for Demo so use they load the std styles ----------//
	//=========================================================================//
	Stack Top1a;
	Top1a.SetRect(rc);
	Top1a.AddAlignString("Hover", ALIGN_CENTER, CPoint(10, 0), White, 16);
	Top1a.AddImage(CPoint(8, 8), IDR_DB16, _T("PNG"));
	
	// add the standard style
	Style std1a; std1a.AddStack(vbs1.VBHot + Top1a);
	m_cVistaStyle1NormH.LoadStdStyle(std1a);

	Stack Top1b;
	Top1b.SetRect(rc);
	Top1b.AddAlignString("Pressed", ALIGN_CENTER, CPoint(12, 0), White, 16);
	Top1b.AddImage(CPoint(8, 8), IDR_DB16, _T("PNG"));
	
	// add the standard style
	Style std1b; std1b.AddStack(vbs1.VBPress + Top1b);
	m_cVistaStyle1NormP.LoadStdStyle(std1b);

	Stack Top2a;
	Top2a.SetRect(rc);
	Top2a.AddAlignString("Toggle", ALIGN_CENTER, CPoint(11, 0), White, 16);
	Top2a.AddImage(CPoint(8, 8), IDR_ADD16, _T("PNG"));
	
	// add the standard style
	Style std2a; std2a.AddStack(vbs1.VBAlt + Top2a);
	m_cVistaStyle1CheckedT.LoadStdStyle(std2a);

	Stack Top2b;
	Top2b.SetRect(rc);
	Top2b.AddAlignString("Disable", ALIGN_CENTER, CPoint(11, 0), White, 16);
	Top2b.AddImage(CPoint(8, 8), IDR_ADD16, _T("PNG"));
	
	// add the standard style
	Style std2b; std2b.AddStack(vbs1.VBAlt + Top2b);
	m_cVistaStyle1CheckedD.LoadStdStyle(std2b);
	m_cVistaStyle1CheckedD.EnableButton(FALSE);
}

/////////////////////////////////////////////
/////////// Aqua Style Buttons //////////////
/////////////////////////////////////////////
void CStyleToolkitDlg::CreateAquaStyleButtons()
{
	GETCONTROLRECT(rc, m_cAqua1);
	AquaButton aqua1(rc, Yellow, White, Orange, "Normal");

	// add the standard style
	m_cAqua1.LoadStdStyle(aqua1.Std);
	m_cAqua1.LoadHotStyle(aqua1.Hot);
	m_cAqua1.LoadPressedStyle(aqua1.Press);

	GETCONTROLRECT(rc2, m_cAqua2);
	AquaButton aqua2(rc2, White, Orange, Orange, "Hot");

	// add the standard style
	m_cAqua2.LoadStdStyle(aqua2.Std);
	m_cAqua2.LoadHotStyle(aqua2.Hot);
	m_cAqua2.LoadPressedStyle(aqua2.Press);

	GETCONTROLRECT(rc3, m_cAqua3);
	AquaButton aqua3(rc3, RGB(0,230,0), Orange, Orange, "Press\nMe", 16);

	// add the standard style
	m_cAqua3.LoadStdStyle(aqua3.Std);
	m_cAqua3.LoadHotStyle(aqua3.Hot);
	m_cAqua3.LoadPressedStyle(aqua3.Press);

	// There is some delay until the control gets around
	// to painting the buttons.  This code will prepaint
	// the buttons so the delay isn't apparent 
	GETSCREENRECT(tr, m_cAqua1);
	aqua1.StdStack.SetPosition(tr.left, tr.top);
	m_Style.AddStack(aqua1.StdStack);

	GETSCREENRECT(tr2, m_cAqua2);
	aqua2.StdStack.SetPosition(tr2.left, tr2.top);
	m_Style.AddStack(aqua2.StdStack);

	GETSCREENRECT(tr3, m_cAqua3);
	aqua3.StdStack.SetPosition(tr3.left, tr3.top);
	m_Style.AddStack(aqua3.StdStack);
}


/////////////////////////////////
/////////// RiverPane ///////////
/////////////////////////////////
void CStyleToolkitDlg::CreateRiverPane()
{
	CRect rect;	GetClientRect(rect); CRect div2; m_cDiv2.GetWindowRect(div2);
	ScreenToClient(div2); rect.top = div2.top; rect.left++;

	rect.DeflateRect(1,1);

	Stack RiverPane(rect);

	// set as rounded rectangle 
	//RiverPane.SetShape(TRANSITION, 13);

	// set the OuterBorder size and color
	RiverPane.SetOuterBorder(6, DeepGreen);

	// add the waterfall image
	RiverPane.AddImage(CPoint(0, 0), CPoint(0, 0), IDR_WATER, _T("JPG"));

	// create a display string
	CString str2(_T("Vista Style 2                 StyleEdit"));

	// add the string with a foreground color
	RiverPane.AddString(str2, CPoint(255, 132), Clr(212,Yellow), 16, FONT_REG);

	// add to stack
	m_Style.AddStack(RiverPane);

	// create a display string
	CString str(_T("Panzer\nComputing"));

	GETSCREENRECT(pcr, m_cStringFrame);

	RaisedFrame rf(pcr, Clr(128, Yellow), DeepGreen, 8);

	// add the string with a foreground color
	rf.RFrame.AddAlignString(pcr, str, ALIGN_CENTER, CPoint(0,0), DeepGreen, 26, FONT_BOLD);

	pcr.top++; pcr.left++;
	rf.RFrame.AddAlignString(pcr, str, ALIGN_CENTER, CPoint(0,0), DarkGreen, 26, FONT_BOLD);

	// add to stack
	m_Style.AddStack(rf.RFrame);
}

/////////////////////////////////////////////
/////////// Vista Style 2 Buttons ///////////
/////////////////////////////////////////////
void CStyleToolkitDlg::CreateVistaStyle2Buttons()
{
	GETCONTROLRECT(rc, m_cVistaStyle2Norm);
	VistaButton2 vbs2(rc, Clr(0, RGB(0,192,0)));

	//========= Normal Button ===========//
	Stack Top1;
	Top1.SetRect(rc);
	Top1.AddAlignString("Std", ALIGN_CENTER, CPoint(0,0), White, 16);
	Top1.AddImage(CPoint(8, 8), IDR_DB16, _T("PNG"));
	
	// create the std stack
	VS2Std = vbs2.VBStd + Top1;

	// create the styles
	Style std; std.AddStack(VS2Std);
	Style hot; hot.AddStack(vbs2.VBHot + Top1);

	// add the styles
	m_cVistaStyle2Norm.LoadStdStyle(std);
	m_cVistaStyle2Norm.LoadHotStyle(hot);

	GETCONTROLRECT(rc2, m_cVistaStyle2Hot);
	VistaButton2 vbs2a(rc2, Clr(0, RGB(0,192,0)));

	//========= Hot Button ===========//
	Stack Top1a;
	Top1a.SetRect(rc2);
	Top1a.AddAlignString("Hot", ALIGN_CENTER, CPoint(0,0), White, 16);
	Top1a.AddImage(CPoint(8, 8), IDR_DB16, _T("PNG"));
	
	// create the hot stack
	VS2Hot = vbs2a.VBHot + Top1a;

	// switch hot and std for demo
	Style std1a; std1a.AddStack(VS2Hot);
	Style hot1a; hot1a.AddStack(vbs2a.VBStd + Top1a);
	
	// load styles
	m_cVistaStyle2Hot.LoadStdStyle(std1a);
	m_cVistaStyle2Hot.LoadHotStyle(hot1a);

}


//////////////////////////////////////////////////////////////////////////////////
///////// Create Edit Boxes //////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
void CStyleToolkitDlg::CreateEditBoxes()
{
	///////////////////////////////////////
	///////// EditBox1 //// ///////////////
	///////////////////////////////////////
	GETCONTROLRECT(erect1, m_cEdit1);

	// create the stack
	Stack Edit1Stack(erect1);
	Edit1Stack.SetOuterBorder(1, DeepGreen);
	Edit1Stack.SetInnerBorder(1, White);

	// set as gradient
	//Edit1Stack.FillSolid(Clr(0,DeepGreen));

	Edit1Stack.FillGrad2(HORIZ, Clr(200,DeepGreen), Clr(0,Black), -0.2f);

	// apply stack to edit box
	m_cEdit1.SetStack(Edit1Stack);

	// set edit box text color
	m_cEdit1.SetTextClr(Yellow);

	m_cEdit1.SetWindowTextA("Edit Box 1");

	///////////////////////////////////////
	///////// EditBox2 //// ///////////////
	///////////////////////////////////////
	GETCONTROLRECT(erect2, m_cEdit2); 

	// the rect will be set by StyleEdit
	Stack Edit2Stack(erect2);
	Edit2Stack.SetOuterBorder(1, DeepGreen);
	Edit2Stack.SetInnerBorder(1, White);

	// set as gradient
	Edit2Stack.FillGrad2(VERT, CLEAR, Clr(200, DeepGreen),0.4f);

	// apply stack to edit box
	m_cEdit2.SetStack(Edit2Stack);

	// set edit box text color
	m_cEdit2.SetTextClr(Yellow);

	m_cEdit2.SetWindowTextA("Edit Box 2");

}

// Transparent controls need to get the correct background.
// By setting them after the background is are created, the 
// controls will have the correct data regardless of what may
// be on the screen.  This function also prepaints the controls
void CStyleToolkitDlg::SetBackgrounds(CDC* pDC, CRect rect)
{

	m_cVistaStyle2Norm.SetBkGnd(pDC);
	m_cVistaStyle2Hot.SetBkGnd(pDC);

	// prepaint these
	Style TempStyle;

	GETSCREENRECT(trc1, m_cVistaStyle2Norm);
	VS2Std.SetPosition(trc1.left, trc1.top);
	TempStyle.AddStack(VS2Std);

	GETSCREENRECT(trc2, m_cVistaStyle2Hot);
	VS2Hot.SetPosition(trc2.left, trc2.top);
	TempStyle.AddStack(VS2Hot);

	TempStyle.PaintStyle(pDC, rect);

	m_bBackgroundsSet = TRUE;
}


void CStyleToolkitDlg::CreateFrame()
{
	CRect drect; GetFrameRect(drect);
	CRect crect; GetClientRect(crect);

	// the RaisedDlgFrame looks best with an odd sized border
	int width = 13;

	// the resource editor is at a lower dpi so set up things manually
	crect.top++; crect.left = drect.left + width;
	crect.right = drect.right - width;	crect.bottom = drect.bottom - width;
	SetClientRect(crect);

	COLORREF hlt = RGB(100, 240, 147); 
	COLORREF base = RGB(27, 80, 33);
	COLORREF bkgnd = RGB(6, 136, 71);
	COLORREF text = RGB(10, 90, 20);

	CRect trect; GetTitleRect(trect);

	Stack Title(trect);
	Title.FillSolid(bkgnd);

	CString str(_T("Style Toolkit"));
	Title.AddEffectString(str, CPoint(16, 11), OUTLINE, 28, FONT_BOLD, L"Old English Text MT",
								text, Clr(140,hlt), 1);


	RaisedDlgFrame RDF(drect, crect,  base, hlt, width);

	Style BackStyle;
	BackStyle.AddStack(Title);
	BackStyle.AddStack(RDF.DlgFrame);

	AddBackGroundStyle(BackStyle);
}
