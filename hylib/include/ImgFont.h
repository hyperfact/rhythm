// ImgFont.h: interface for the CImgFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGFONT_H__AE29850B_3203_40C0_927E_31417EF208ED__INCLUDED_)
#define AFX_IMGFONT_H__AE29850B_3203_40C0_927E_31417EF208ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "_prolog.h"
#include <exception>
#include "util.h"
#include <Gdiplus.h>
#include <map>

_HYLIB_BEGIN

#ifdef _USING_MFC

class CNoFontException : public std::exception
{
public:
	CNoFontException() throw()
		: exception("No font exception") {}
};

class CImgFontBase
{
public:
	virtual ~CImgFontBase() {}

	// 在指定矩形之中绘制指定字符串
	virtual void Draw(HDC hdc, const CString &str, const CRect &rc,
		UINT nAlignFlag=LOS_CENTER) throw(CNoFontException) = 0;
	// 在指定点绘制字符串
	virtual void Draw(HDC hdc, const CString &str, int x, int y,
		UINT nAnchor=LOS_LEFT|LOS_TOP) throw(CNoFontException) = 0;

protected:
	virtual CRect Chr2Rect(TCHAR cChar) const throw(CNoFontException) = 0;
	virtual int StrWidth(const CString &str) const = 0;
	virtual int StrHeight(const CString &str) const = 0;
};

class HYLIB_DECL CImgFontNormal : public CImgFontBase
{
//******************************************************************************
// Types:
public:
	typedef Gdiplus::Image Image;
	typedef int (*FuncChr2Index)(TCHAR);

//******************************************************************************
// Ctor & Dtor:
public:
	CImgFontNormal();
	virtual ~CImgFontNormal();

//******************************************************************************
// Initialization methods:
public:
	void Init(const CString &sFontSet, Image *pImg, int nRows, int nCols,
		const Gdiplus::ImageAttributes *imgAttr=NULL);
	void Init(FuncChr2Index pfnChr2Index, Image *pImg, int nRows, int nCols,
		const Gdiplus::ImageAttributes *imgAttr=NULL);

//******************************************************************************
// Override class CImgFontBase:
public:
	/*_override_*/ void Draw( HDC hdc, const CString &str, const CRect &rc,
		UINT nAlignFlag=LOS_CENTER ) throw(CNoFontException);
	
	/*_override_*/ void Draw( HDC hdc, const CString &str, int x, int y,
		UINT nAnchor=LOS_LEFT|LOS_TOP ) throw(CNoFontException);
	
protected:
	/*_override_*/ CRect Chr2Rect( TCHAR cChar ) const throw(CNoFontException);
	
	/*_override_*/ int StrWidth( const CString &str ) const;
	
	/*_override_*/ int StrHeight( const CString &str ) const;

//******************************************************************************
// Member variables:
protected:
	Image		*m_imgFontSet;
	Gdiplus::ImageAttributes *m_pImgAttr;

	typedef std::map<TCHAR, int> FontMap;
	FontMap		m_mapFont;

	FuncChr2Index m_pfnChr2Index;

	int			m_nRows;
	int			m_nCols;
};

#endif // _USING_MFC

_HYLIB_END

#endif // !defined(AFX_IMGFONT_H__AE29850B_3203_40C0_927E_31417EF208ED__INCLUDED_)
