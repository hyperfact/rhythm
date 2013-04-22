// ImgFont.cpp: implementation of the CImgFont class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ImgFont.h"

#pragma comment(lib, "Gdiplus.lib")

_HYLIB_BEGIN

#ifdef _USING_MFC

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImgFontNormal::CImgFontNormal()
	: m_imgFontSet(NULL)
	, m_nRows(0)
	, m_nCols(0)
	, m_pfnChr2Index(NULL)
	, m_pImgAttr(NULL)
{

}

CImgFontNormal::~CImgFontNormal()
{

}

void CImgFontNormal::Draw( HDC hdc, const CString &str, const CRect &rc, UINT nAlignFlag/*=LOS_CENTER */ ) throw(CNoFontException)
{
	throw std::exception("The method or operation is not implemented.");
}

void CImgFontNormal::Draw( HDC hdc, const CString &str, int x, int y, UINT nAnchor/*=LOS_LEFT|LOS_TOP */ ) throw(CNoFontException)
{
	ASSERT(hdc);

	int i = 0;
	using namespace Gdiplus;
	Graphics g(hdc);
	for (i=0; i<str.GetLength(); ++i) {
		CRect rc(Chr2Rect(str[i]));
		g.DrawImage(m_imgFontSet, x, y, rc.left, rc.top, rc.Width(), rc.Height(), UnitPixel);
	}
}

CRect CImgFontNormal::Chr2Rect( TCHAR cChar ) const throw(CNoFontException)
{
	CRect rc(0, 0, m_imgFontSet->GetWidth()/m_nCols, m_imgFontSet->GetHeight()/m_nRows);
	
	int nIndex = -1;
	if (m_pfnChr2Index) {
		nIndex = (*m_pfnChr2Index)(cChar);
	} else {
		FontMap::const_iterator iFind = m_mapFont.find(cChar);
		nIndex = ((m_mapFont.end()==iFind) ? (-1) : (iFind->second));
	}
	if (-1 == nIndex) { throw CNoFontException(); }
	RectMoveToXY(rc, nIndex%m_nCols*rc.Width(), nIndex/m_nCols*rc.Height());
	return rc;
}

int CImgFontNormal::StrWidth( const CString &str ) const
{
	return str.GetLength() * m_imgFontSet->GetWidth() / m_nCols;
}

int CImgFontNormal::StrHeight( const CString &str ) const
{
	return m_imgFontSet->GetHeight() / m_nRows;
}

void CImgFontNormal::Init( const CString &sFontSet, Image *pImg, int nRows, int nCols,
						  const Gdiplus::ImageAttributes *imgAttr)
{
	ASSERT(pImg != NULL);
	ASSERT(sFontSet.GetLength());
	ASSERT(nRows>0 && nCols>0);
	ASSERT(sFontSet.GetLength() <= nRows*nCols);

	m_imgFontSet = pImg;
	m_nRows = nRows;
	m_nCols = nCols;
	m_pImgAttr = (imgAttr?imgAttr->Clone():NULL);

	int i = 0;
	for (i=0; i<sFontSet.GetLength(); ++i) {
		m_mapFont[sFontSet[i]] = i;
	}
}

void CImgFontNormal::Init( FuncChr2Index pfnChr2Index, Image *pImg, int nRows, int nCols,
						  const Gdiplus::ImageAttributes *imgAttr)
{
	ASSERT(pfnChr2Index != NULL);
	ASSERT(nRows>0 && nCols>0);

	m_pfnChr2Index = pfnChr2Index;
	m_nRows = nRows;
	m_nCols = nCols;
	m_pImgAttr = (imgAttr?imgAttr->Clone():NULL);
}

#endif // _USING_MFC

_HYLIB_END
