#include "stdafx.h"
#include "resource.h"
#include "GraphCtrl.h"
#include "2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMy2App theApp;

MECGraphCtrl::MECGraphCtrl()
{
	ZeroMemory(point,70);
	m_DrawPoint  = 0;
	m_dPreviousPosition =   0.0 ;	
	m_nLastIndexPoint = 0;
	m_nYDecimals = 0 ;
	m_nXDecimals = 0 ;

    m_dLowerLimit = 0 ;
	m_dUpperLimit =  100 ;
	m_dRange      =  m_dUpperLimit - m_dLowerLimit ;   

	m_dXLower = 0 ;
	m_dXUpper =  100 ;
	m_XRange      =  m_dXUpper - m_dXLower;   

// m_nShiftPixels determines how much the plot shifts (in terms of pixels) 
  // with the addition of a new data point
	//n=0;
	m_nShiftPixels     = -4 ;
	m_nHalfShiftPixels = m_nShiftPixels/2 ;                     // protected
	m_nPlotShiftPixels = m_nShiftPixels + m_nHalfShiftPixels ;  // protected

	m_crBackColor  = RGB(  0,   0,   0) ;  
	m_crGridColor  = RGB(  0, 255, 255) ;  
	m_crPlotColor  = RGB(255, 255, 255) ;  


	m_penPlot.CreatePen(PS_SOLID, 0, m_crPlotColor) ;
	m_brushBack.CreateSolidBrush(m_crBackColor) ;

//	m_strXUnitsString.Format(_T("Samples")) ;  
    m_strXUnitsString.Format(_T("Samples")) ;  // can also be set with SetXUnits
    m_strYUnitsString.Format(_T("Y units")) ;  // can also be set with SetYUnits

	m_pbitmapOldGrid = NULL ;
	m_pbitmapOldPlot = NULL ;
}

MECGraphCtrl::~MECGraphCtrl()
{

	if (m_pbitmapOldGrid != NULL)
		m_dcGrid.SelectObject(m_pbitmapOldGrid) ;  
	if (m_pbitmapOldPlot != NULL)
		m_dcPlot.SelectObject(m_pbitmapOldPlot) ;  
	
}


BEGIN_MESSAGE_MAP(MECGraphCtrl, CWnd)
	//{{AFX_MSG_MAP(MECGraphCtrl)
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
float MECGraphCtrl::AppendPoint(float dNewPoint)
{
	if (theApp.ReGraph)
	{
		m_DrawPoint = 0;
		theApp.ReGraph = false;
	}
		//InvalidateCtrl();
  // append a data point to the plot
  // return the previous point
	int i;
	int m_Numpoint = (int)((m_rectPlot.right - m_rectPlot.left)/m_nShiftPixels);
	//printf("%d",m_Numpoint);
	if (m_DrawPoint<70)
	{
		point[m_DrawPoint] = dNewPoint;
	}
	else if(m_DrawPoint == 70)
	{
		for(i = 0;i<(70-1);i++)
		{
			point[i] = point[i+1];
			//printf("point[%d] = %x",i,point[i]);
		}
		point [(70-1)] = dNewPoint;
		m_DrawPoint = (70-1);
	}

	float max,min;
	max = (float)m_fVerticalMax;
	min =  (float)m_fVerticalMin;
	//printf("m_DrawPoint = %d\n",m_DrawPoint);
	//printf("max = %d\n",(int)max);
	//printf("min = %d\n",(int)min);
	if(m_DrawPoint>0)
	{
		GetMaxMin<float>(point,m_DrawPoint,max,min);


		if (max == min)
		{
			SetRange(-2 * abs((int)min)  - 1,2 * abs((int)max) + 1,0);
			m_fVerticalMax = 2 * abs((int)max) + 1;
			m_fVerticalMin = -2 * abs((int)min)  - 1;
		}
		else
		{
			float max_min;
			max_min = (max - min) * 0.1f;
			SetRange(min - max_min, max + max_min, 0) ;
			m_fVerticalMax = max + max_min;
			m_fVerticalMin = min - max_min;

		}
	}
	m_DrawPoint++;

	//DrawPoint() ;

	Invalidate() ;

  //return dPrevious ;
  return 0;

} // AppendPoint

void MECGraphCtrl::DrawPoint()
{ 
  //int currX, prevX, currY, prevY ;
  int i ;
  CPen *oldPen ;
  CRect rectCleanUp ;

  if (m_dcPlot.GetSafeHdc() != NULL)
  {

    // establish a rectangle over the right side of plot
    // which now needs to be cleaned up proir to adding the new point
    rectCleanUp = m_rectPlot ;
    rectCleanUp.left  = rectCleanUp.right - m_nShiftPixels ;

    // fill the cleanup area with the background
    m_dcPlot.FillRect(rectCleanUp, &m_brushBack) ;

    // draw the next line segement

    // grab the plotting pen
    oldPen = m_dcPlot.SelectObject(&m_penPlot) ;
	m_dcPlot.MoveTo (m_rectPlot.left, m_rectPlot.bottom -(long)((point[0] - m_dLowerLimit) * m_dVerticalFactor)) ;
	for (i = 0;i<m_DrawPoint-1;i++)
	{

		m_dcPlot.LineTo((m_rectPlot.left - (i+1)*m_nShiftPixels), m_rectPlot.bottom -(long)((point[i+1] - m_dLowerLimit) * m_dVerticalFactor));
	}


    m_dcPlot.SelectObject(oldPen) ;
	//Invalidate() ;

  }


}
void MECGraphCtrl::InvalidateCtrl()
{

	int nCharacters ;
	int i ;
	int nTopGridPix, nMidGridPix, nBottomGridPix ;


	CPen *oldPen ;
	CPen solidPen(PS_SOLID, 0, m_crGridColor) ;
	CFont axisFont, yUnitFont, *oldFont ;
	CString strTemp ;

	CClientDC dc(this) ;  

	if (m_dcGrid.GetSafeHdc() == NULL)
	{
		m_dcGrid.CreateCompatibleDC(&dc) ;
		m_bitmapGrid.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
		m_pbitmapOldGrid = m_dcGrid.SelectObject(&m_bitmapGrid) ;
	}
  
	m_dcGrid.SetBkColor (m_crBackColor) ;
	m_dcGrid.FillRect(m_rectClient, &m_brushBack) ;
	nCharacters = abs((int)log10(fabs(m_dUpperLimit))) ;
	nCharacters = max(nCharacters, abs((int)log10(fabs(m_dLowerLimit)))) ;

	nCharacters = nCharacters + 4 + m_nYDecimals ;  
	 //m_rectPlot.left = m_rectClient.left + 6*(nCharacters) ;
	m_rectPlot.left = m_rectClient.left + 40;
	m_nPlotWidth    = m_rectPlot.Width() ;

	oldPen = m_dcGrid.SelectObject (&solidPen) ; 
	m_dcGrid.MoveTo (m_rectPlot.left, m_rectPlot.top) ;
	m_dcGrid.LineTo (m_rectPlot.right+1, m_rectPlot.top) ;
	m_dcGrid.LineTo (m_rectPlot.right+1, m_rectPlot.bottom+1) ; 
	m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.bottom+1) ;
	m_dcGrid.LineTo (m_rectPlot.left, m_rectPlot.top) ;
	m_dcGrid.SelectObject (oldPen) ;
	  
	  
	  // draw the dotted lines, 
	  // use SetPixel instead of a dotted pen - this allows for a 
	  // finer dotted line and a more "technical" look
	nMidGridPix    = (m_rectPlot.top + m_rectPlot.bottom)/2 ;
	nTopGridPix    = nMidGridPix - m_nPlotHeight/4 ;
	nBottomGridPix = nMidGridPix + m_nPlotHeight/4 ;

	for (i=m_rectPlot.left; i<m_rectPlot.right; i+=4)
	{
		m_dcGrid.SetPixel (i, nTopGridPix,    m_crGridColor) ;
		m_dcGrid.SetPixel (i, nMidGridPix,    m_crGridColor) ;
		m_dcGrid.SetPixel (i, nBottomGridPix, m_crGridColor) ;
	}

	yUnitFont.CreateFont (14, 0, 900, 0, 300,
						   FALSE, FALSE, 0, ANSI_CHARSET,
						   OUT_DEFAULT_PRECIS, 
						   CLIP_DEFAULT_PRECIS,
						   DEFAULT_QUALITY, 
						   DEFAULT_PITCH|FF_SWISS, _T("Arial")) ;
	axisFont.CreateFont (14, 0, 0, 0, 300,
						   FALSE, FALSE, 0, ANSI_CHARSET,
						   OUT_DEFAULT_PRECIS, 
						   CLIP_DEFAULT_PRECIS,
						   DEFAULT_QUALITY, 
						   DEFAULT_PITCH|FF_SWISS, _T("Arial")) ;
	  
	oldFont = m_dcGrid.SelectObject(&axisFont) ;

			// grab the horizontal font
	oldFont = m_dcGrid.SelectObject(&axisFont) ;
	  
	 // // y max
	  m_dcGrid.SetTextColor (m_crGridColor) ;
	 // strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dUpperLimit) ;
	 // int nYMaxLength = strTemp.GetLength();
	 // if(m_nYDecimals == 2)
		//m_dcGrid.ExtTextOut(m_rectPlot.left-38,m_rectPlot.top,ETO_CLIPPED|ETO_OPAQUE,NULL,strTemp,nYMaxLength,NULL);

	 // // y min
	 // strTemp.Format (_T("%.*lf"), m_nYDecimals, m_dLowerLimit) ;
	 // nYMaxLength = strTemp.GetLength();
	 // if(m_nYDecimals == 2)
		//m_dcGrid.ExtTextOut(m_rectPlot.left-30, m_rectPlot.bottom -10,ETO_CLIPPED|ETO_OPAQUE,NULL,strTemp,nYMaxLength,NULL);

	strTemp.Format (_T("%.*lf"), m_nXDecimals, m_dXLower) ;
	int  nXMaxLength = strTemp.GetLength();
	m_dcGrid.ExtTextOut(m_rectPlot.left,m_rectPlot.bottom+4,ETO_CLIPPED,NULL,strTemp,nXMaxLength,NULL);

	  // x max
	strTemp.Format (_T("%.*lf"), m_nXDecimals, m_dXUpper) ;
	nXMaxLength = strTemp.GetLength();
	m_dcGrid.ExtTextOut(m_rectPlot.right-10,m_rectPlot.bottom+4,ETO_CLIPPED,NULL,strTemp,nXMaxLength,NULL);

	  // x units
	  
	m_dcGrid.ExtTextOut (((m_rectPlot.left+m_rectPlot.right)/2)-3*m_strXUnitsString.GetLength(), 
						m_rectPlot.bottom+5, ETO_CLIPPED,NULL,m_strXUnitsString,m_strXUnitsString.GetLength() ,NULL) ;

	  // restore the font
	m_dcGrid.SelectObject(oldFont) ;

	  // y units
	  oldFont = m_dcGrid.SelectObject(&yUnitFont) ;
	  m_dcGrid.ExtTextOut (((m_rectClient.left+m_rectPlot.left)/2), 
	                    ((m_rectPlot.bottom+m_rectPlot.top)/2)+3*m_strYUnitsString.GetLength(), ETO_CLIPPED,NULL,m_strYUnitsString,m_strYUnitsString.GetLength() ,NULL) ;
	m_dcGrid.SelectObject(oldFont) ;

	if (m_dcPlot.GetSafeHdc() == NULL)
	{
		m_dcPlot.CreateCompatibleDC(&dc) ;
		m_bitmapPlot.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
		m_pbitmapOldPlot = m_dcPlot.SelectObject(&m_bitmapPlot) ;
	}

	m_dcPlot.SetBkColor (m_crBackColor) ;
	m_dcPlot.FillRect(m_rectClient, &m_brushBack) ;

	InvalidateRect(m_rectClient) ;

}
BOOL MECGraphCtrl::Create(DWORD dwStyle, const RECT& rect, 
                         CWnd* pParentWnd, UINT nID) 
{
  BOOL result ;
  static CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW) ;
  //result = CWnd::CreateEx(WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 
  result = CWnd::CreateEx(NULL ,
                          className, NULL, dwStyle | WS_CHILD, 
                          rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
                          pParentWnd->GetSafeHwnd(), (HMENU)nID) ;
  if (result != 0)
    InvalidateCtrl() ;
  return result ;
} // Create
void MECGraphCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	GetClientRect(m_rectClient) ;

	m_nClientHeight = m_rectClient.Height() ;
	m_nClientWidth  = m_rectClient.Width() ;

	m_rectPlot.left   = 40 ;  
	m_rectPlot.top    = 10 ;
	m_rectPlot.right  = m_rectClient.right -10;
	m_rectPlot.bottom = m_rectClient.bottom-25 ;

	m_nPlotHeight = m_rectPlot.Height() ;
	m_nPlotWidth  = m_rectPlot.Width() ;

//	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange ; 

}

void MECGraphCtrl::OnPaint() 
{
	CPaintDC dc(this); 
	

	CDC memDC ;
	CBitmap memBitmap ;
	CBitmap* oldBitmap ; 
	int i ;
	CPen *oldPen ; 
	CString strTemp;
	memDC.CreateCompatibleDC(&dc) ;
	memBitmap.CreateCompatibleBitmap(&dc, m_nClientWidth, m_nClientHeight) ;
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap) ;

	if (memDC.GetSafeHdc() != NULL)
	{
	
		memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			         &m_dcGrid, 0, 0, SRCCOPY) ;
		// now add the plot on top as a "pattern" via SRCPAINT.
		// works well with dark background and a light plot
		//memDC.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
		//	         &m_dcPlot, 0, 0, SRCPAINT) ;  //SRCPAINT
		memDC.SetBkColor (m_crBackColor) ;
		// y max
		memDC.SetTextColor (m_crGridColor) ;
		strTemp.Format (_T("%7.1f"), m_dUpperLimit) ;
		int nYMaxLength = strTemp.GetLength();

		memDC.ExtTextOut(m_rectPlot.left-44,m_rectPlot.top,ETO_CLIPPED|ETO_OPAQUE,NULL,strTemp,nYMaxLength,NULL);

		// y min
		strTemp.Format (_T("%7.1f"), m_dLowerLimit) ;
		nYMaxLength = strTemp.GetLength();

		memDC.ExtTextOut(m_rectPlot.left-44, m_rectPlot.bottom -10,ETO_CLIPPED|ETO_OPAQUE,NULL,strTemp,nYMaxLength,NULL);
		oldPen = memDC.SelectObject(&m_penPlot) ;
		memDC.MoveTo (m_rectPlot.left, m_rectPlot.bottom -(long)((point[0] - m_dLowerLimit) * m_dVerticalFactor)) ;
		for (i = 0;i<m_DrawPoint-1;i++)
		{

			memDC.LineTo((m_rectPlot.left - (i+1)*m_nShiftPixels), m_rectPlot.bottom -(long)((point[i+1] - m_dLowerLimit) * m_dVerticalFactor));
		}


		memDC.SelectObject(oldPen) ;
		// finally send the result to the display
		dc.BitBlt(0, 0, m_nClientWidth, m_nClientHeight, 
			      &memDC, 0, 0, SRCCOPY) ;
	}

	memDC.SelectObject(oldBitmap) ;
	// Do not call CWnd::OnPaint() for painting messages
}
void MECGraphCtrl::SetRange(double dLower, double dUpper, int nDecimalPlaces)
{
	CString strTemp ;
    ASSERT(dUpper > dLower) ;

	m_dOldLowerLimit = m_dLowerLimit;
	m_dOldUpperLimit = m_dUpperLimit;

    m_dLowerLimit     = dLower ;
	m_dUpperLimit     = dUpper ;
	m_nYDecimals      = nDecimalPlaces ;
	m_dRange          = m_dUpperLimit - m_dLowerLimit ;
	m_dVerticalFactor = (double)m_nPlotHeight / m_dRange; 



}
void MECGraphCtrl::SetXUnits(CString string)
{
  m_strXUnitsString = string ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetXUnits

void MECGraphCtrl::SetYUnits(CString string)
{
  m_strYUnitsString = string ;
  // clear out the existing garbage, re-start with a clean plot
   InvalidateCtrl() ;
  //m_dcGrid.ExtTextOut (((m_rectClient.left+m_rectPlot.left)/2)-10, 
   //                 ((m_rectPlot.bottom+m_rectPlot.top)/2), ETO_CLIPPED,NULL,
	//				m_strYUnitsString,m_strYUnitsString.GetLength() ,NULL) ;
}
void MECGraphCtrl::SetGridColor(COLORREF color)
{
  m_crGridColor = color ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetGridColor
void MECGraphCtrl::SetPlotColor(COLORREF color)
{
  m_crPlotColor = color ;
  m_penPlot.DeleteObject() ;
  m_penPlot.CreatePen(PS_SOLID, 2, m_crPlotColor) ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetPlotColor
void MECGraphCtrl::SetBackgroundColor(COLORREF color)
{
  m_crBackColor = color ;
  m_brushBack.DeleteObject() ;
  m_brushBack.CreateSolidBrush(m_crBackColor) ;
  // clear out the existing garbage, re-start with a clean plot
  InvalidateCtrl() ;
}  // SetBackgroundColor
void MECGraphCtrl::SetXRange(double dLower, double dUpper, int nDecimalPlaces)
{
	ASSERT(dUpper > dLower) ;
	m_dXLower     = dLower ;
	m_dXUpper     = dUpper ;
	m_nXDecimals  = nDecimalPlaces ;
	m_XRange          = m_dXUpper - m_dXLower ;
	m_dHorizontalFactor = (double)m_nPlotWidth / m_XRange ; 
    // clear out the existing garbage, re-start with a clean plot
	InvalidateCtrl() ;
}
/////////////////////////////////////////////////////////////////////////////

void MECGraphCtrl::ClearPloted()
{
	m_DrawPoint = 0;

	Invalidate() ;
}

void MECGraphCtrl::Reset()
{
  // to clear the existing data (in the form of a bitmap)
  // simply invalidate the entire control
  InvalidateCtrl() ;
}

CString MECGraphCtrl::GetXUnits()
{
  return m_strXUnitsString; 
}  // GetXUnits

CString MECGraphCtrl::GetYUnits()
{
  return m_strYUnitsString;
}// GetYUnits
//BOOL MECGraphCtrl::OnEraseBkgnd(CDC* pDC)
//{
//	return TRUE;
//}


BOOL MECGraphCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;
}
