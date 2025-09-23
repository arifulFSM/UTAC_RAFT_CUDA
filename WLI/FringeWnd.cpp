#include "pch.h"
#include "resource.h"

#include "IM/IM.h"
#include "Color.h"
#include "MTH/Define.h"
#include "PSI/ICC.h" // 05302023 - Mortuja
#include "PSI/Strip.h" // 05302023 - Mortuja
#include "FringeWnd.h"

IMPLEMENT_DYNAMIC(CFringeWnd, CStatic)

CFringeWnd::CFringeWnd() {}

CFringeWnd::~CFringeWnd() {}

void CFringeWnd::Redraw(BOOL bClear) {
	if (BMP.m_hObject) BMP.DeleteObject();
	Invalidate(bClear);
}

void CFringeWnd::DrawHVLines(CDC& dc) {
	int sz = int(vLin.size());
	int sdc = dc.SaveDC();
	dc.SetTextAlign(TA_CENTER | TA_BOTTOM);
	for (int i = 0; i < sz; i++) {
		if ((vLin[i].ix < CPar.Xmin) || (vLin[i].ix > CPar.Xmax)) continue;
		CPen* pPen = new CPen(PS_SOLID, 2, vLin[i].cr);
		CPen* pOPen = dc.SelectObject(pPen);
		int x = rcPlot.left + int(CPar.sx * (vLin[i].ix - CPar.Xmin));
		dc.MoveTo(x, rcPlot.top); dc.LineTo(x, rcPlot.bottom);
		dc.SelectObject(pOPen); delete pPen;
		CString str; str.Format(_T("%.4f"), vLin[i].ix);
		dc.TextOutW(x, rcPlot.top, str);
	}
	sz = int(hLin.size());
	dc.SetTextAlign(TA_RIGHT | TA_BASELINE);
	for (int i = 0; i < sz; i++) {
		if ((hLin[i].ix < CPar.Ymin) || (hLin[i].ix > CPar.Ymax)) continue;
		CPen* pPen = new CPen(PS_SOLID, 2, hLin[i].cr);
		CPen* pOPen = dc.SelectObject(pPen);
		int y = rcChart.bottom - int(CPar.sy * (hLin[i].ix - CPar.Ymin) + 8);
		dc.MoveTo(rcPlot.left, y); dc.LineTo(rcPlot.right, y);
		dc.SelectObject(pOPen); delete pPen;
		CString str; str.Format(_T("%.4f"), hLin[i].ix);
		dc.TextOutW(rcPlot.left, y, str);
	}
	dc.RestoreDC(sdc);
}

void CFringeWnd::DrawMarker(CDC& dc, int x, int y, CPen* pPen1, CPen* pPen2, int nMkr) {
	if (nMkr == 0) return;
	int state = dc.SaveDC();
	dc.SelectObject(pPen1);
	switch (nMkr) {
	case 1:// circle
		dc.Ellipse(x - 2, y - 2, x + 2, y + 2);
		break;
	case 2:// circle
		dc.Ellipse(x - 3, y - 3, x + 3, y + 3);
		break;
	case 3:
		dc.MoveTo(x - 2, y - 2); dc.LineTo(x + 2, y + 2);
		dc.MoveTo(x + 2, y - 2); dc.LineTo(x - 2, y + 2);
		break;
	case 4:
		dc.MoveTo(x, y - 3); dc.LineTo(x, y + 3);
		break;
	case 5:
		dc.MoveTo(x - 3, y); dc.LineTo(x + 3, y);
		break;
	case 6:
		dc.Ellipse(x - 3, y - 1, x + 3, y + 1);
		break;
	case 7:
		dc.Ellipse(x - 3, y - 2, x + 3, y + 2);
		break;
	case 8:
		dc.Ellipse(x - 1, y - 3, x + 1, y + 3);
		break;
	case 9:
		dc.Ellipse(x - 2, y - 3, x + 2, y + 3);
		break;
	}
	dc.RestoreDC(state);
}

void CFringeWnd::SetFontSize(CFringeWnd::FONTSIZ sz) {
	FonSz = sz;
}

BEGIN_MESSAGE_MAP(CFringeWnd, CStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CFringeWnd::Clear() {
	vLin.clear();
	hLin.clear();
	dat.clear();
	StepHeightPts.clear();	//07182023
	prevRC = -1;	//07182023
}

void CFringeWnd::Refresh() {
	Invalidate(TRUE);
}

void CFringeWnd::Plot(IMGL::Slin& lin, short nChart) {
	IMGL::SLines& ln = dat.get(nChart);
	ln.lins.push_back(lin);
}

void CFringeWnd::OnDraw(CDC* pDC) {
	CDC dc; if (!dc.CreateCompatibleDC(pDC)) return;
	if (BMP.m_hObject) { ShowBMP(dc, pDC); return; } // done and exit [11/7/2020 FSM]

	SetupRect();

	CBitmap* pOBMP;
	if (!BMP.CreateCompatibleBitmap(pDC, rcAll.Width(), rcAll.Height())) return;
	pOBMP = dc.SelectObject(&BMP);

	DrawBackground(dc);
	CRgn Rg;
	Rg.CreateRectRgn(rcPlot.left + 1, rcPlot.top + 1, rcPlot.right, rcPlot.bottom);
	dc.SelectClipRgn(&Rg);

	//////////////////////////////////////////////////////////////////////////
	// main plot [2/15/2021 FSM]
	IMGL::SLines& lins = dat.line4;
	int sz = int(lins.size());
	/*if (sz > 0) {  // 05302023 - Mortuja
		CalcScaling(lins);
		for (int i = 0; i < sz; i++) DrawMainLines(dc, lins.lins[i]);
	}*/

	lins = dat.line3;
	sz = int(lins.size());
	if (sz > 0) {
		CalcScaling(lins);
		for (int i = 0; i < sz; i++) DrawMainLines(dc, lins.lins[i]);
	}

	lins = dat.line2;
	sz = int(lins.size());
	if (sz > 0) {
		CalcScaling(lins);
		for (int i = 0; i < sz; i++) DrawMainLines(dc, lins.lins[i]);
	}

	lins = dat.line1;
	sz = int(lins.size());
	if (sz > 0) {
		CalcScaling(lins);
		for (int i = 0; i < sz; i++) DrawMainLines(dc, lins.lins[i]);
		CPen* pPen = nullptr, * pOPen = nullptr;
		if (bHZero || bVCenter) {
			pPen = new CPen(PS_DASH, 1, RGB(128, 128, 128));
			pOPen = (CPen*)dc.SelectObject(pPen);
		}
		if (bHZero) {
			int y = rcPlot.bottom - int(CPar.sy * (0 - CPar.Ymin));
			if ((y < rcPlot.bottom) && (y > rcPlot.top)) {
				dc.MoveTo(rcPlot.left, y); dc.LineTo(rcPlot.right, y);
			}
		}
		if (bVCenter) {
			int x; CString str;
			short FontSz;
			switch (FonSz) {
			case CFringeWnd::SMALL: FontSz = 14; break;
			default: FontSz = 18; break;
			case CFringeWnd::LARGE: FontSz = 22; break;
			}
			CFont font, * pfont = NULL;
			font.CreateFont(FontSz, 0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"));
			pfont = dc.SelectObject(&font);
			dc.SetTextColor(RGB(0, 0, 255));
			dc.SetBkMode(TRANSPARENT);
			dc.SetTextAlign(TA_CENTER | TA_TOP);
			float dx = (CPar.Xmax - CPar.Xmin);
			x = rcPlot.left + int(CPar.sx * (dx / 2.f /*- CPar.Xmin*/));
			if ((x < rcPlot.right) && (x > rcPlot.left)) {
				dc.MoveTo(x, rcPlot.bottom); dc.LineTo(x, rcChart.top);
				str.Format(fmtX, dx / 2.f + CPar.Xmin);
				dc.TextOutW(x, rcChart.bottom, str);
			}
			x = rcPlot.left + int(CPar.sx * (dx / 4.f /*- CPar.Xmin*/));
			if ((x < rcPlot.right) && (x > rcPlot.left)) {
				dc.MoveTo(x, rcPlot.bottom); dc.LineTo(x, rcChart.top);
				str.Format(fmtX, dx / 4.f + CPar.Xmin);
				dc.TextOutW(x, rcChart.bottom, str);
			}
			x = rcPlot.left + int(CPar.sx * (3 * dx / 4.f /*- CPar.Xmin*/));
			if ((x < rcPlot.right) && (x > rcPlot.left)) {
				dc.MoveTo(x, rcPlot.bottom); dc.LineTo(x, rcChart.top);
				str.Format(fmtX, 3 * dx / 4.f + CPar.Xmin);
				dc.TextOutW(x, rcChart.bottom, str);
			}
			// 07182023 - START
			pPen = new CPen(PS_SOLID, 1.5, RGB(128, 128, 128));
			(CPen*)dc.SelectObject(pPen);
			if (prevRC == -1) {
				prevRC = abs(rcPlot.right - rcPlot.left);
			}
			LONG presentRC = abs(rcPlot.right - rcPlot.left);
			double scaleRC = prevRC / (presentRC * 1.0);
			//TRACE("%d %d %lf\n", prevRC, presentRC, scaleRC);
			prevRC = presentRC;
			double xf;
			for (int pt = 0; pt < StepHeightPts.size(); pt++) {
				//x = int(CPar.sx * (StepHeightPts[pt].x - rcPlot.left));
				xf = ((StepHeightPts[pt].x - rcPlot.left) / scaleRC) + rcPlot.left;
				StepHeightPts[pt].x = xf;
				if ((xf < rcPlot.right) && (xf > rcPlot.left)) {
					dc.MoveTo(xf, rcPlot.bottom); dc.LineTo(xf, rcChart.top);
				}
			}
			// 07182023 - END
			if (pfont) dc.SelectObject(pfont);
		}
		if (pOPen) {
			dc.SelectObject(pOPen);
			if (pPen) delete pPen;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// Draw CG line [10/25/2020 yuenl]
	if (int(dat.line1.size()) || int(dat.line2.size()) || int(dat.line3.size()) || int(dat.line4.size())) {
		short FontSz;
		switch (FonSz) {
		case CFringeWnd::SMALL: FontSz = 14; break;
		default: FontSz = 18; break;
		case CFringeWnd::LARGE: FontSz = 22; break;
		}
		CFont font, * pfont = NULL;
		font.CreateFont(FontSz, 0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"));
		pfont = dc.SelectObject(&font);
		dc.SetBkMode(TRANSPARENT);

		CString str = _T("");

		DrawHVLines(dc);

		if (bFringe) { DrawT(dc, dat.title); }
		else {
			if (lins.size() > 0) {
				IMGL::Slin& A = lins.lins[0];
				str.Format(_T("%s Pos (%d, %d), Height: %.1f nm"),
					dat.title.c_str(), Pt.x, Pt.y, (A.ymax - A.ymin) * 1e3);
				dc.TextOutW(rcChart.left + rcChart.Width() / 2, 0, str);
			}
		}
		if (bFringe) { DrawY(dc); }
		else { DrawY1(dc); }

		if (bFringe) { DrawX(dc); }
		else { DrawX1(dc); }

		if (pfont) dc.SelectObject(pfont);
	}

	dc.SelectClipRgn(NULL);
	pDC->BitBlt(rcAll.left, rcAll.top, rcAll.Width(), rcAll.Height(), &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOBMP);
}

void CFringeWnd::SetupRect() {
	short legendwd = 128;
	GetClientRect(&rcAll); rcChart = rcAll;
	if (rcAll.Width() < 1200) legendwd = 0;
	switch (FonSz) {
	case FONTSIZ::LARGE:
		InflateRect(&rcChart, -96, -24); rcChart.left += 8; rcChart.right += (92 - legendwd);
		break;
	default:
		InflateRect(&rcChart, -72, -24); rcChart.left += 8; rcChart.right += (60 - legendwd);
		break;
	case FONTSIZ::SMALL:
		InflateRect(&rcChart, -48, -18); rcChart.left += 8; rcChart.right += (38 - legendwd);
		break;
	}
	if (ICC.isHeightMapPlot) rcChart.bottom -= 35;
	rcPlot = rcChart;
	rcPlot.left += 4; rcPlot.top += 24;
	rcPlot.right -= 4; rcPlot.bottom -= 6;
}

void CFringeWnd::ShowBMP(CDC& dc, CDC* pDC) {
	CBitmap* pOBMP = dc.SelectObject(&BMP);
	pDC->BitBlt(rcAll.left, rcAll.top, rcAll.Width(), rcAll.Height(), &dc, 0, 0, SRCCOPY);
	dc.SelectObject(pOBMP);
}

void CFringeWnd::DrawBackground(CDC& dc) {
	dc.FillSolidRect(rcAll.left, rcAll.top, rcAll.Width(), rcAll.Height(), RGB(210, 210, 210));

	dc.FillSolidRect(rcChart.left, rcChart.top, rcChart.Width(), rcChart.Height(), RGB(231, 230, 230));
}

void CFringeWnd::CalcScaling(IMGL::SLines& lins) {
	int wd = rcPlot.Width(), ht = rcPlot.Height();
	lins.maxmin();
	CPar.Xmin = lins.xmin, CPar.Xmax = lins.xmax;
	CPar.Ymin = lins.ymin, CPar.Ymax = lins.ymax;
	CPar.sx = wd / (CPar.Xmax - CPar.Xmin);
	CPar.sxx = lins.sz / float(wd);
	CPar.sy = ht / (CPar.Ymax - CPar.Ymin);
	CPar.sz = lins.sz;
}

void CFringeWnd::ClearVLine() {
	vLin.clear();
}

void CFringeWnd::Export(const TCHAR* fname) {
	dat.Export(fname);
}

void CFringeWnd::ClearHLine() {
	hLin.clear();
}

void CFringeWnd::AddVLine(SVHLine V) {
	vLin.push_back(V);
}

void CFringeWnd::AddHLine(SVHLine V) {
	hLin.push_back(V);
}

#include <fstream>
std::ofstream myFile1;

void CFringeWnd::DrawMainLines(CDC& dc, IMGL::Slin& lin) {
	IMGL::SPtS* pPks = &lin.ln.Pts[0];
	int sz = lin.size();
	if (!pPks || (sz < 1)) return;

	short nMkr = lin.nMkr;
	COLORREF cr = lin.cr;
	IMGL::Slin::EDType Lty = lin.type;

	int x, ix, y = 0;
	int wd = rcPlot.Width();
	//////////////////////////////////////////////////////////////////////////
	// first data set [3/26/2021 FSM]
	// Color channel [10/8/2020 FSM]
	CPen* pPen1 = new CPen(PS_SOLID, 2, cr);
	CPen* pPen2 = new CPen(PS_SOLID, 2, RGB((cr && 0xFF), (cr && 0xFF00) >> 8, (cr && 0xFF0000) >> 16));
	CPen* pOPen = (CPen*)dc.SelectObject(pPen1);

	/*x = rcPlot.left + int(CPar.sx * (pPks[0].x - CPar.Xmin));
	y = rcPlot.bottom - int(CPar.sy * (pPks[0].y - CPar.Ymin));*/

	// 05302023 - Mortuja [START]

	if (ICC.isHeightMapPlot) {
		if (ICC.isOriental == ICC.HORIZONTAL) {
			x = rcPlot.left + int(CPar.sx * (pPks[ICC.x1].x - CPar.Xmin));
			y = rcPlot.bottom - int(CPar.sy * (pPks[ICC.x1].y - CPar.Ymin));
		}
		else {
			x = rcPlot.left + int(CPar.sx * (pPks[ICC.y1].x - CPar.Xmin));
			y = rcPlot.bottom - int(CPar.sy * (pPks[ICC.y1].y - CPar.Ymin));
		}
	}
	else {
		x = rcPlot.left + int(CPar.sx * (pPks[0].x - CPar.Xmin));
		y = rcPlot.bottom - int(CPar.sy * (pPks[0].y - CPar.Ymin));
	}

	if (ICC.isHeightMapPlot) {
		ICC.HeightDataOvrCord.clear();
		myFile1.open("C:\\WLI\\pixels_plotting_data.txt");
		if (ICC.isOriental == ICC.HORIZONTAL) {
			ICC.HeightDataOvrCord.push_back({ {x, y}, pPks[ICC.x1].y });
			myFile1 << x << " " << y << " " << pPks[ICC.x1].y << std::endl;
		}
		else {
			ICC.HeightDataOvrCord.push_back({ {x, y}, pPks[ICC.y1].y });
			myFile1 << x << " " << y << " " << pPks[ICC.y1].y << std::endl;
		}
	}

	// 05302023 - Mortuja [END]

	DrawMarker(dc, x, y, pPen1, pPen2, nMkr);
	if (Lty == IMGL::Slin::LINEC) dc.MoveTo(x, y);
	int on = 0;
	for (int i = 1; i < wd; i++) {
		ix = int(CPar.sxx * i); if (ix >= sz) break;
		if (ICC.isHeightMapPlot && ICC.isOriental == ICC.HORIZONTAL && (ix < ICC.x1 || ICC.x2 - 1 < ix)) continue; // 05302023 - Mortuja
		if (ICC.isHeightMapPlot && ICC.isOriental == ICC.VERTICAL && (ix < ICC.y1 || ICC.y2 - 1 < ix)) continue; // 05302023 - Mortuja
		if (pPks[ix].y == BADDATA) continue;
		if (on != ix) {
			on = ix;
			x = rcPlot.left + int(CPar.sx * (pPks[ix].x - CPar.Xmin));
			y = rcPlot.bottom - int(CPar.sy * (pPks[ix].y - CPar.Ymin));
			if (Lty != IMGL::Slin::POINTC) {
				if (Lty == IMGL::Slin::BARC) dc.MoveTo(x, rcPlot.bottom);
				dc.LineTo(x, y);
				if (ICC.isHeightMapPlot) { // 05302023 - Mortuja
					ICC.HeightDataOvrCord.push_back({ {x, y}, pPks[ix].y });
					myFile1 << x << " " << y << " " << pPks[ix].y << std::endl;
				}
			}
			DrawMarker(dc, x, y, pPen1, pPen2, nMkr);
		}
	}
	dc.SelectObject(pOPen);
	if (pPen1) delete pPen1;
	if (pPen2) delete pPen2;
	dc.SelectClipRgn(NULL);
	if (ICC.isHeightMapPlot) { // 05302023 - Mortuja
		DrawLabelForHeightMapPlot(dc);
		myFile1.close();
	}
	ICC.isHeightMapPlot = false;
}

void CFringeWnd::DrawLabelForHeightMapPlot(CDC& dc) {
	CString str;
	dc.SetTextAlign(TA_CENTER | TA_TOP);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(255, 0, 0));
	dc.TextOutW((rcPlot.right + 108) / 2, rcChart.bottom + 25, L"Distance");
	PLOGFONT plf = (PLOGFONT)LocalAlloc(LPTR, sizeof(LOGFONT));
	HGDIOBJ hfnt, hfntPrev;
	plf->lfEscapement = 901;
	hfnt = CreateFontIndirect(plf);
	hfntPrev = SelectObject(dc, hfnt);
	dc.SetTextAlign(TA_CENTER | TA_TOP | TA_BASELINE);
	dc.TextOutW(rcPlot.left - 75, (rcChart.bottom + 35) / 2, L"Height [um]");
	Invalidate(FALSE);
}

void CFringeWnd::DrawT(CDC& dc, std::string& title) {
	dc.SetTextAlign(TA_CENTER | TA_TOP);
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0, 0, 255));
	dc.TextOut(rcChart.left + rcChart.Width() / 2, 0, CString(title.c_str()));
}

void CFringeWnd::DrawX(CDC& dc) {
	CString str;
	dc.SetTextAlign(TA_LEFT | TA_TOP);
	str.Format(fmtX, CPar.Xmin);
	dc.TextOutW(rcPlot.left, rcChart.bottom, str);

	//dc.SetTextAlign(TA_CENTER | TA_TOP);
	//str.Format(fmtX, (CPar.Xmax + CPar.Xmin) / 2.f);
	//dc.TextOutW((rcPlot.left + rcPlot.right) / 2, rcChart.bottom, str);
	//
	//str.Format(fmtX, (CPar.Xmax + CPar.Xmin) / 4.f);
	//dc.TextOutW((rcPlot.left + rcPlot.right) / 4, rcChart.bottom, str);
	//
	//str.Format(fmtX, 3 * (CPar.Xmax + CPar.Xmin) / 4.f);
	//dc.TextOutW(3 * (rcPlot.left + rcPlot.right) / 4, rcChart.bottom, str);

	dc.SetTextAlign(TA_RIGHT | TA_TOP);
	str.Format(fmtX, CPar.Xmax);
	dc.TextOutW(rcPlot.right - 2, rcChart.bottom, str);

	dc.MoveTo(rcChart.left, rcChart.bottom); dc.LineTo(rcChart.right, rcChart.bottom);
	dc.MoveTo(rcChart.left, rcChart.bottom); dc.LineTo(rcChart.right, rcChart.bottom);
}

void CFringeWnd::DrawX1(CDC& dc) {
	CString str;
	dc.SetTextAlign(TA_CENTER | TA_TOP);
	str.Format(_T("%d"), 1);
	dc.TextOutW(rcChart.left, rcChart.bottom, str);
	//dc.SetTextAlign(TA_RIGHT | TA_TOP);
	//str.Format(_T("%d"), int(sz));
	//dc.TextOutW(rcChart.right, rcChart.bottom, str);
}

void CFringeWnd::DrawY(CDC& dc) {
	CString str;
	dc.SetTextAlign(TA_RIGHT | TA_TOP);
	str.Format(fmtY, CPar.Ymax);
	dc.TextOutW(rcChart.left - 2, rcPlot.top, str);
	dc.SetTextAlign(TA_RIGHT | TA_BASELINE);
	//str.Format(fmtY, (CPar.Ymax + CPar.Ymin) / 2.0f);
	//dc.TextOutW(rcChart.left - 2, rcPlot.top + rcPlot.Height() / 2 + 4, str);
	dc.SetTextAlign(TA_RIGHT | TA_BOTTOM);
	str.Format(fmtY, CPar.Ymin);
	dc.TextOutW(rcChart.left - 2, rcPlot.bottom, str);

	dc.MoveTo(rcChart.left, rcChart.bottom); dc.LineTo(rcChart.left, rcChart.top);
}

void CFringeWnd::DrawY1(CDC& dc) {
	CString str;
	dc.SetTextAlign(TA_RIGHT | TA_TOP);
	str.Format(_T("%.2f"), CPar.Ymax);
	dc.TextOutW(rcChart.left - 2, rcChart.top, str);
	dc.SetTextAlign(TA_RIGHT | TA_BASELINE);
	//str.Format(_T("%.2f"), (CPar.Ymax + CPar.Ymin) / 2.0f);
	//dc.TextOutW(rcChart.left - 2, rcChart.top + rcChart.Height() / 2 + 4, str);
	dc.SetTextAlign(TA_RIGHT | TA_BOTTOM);
	str.Format(_T("%.2f"), CPar.Ymin);
	dc.TextOutW(rcChart.left - 2, rcChart.bottom, str);
}

void CFringeWnd::Label(std::string title, std::string xlbel, std::string ylbel) {
	dat.label(title, xlbel, ylbel);
}

void CFringeWnd::OnPaint() {
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages

	OnDraw(&dc);
}

void CFringeWnd::OnSize(UINT nType, int cx, int cy) {
	CWnd::OnSize(nType, cx, cy);

	// [ 05302023 - Mortuja START
	int wd, ht, bpp;
	Strip.GetDim(wd, ht, bpp);
	if (this->CPar.sz == wd || this->CPar.sz == ht) ICC.isHeightMapPlot = true;
	// 05302023 - Mortuja END ]

	Redraw(TRUE);
}

void CFringeWndEx::DrawROI(CDC& dc, SROI& R) {
	//int x, sdc = dc.SaveDC();
	//CPen* pPen = new CPen(PS_DASHDOT, 1, CR.crB);
	//CPen* pOPen = dc.SelectObject(pPen);
	//if (R.i1) { // Draw ROI 1 [4/7/2021 FSM]
	//	x = rcChart.left + int(CPar.sx * (R.x1_um - CPar.Xmin)) + 1;
	//	dc.MoveTo(x, rcChart.top);
	//	dc.LineTo(x, rcChart.bottom);
	//};
	//if (R.i2) { // Draw ROI 1 [4/7/2021 FSM]
	//	x = rcChart.left + int(CPar.sx * (R.x2_um - CPar.Xmin)) + 1;
	//	dc.MoveTo(x, rcChart.top);
	//	dc.LineTo(x, rcChart.bottom);
	//}
	//dc.SelectObject(pOPen); delete pPen;
	//dc.RestoreDC(sdc);
}

void CFringeWndEx::OnDraw(CDC* pDC) {
	//CDC dc; if (!dc.CreateCompatibleDC(pDC)) return;
	//if (BMP.m_hObject) { ShowBMP(dc, pDC); return; } // done and exit [11/7/2020 FSM]
	//SetupRect();
	//if (!BMP.CreateCompatibleBitmap(pDC, rcAll.Width(), rcAll.Height())) return;
	//CBitmap* pOBMP = dc.SelectObject(&BMP);
	//CreateBMPSurface(dc);
	//////////////////////////////////////////////////////////////////////////
	// main plot [2/15/2021 FSM]
	// int sz = int(Peak4.size());
	// if (sz > 0) {
	// 	MaxMinAll(Peak4); CalcScaling(Peak4);
	// 	for (int i = 0; i < sz; i++) {
	// 		SPeakArray& A = Peak4[i];
	// 		if (A.size() > 0) DrawMainLines(dc, A.GetPeak(), A.size(), A.cr, mMarker[3], LnTy[3]);
	// 	}
	// }
	// sz = int(Peak3.size());
	// if (sz > 0) {
	// 	MaxMinAll(Peak3); CalcScaling(Peak3);
	// 	for (int i = 0; i < sz; i++) {
	// 		SPeakArray& A = Peak3[i];
	// 		if (A.size() > 0) DrawMainLines(dc, A.GetPeak(), A.size(), A.cr, mMarker[2], LnTy[2]);
	// 	}
	// }
	// sz = int(Peak2.size());
	// if (sz > 0) {
	// 	MaxMinAll(Peak2); CalcScaling(Peak2);
	// 	for (int i = 0; i < sz; i++) {
	// 		SPeakArray& A = Peak2[i];
	// 		if (A.size() > 0) DrawMainLines(dc, A.GetPeak(), A.size(), A.cr, mMarker[1], LnTy[1]);
	// 	}
	// }
	// sz = int(Peak1.size());
	// if (sz > 0) {
	// 	MaxMinAll(Peak1); CalcScaling(Peak1);
	// 	DrawROI(dc, R);
	// 	DrawHLines(dc);
	// 	DrawVLines(dc);
	// 	for (int i = 0; i < sz; i++) {
	// 		SPeakArray& A = Peak1[i];
	// 		if (A.size() > 0) DrawMainLines(dc, A.GetPeak(), A.size(), A.cr, mMarker[0], LnTy[0]);
	// 	}
	// }
	//////////////////////////////////////////////////////////////////////////
	// Draw others [10/25/2020 yuenl]
	//CFont font, * pfont = NULL;
	//font.CreateFont(FontSz, 0, 0, 0, FW_LIGHT, 0, 0, 0, ANSI_CHARSET, 0, CLIP_DEFAULT_PRECIS,
	//	DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"));
	//pfont = dc.SelectObject(&font);
	//dc.SetBkMode(TRANSPARENT);
	//CString str = _T("");
	//DrawT(dc, str);
	//DrawY(dc);
	//DrawX(dc);
	//if (pfont) dc.SelectObject(pfont);
	//dc.SelectClipRgn(NULL);
	//pDC->BitBlt(rcAll.left, rcAll.top, rcAll.Width(), rcAll.Height(), &dc, 0, 0, SRCCOPY);
	//dc.SelectObject(pOBMP);
}

BEGIN_MESSAGE_MAP(CFringeWndEx, CFringeWnd)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CFringeWndEx::OnLButtonDblClk(UINT nFlags, CPoint point) {
	//! NOTE: only works with Peaks1[0] [4/7/2021 FSM]
	//!
	//if (!Peak1.size()) return;
	//int sz = int(Peak1[0].Pks.size());

	// CFringeWndOptionDlg dlg;
	// if (dlg.DoModal() == IDOK) {
	// 	switch (dlg.Sel) {
	// 	case CFringeWndOptionDlg::LLIM1:
	// 		R.i1 = int(sz * (float(point.x - rcChart.left) / float(rcChart.Width())));
	// 		R.x1_um = Peak1[0].Pks[R.i1].x_um;
	// 		/*if (R1.Validate(sz)) UI.SetR1(R1);*/
	// 		break;
	// 	case CFringeWndOptionDlg::ULIM1:
	// 		R.i2 = int(sz * (float(point.x - rcChart.left) / float(rcChart.Width())));
	// 		R.x2_um = Peak1[0].Pks[R.i2].x_um;
	// 		/*if (R1.Validate(sz)) UI.SetR1(R1);*/
	// 		break;
	// 	case CFringeWndOptionDlg::CLRALLROI:
	// 		//R1.Clear();
	// 		UI.ClearROIs();
	// 		break;
	// 	}
	// 	Redraw(FALSE);
	// }
}

void CFringeWndEx::OnLButtonDown(UINT nFlags, CPoint point) {}

void CFringeWndEx::OnLButtonUp(UINT nFlags, CPoint point) {}

BOOL CFringeWndEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) { return TRUE; }

void CFringeWndEx::OnMouseMove(UINT nFlags, CPoint point) {}

void SPlot::clear() {
	line1.clear(); line2.clear(); line3.clear(); line4.clear();
}

IMGL::SLines& SPlot::get(int n) {
	switch (n) {
	case 2: return line2; break;
	case 3: return line3; break;
	case 4: return line4; break;
	default: return line1; break;
	}
}

SPlot::SPlot() {
	// give each line an id
	line1.name = "ln1"; line2.name = "ln2";
	line3.name = "ln3"; line4.name = "ln4";
}

void SPlot::maxmin() {
	line1.maxmin(); line2.maxmin();
	line3.maxmin(); line4.maxmin();
}

void SPlot::label(std::string ttle, std::string xlbel, std::string ylbel) {
	title = ttle; xlabel = xlbel; ylabel = ylbel;
}

void SPlot::dump(char* fname) {
	FILE* fp = fopen(fname, "wb");
	if (!fp) return;
	if (line1.size()) { line1.dump(fp); }
	if (line2.size()) { line2.dump(fp); }
	if (line3.size()) { line3.dump(fp); }
	if (line4.size()) { line4.dump(fp); }
	fclose(fp);
}

bool SPlot::Export(const TCHAR* fname) {
	FILE* fp = _wfopen(fname, L"wb"); if (!fp) return false;
	fprintf(fp, "Title,%s\nX label,%s\nY label,%s\n",
		title.c_str(), xlabel.c_str(), ylabel.c_str());
	if (line1.size() > 0) {
		fprintf(fp, "line group 1\n");
		line1.dump(fp);
	}
	if (line2.size() > 0) {
		fprintf(fp, "line group 2\n");
		line2.dump(fp);
	}
	if (line3.size() > 0) {
		fprintf(fp, "line group 3\n");
		line3.dump(fp);
	}
	if (line4.size() > 0) {
		fprintf(fp, "line group 4\n");
		line4.dump(fp);
	}
	if (line5.size() > 0) {
		fprintf(fp, "line group 5\n");
		line5.dump(fp);
	}
	fclose(fp);
	return true;
}

bool SPlot::Export(const TCHAR* fname, int n) {
	bool bRet = false;
	FILE* fp = _wfopen(fname, L"wb"); if (!fp) return bRet;
	IMGL::SLines& line = Get(n);
	if (line.size() > 0) {
		line.dump1(fp); bRet = true;
	}
	fclose(fp);
	return bRet;
}

IMGL::SLines& SPlot::Get(int n) {
	switch (n) {
	case 2: return line2; break;
	case 3: return line3; break;
	case 4: return line4; break;
	case 5: return line5; break;
	default: return line1; break;
	}
}