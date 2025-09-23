#pragma once
// WaferMap.h : header file
//
#include "Coor.h"

#define FILL_LEVEL	(26)

// class CCoor;			HAQUE/ADDED/WAFER MAP
class CRecipeRAFT;	

/////////////////////////////////////////////////////////////////////////////
// CWaferMap window
struct ColorMapPoint {
	float fX;
	float fY;
	float fT;
};

class CWaferMap : public CStatic {
	// Construction
public:
	CRecipeRAFT* pRcp;	

	CWaferMap();

	// Attributes
public:
	enum DSPMODE {
		dmUSER, dmENGINNER
	};
	DSPMODE dmMode;   // 0 user mode, 1 engineering mode [7/21/2010 C2C]
	short MeSet;
	float m_fLog2Device;
	BOOL m_bFitScreen;
	int m_nWaferSize;
	BOOL bRedraw;
	int m_nImageType;

	BOOL m_bMask;
	BOOL bDrawBackground;
	BOOL bDrawPoint;
	BOOL bDrawPatt;
	BOOL bDrawText;
	BOOL bDrawValues;
	BOOL bSiteView;
	BOOL bLandscape;
	BOOL bShowStat;

	// 	int nLevelAdjustment;

	short MapCol;
	short DataCol;	// DataCol 999 is for baseline [10/4/2012 Yuen]
	CString Title;

	CWnd* pParent;

	CPoint cpt;
	CBitmap m_bmpWaferMap;
	CStringList strList;

	COLORREF m_crFill[FILL_LEVEL];

	// Operations
public:
	CCoor& Dev2Log(CPoint point);	
	CPoint Log2Dev(CCoor* point);	
	void DrawOtherText(CDC* pDC, LPCRECT lpRect);
	void DrawPoint(CDC* pDC, CRect& rect);

	void DrawPattern(CDC* pDC, CRect& rect);
	BOOL DrawColorMap(CDC* pDC, LPCRECT lpRect);
	//	BOOL DrawColorMap2(CDC *pDC, LPCRECT lpRect);
	BOOL DrawProfiles(CDC* pDC, LPCRECT lpRect);

	void DrawStage(CDC* pDC, CRect& rect);
	void DrawWafer(CDC* pDC, CRect& rc);
	void DrawBackground(CDC* pDC, CRect& rect);
	void DrawHorizontalRuler(CDC* pDC, CRect& rc);
	void DrawVerticalRuler(CDC* pDC, CRect& rc);
	void DrawMain(CDC* pDC, CRect& rc);
	void DrawValues(CDC* pDC, CRect&);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CWaferMap)
		//}}AFX_VIRTUAL

	// Implementation
public:
	void GenerateZPosData();
	void UpdatePoint(CPoint& point);
	void RelocateToTop(CPoint& point);
	void RelocateToBack(CPoint& point);
	void AddPointAlgn(CPoint& point);
	void AddPoint(CPoint& point);
	void DeletePoint(CPoint& point);
	void EditPoint(CPoint& point);
	void GotoPoint(CPoint& point);
	void Draw(CDC& dc, CRect rc, BOOL bDoNotShow = FALSE);
	BOOL SaveBitmap(CFile& fp, CRect& rc);
	void Redraw();
	void OnDraw(CDC& dc);
	virtual ~CWaferMap();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWaferMap)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
