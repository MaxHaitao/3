#if !defined(AFX_GRAPHCTRL_H__49A61F3B_12CB_4BCB_9D87_2B23CF23958C__INCLUDED_)
#define AFX_GRAPHCTRL_H__49A61F3B_12CB_4BCB_9D87_2B23CF23958C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/////////////////////////////////////////////////////////////////////////////
// MECGraphCtrl window
class MECPoint: public CObject
{
public:
	double x;
	double y;
};
class MECGraphCtrl : public CWnd
{
// Construction
public:
	MECGraphCtrl();

// Attributes
public:
		MECPoint m_oPoints[MAX_PATH], m_oNewPoint,m_dPoints[MAX_PATH];
		int m_nLastIndexPoint;
		int m_DrawPoint;
		float point[70];
		COLORREF m_crBackColor;        // background color
		COLORREF m_crGridColor;        // grid color
		COLORREF m_crPlotColor;        // data color  

		double m_dCurrentPosition;   // current position
        double m_dPreviousPosition;  // previous position
		int n;


		int m_nClientHeight;
        int m_nClientWidth;
	    int m_nPlotHeight;
	    int m_nPlotWidth;

		int m_nHalfShiftPixels;
        int m_nPlotShiftPixels;
		double m_fVerticalMax;
		double m_fVerticalMin;
		double m_dLowerLimit, m_dXLower;        // lower bounds
		double m_dUpperLimit, m_dXUpper;        // upper bounds
		double m_dOldLowerLimit,m_dOldUpperLimit;	//YÖá¾ÉµÄ·¶Î§
		double m_dRange, m_XRange;
		double m_dVerticalFactor, m_dHorizontalFactor;
  
		CRect  m_rectClient;
		CRect  m_rectPlot;
		CPen   m_penPlot;
		CBrush m_brushBack;

		CDC     m_dcGrid;
		CDC     m_dcPlot;
		CBitmap *m_pbitmapOldGrid;
		CBitmap *m_pbitmapOldPlot;
		CBitmap m_bitmapGrid;
		CBitmap m_bitmapPlot;

		int m_nShiftPixels;
		int m_nYDecimals, m_nXDecimals;
	
		CString m_strXUnitsString;
		CString m_strYUnitsString;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MECGraphCtrl)
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID=NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetXRange(double dLower, double dUpper, int nDecimalPlaces);
	void InvalidateCtrl();
	void DrawPoint();
	void ClearPloted();
	void Reset();
	//void AppendPoint(double dPointX, double dPointY);
	float AppendPoint(float dNewPoint);
	void SetRange(double dLower, double dUpper, int nDecimalPlaces);
	void SetXUnits(CString string);
	void SetYUnits(CString string);
	void SetGridColor(COLORREF color);
	void SetPlotColor(COLORREF color);
	void SetBackgroundColor(COLORREF color);
	CString GetYUnits(void);
	CString GetXUnits(void);
	virtual ~MECGraphCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(MECGraphCtrl)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHCTRL_H__49A61F3B_12CB_4BCB_9D87_2B23CF23958C__INCLUDED_)
