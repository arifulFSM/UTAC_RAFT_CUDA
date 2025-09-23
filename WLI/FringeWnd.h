#pragma once

#include <vector>

#include "PSI/Roi.h"

struct SVHLine {
	float ix = 0;
	COLORREF cr = 0;
	SVHLine(float x, COLORREF c) : ix(x), cr(c) {}
};

//07182023 - START

struct SFPoint {
	double x;
	double y;
};

//07182023 - END

struct SPlot {
	std::string title = "", xlabel = "", ylabel = "";
	IMGL::SLines line1, line2, line3, line4, line5; // 5 sets of independent line groups
	IMGL::SLines& get(int n);

	bool Export(const TCHAR* fname);
	bool Export(const TCHAR* fname, int n);
	IMGL::SLines& Get(int n);
	void clear();
	void dump(char* fname);
	void label(std::string ttle, std::string xlbel = "", std::string ylbel = "");
	void maxmin();

	SPlot();
};

class CFringeWnd : public CWnd {
	DECLARE_DYNAMIC(CFringeWnd)

		bool bHZero = true, bVCenter = true;
	std::vector<SVHLine> hLin, vLin;
	SPlot dat; // data container [3/25/2022 yuenl]
	LONG prevRC;	//07182023

	void DrawHVLines(CDC& dc);
	void DrawMarker(CDC& dc, int x, int y, CPen* pPen1, CPen* pPen2, int nMkr);

public:
	enum FONTSIZ { SMALL = 16, MIDSIZ = 21, LARGE = 26 };
	enum LINETYPE { POINT, LINE, BAR };

public:
	void AddHLine(SVHLine V);
	void AddVLine(SVHLine V);
	void Clear();
	void ClearHLine();
	void ClearVLine();
	void Export(const TCHAR* fname);
	void Label(std::string title, std::string xlbel = "", std::string ylbel = "");
	void Plot(IMGL::Slin& lin, short nChart);
	void Refresh();
	void SetFontSize(CFringeWnd::FONTSIZ sz);

protected:
	SROI R;
	CBitmap BMP;
	BOOL bAutoScale = FALSE;
public:
	CRect rcPlot, rcChart, rcAll;

protected:
	virtual void OnDraw(CDC* pDC);

public:
	std::vector<SFPoint>StepHeightPts; //07182023
	bool bFringe = true;
	FONTSIZ FonSz = FONTSIZ::LARGE;

	CString fmtX = _T("%.4f");
	CString fmtY = _T("%.0f");

	struct SCPar { // Chart parameters [4/20/2021 FSM]
		int sz = 0;
		float sx = 0, sy = 0, sxx = 0, syy = 0;
		float Xmin = 0, Xmax = 0;
		float Ymin = 0, Ymax = 0;
		float Dy = 0; // Y range [5/6/2021 FSM]
		float ZDy = 0; // Zoom window Y range [5/6/2021 FSM]
		float WvDy = 0, RDy; // Waviness and Roughness Y range [5/6/2021 FSM]
	}CPar;

	CPoint Pt;

public:
	CFringeWnd();
	virtual ~CFringeWnd();

public:
	void Redraw(BOOL bClear);

protected:
	void CalcScaling(IMGL::SLines& lins);
	void DrawMainLines(CDC& dc, IMGL::Slin& lin);
	void DrawBackground(CDC& dc);
	void DrawT(CDC& dc, std::string& title);
	void DrawX(CDC& dc);
	void DrawX1(CDC& dc);
	void DrawY(CDC& dc);
	void DrawY1(CDC& dc);
	void SetupRect();
	void ShowBMP(CDC& dc, CDC* pDC);
	void DrawLabelForHeightMapPlot(CDC& dc); // 05302023 - Mortuja

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

class CFringeWndEx : public CFringeWnd {
protected:
	void DrawROI(CDC& dc, SROI& R);
	virtual void OnDraw(CDC* pDC);

public:
	CFringeWndEx() {}
	~CFringeWndEx() {}

public:
	DECLARE_MESSAGE_MAP()
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
