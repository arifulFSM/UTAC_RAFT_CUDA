// HeightPlot.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "PSI/ICC.h"
#include "PSI/Strip.h"
#include "HeightPlot.h"
#include <iostream>
#include <fstream>

// HeightPlot dialog

#define PI 3.141592653589793238463 //20230801
#define PixelSize 4.8

HeightPlot hPlot;

IMPLEMENT_DYNAMIC(HeightPlot, CResizableDialog)

HeightPlot::HeightPlot(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DLG_HEIGHT_PLOT, pParent) {}

HeightPlot::~HeightPlot() {}

void HeightPlot::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HEIGHT_MAP, cPlot);
	DDX_Control(pDX, IDC_SHOW_HEIGHT_DATA, cHeightData);
}

BEGIN_MESSAGE_MAP(HeightPlot, CResizableDialog)
	ON_MESSAGE(IDC_HEIGHT_MAP_PM, &HeightPlot::OnUmHeightCalc)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// HeightPlot message handlers

BOOL HeightPlot::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_HEIGHT_MAP, TOP_LEFT, TOP_RIGHT);
	ArrangeLayout();

	cPlot.fmtX = "%.2f";
	cPlot.fmtY = "%.4f";
	stepHeight = 0;	//07182023
	CDLength = 0;	//20230801
	CDWidth = 0;	//20230801
	depth = 0;	//20230801

	return TRUE;
}

void HeightPlot::OnLButtonDown(UINT nFlags, CPoint point) {
	if (GetAsyncKeyState(VK_SHIFT) & 0xF0000) return;

	/*static MTH::SPointf Pt;
	CRect graphRect = ICC.gRect;

	Pt.x = (point.x - graphRect.left) / float(graphRect.Width());
	Pt.y = (point.y - graphRect.top) / float(graphRect.Height());

	int X = graphRect.left + (int)(Pt.x * (point.x - graphRect.left));
	int Y = graphRect.left;

	int idx = (point.x - graphRect.left);

	TRACE("x = %d, y = %d | X = %d, Y = %d\n", point.x, point.y, X, Y);*/
	//07182023
	if (cPlot.StepHeightPts.size() >= 2) {
		cPlot.StepHeightPts.erase(cPlot.StepHeightPts.begin());
	}
	SFPoint FPoint;
	FPoint.x = point.x;
	FPoint.y = point.y;
	cPlot.StepHeightPts.push_back(FPoint);
	if (cPlot.StepHeightPts.size() == 2) {
		CString dataT;
		cHeightData.GetWindowTextW(dataT);

		int pt1, pt2, dist1 = INT_MAX, dist2 = INT_MAX;
		for (int i = 0; i < ICC.HeightDataOvrCord.size(); i++) {
			if (abs(cPlot.StepHeightPts[0].x - ICC.HeightDataOvrCord[i].first.first) < dist1) {
				dist1 = abs(cPlot.StepHeightPts[0].x - ICC.HeightDataOvrCord[i].first.first);
				pt1 = i;
			}
			if (abs(cPlot.StepHeightPts[1].x - ICC.HeightDataOvrCord[i].first.first) < dist2) {
				dist2 = abs(cPlot.StepHeightPts[1].x - ICC.HeightDataOvrCord[i].first.first);
				pt2 = i;
			}
		}
		stepHeight = abs(ICC.HeightDataOvrCord[pt2].second - ICC.HeightDataOvrCord[pt1].second);
		OnMouseMove(0, point);
	}

	// [ 05302023 - Mortuja START
	int wd, ht, bpp;
	Strip.GetDim(wd, ht, bpp);
	if (cPlot.CPar.sz == wd || cPlot.CPar.sz == ht) ICC.isHeightMapPlot = true;
	// 05302023 - Mortuja END ]

	cPlot.Redraw(TRUE);
	CResizableDialog::OnLButtonDown(nFlags, point);
}

void HeightPlot::ShowPlot(int x, int y) {
	ICC.isHeightMapPlot = true;
	IMGL::Slin lin;
	IMGL::CIM16& Im = Strip.Im16um;
	Im.Make24H();
	if (ICC.isRegionType == ICC.LINE) { // 07122023
		if (ICC.isOriental == ICC.HORIZONTAL) Im.GetHLn(lin.ln, y); // Horizontal
		else Im.GetVLn(lin.ln, x); // Vertical
	}
	else { // 07122023
		Im.GetAreaAvgHLn(lin.ln, y);
	}
	lin.maxmin();
	lin.setprop(RGB(0, 182, 255)/*, IMGL::Slin::LINEC, 1*/);
	cPlot.Clear();
	cPlot.Plot(lin, 1);
	cPlot.Redraw(TRUE);
}

afx_msg LRESULT HeightPlot::OnUmHeightCalc(WPARAM wParam, LPARAM lParam) {
	ShowPlot(ICC.x1, ICC.y1);
	cPlot.ShowWindow(SW_SHOW);
	//20230801 ------------------------------------
	if (ICC.isRegionType == ICC.AREA) {
		//DetectCD(); // For Testing
	}
	//20230801 ------------------------------------
	return 0;
}

void HeightPlot::OnMouseMove(UINT nFlags, CPoint point) {
	TRACE("X = %d, Y = %d\n", point.x, point.y);

	double HeightValue = 0.0;
	for (auto X : ICC.HeightDataOvrCord) {
		if (X.first.first == point.x && X.first.second == point.y) {
			HeightValue = X.second;
			break;
		}
	}

	int X, Y;
	X = round((point.x - cPlot.rcPlot.left) / cPlot.CPar.sx);
	if (ICC.isRegionType == ICC.LINE) {
		if (ICC.isOriental == ICC.HORIZONTAL) {
			Y = ICC.y1;
		}
		else {
			Y = X;
			X = ICC.x1;
		}
	}
	else {
		Y = ICC.y1 + (ICC.y2 - ICC.y1 + 1) / 2;
	}

	CString dataT = L"";
	//20230801=======================================
	if (ICC.isRegionType == ICC.AREA) {
		dataT.Format(L"X = %d, Y = %d, Height = %.6f [um], CD Length = %.6f [um], CD Width = %.6f [um], CD Depth = %.6f [um]", X, Y, HeightValue, CDLength, CDWidth, depth);
	}
	else {
		dataT.Format(L"X = %d, Y = %d, Height = %.6f [um], Step Height = %.6f [um]", X, Y, HeightValue, stepHeight);
	}
	cHeightData.SetWindowTextW(dataT);

	CResizableDialog::OnMouseMove(nFlags, point);
}

//20230801 =============================
void HeightPlot::DetectCD() {
	int maxWidth1 = INT_MIN;
	int maxWidth2 = INT_MIN;
	int rowNo, colNo1, colNo2, yTemp1, rowNo1, rowNo2, colNo, xTemp1;
	bool isTransition = false;
	int windowSize = 2;
	double angle;
	int state = 0;
	double depth1, depth2, minHeight = INT_MAX, maxHeight = INT_MIN;
	double difference = 10;
	std::ofstream myfile;
	myfile.open("C:/WLI/test.csv");
	for (int i = 0; i < ICC.LineData.size(); i++) {
		for (int j = 0; j < ICC.LineData[0].size(); j++) {
			myfile << ICC.LineData[i][j] << ",";
		}
		myfile << "\n";
	}
	myfile.close();

	for (int i = 0; i < ICC.LineData.size(); i++) { // Horizontal
		state = 0;
		minHeight = INT_MAX;
		maxHeight = INT_MIN;
		for (int j = 0; j < ICC.LineData[i].size() - windowSize; j++) {
			if (ICC.LineData[i][j] == BADDATA) {
				continue;
			}
			isTransition = true;
			for (int k = 1; k < windowSize; k++) {
				if (ICC.LineData[i][j + k] == BADDATA) {
					continue;
				}
				angle = ((ICC.LineData[i][j] - ICC.LineData[i][j + k]) / k) * (180 / PI);
				if (state == 0 && angle < -85) {}
				else if (state == 1 && angle > 70) {}
				else if (state == 2 && angle < 60) {}
				else {
					isTransition = false;
				}
			}
			if (isTransition) {
				state++;
			}
			if (state != 0) {
				minHeight = std::min(minHeight, ICC.LineData[i][j]);
				maxHeight = std::max(maxHeight, ICC.LineData[i][j]);
			}
			if (isTransition && state == 1) {
				yTemp1 = j;
			}
			if (state == 3) {
				if (maxWidth1 < (j - yTemp1 + 1)) {
					maxWidth1 = j - yTemp1 + 1;
					rowNo = i;
					colNo1 = yTemp1;
					colNo2 = j;
					depth1 = maxHeight - minHeight;
				}
				state = 0;
				minHeight = INT_MAX;
				maxHeight = INT_MIN;
			}
		}
	}
	//Strip.Imgs[ICC.nIdx]->Im.DrawLine(colNo1+ICC.x1, rowNo+ICC.y1+1, colNo2+ICC.x1, rowNo+ICC.y1+1, 255, 0, 0);

	for (int j = 0; j < ICC.LineData[0].size(); j++) { // Vertical
		state = 0;
		minHeight = INT_MAX;
		maxHeight = INT_MIN;
		for (int i = 0; i < ICC.LineData.size() - windowSize; i++) {
			if (ICC.LineData[i][j] == BADDATA) {
				continue;
			}
			isTransition = true;
			for (int k = 1; k < windowSize; k++) {
				if (ICC.LineData[i + k][j] == BADDATA) {
					continue;
				}
				angle = ((ICC.LineData[i][j] - ICC.LineData[i + k][j]) / k) * (180 / PI);
				if (state == 0 && angle < -85) {}
				else if (state == 1 && angle > 70) {}
				else if (state == 2 && angle < 60) {}
				else {
					isTransition = false;
				}
			}
			if (isTransition) {
				state++;
			}
			if (state != 0) {
				minHeight = std::min(minHeight, ICC.LineData[i][j]);
				maxHeight = std::max(maxHeight, ICC.LineData[i][j]);
			}
			if (isTransition && state == 1) {
				xTemp1 = i;
			}
			if (state == 3) {
				if (maxWidth2 < (i - xTemp1 + 1)) {
					maxWidth2 = i - xTemp1 + 1;
					rowNo2 = i;
					rowNo1 = xTemp1;
					colNo = j;
					depth2 = maxHeight - minHeight;
				}
				else if (maxWidth2 == (i - xTemp1 + 1) && abs((colNo1 + (colNo2 - colNo1) / 2) - j) <= difference) {
					difference = abs((colNo1 + (colNo2 - colNo1) / 2) - j);
					rowNo2 = i;
					rowNo1 = xTemp1;
					colNo = j;
					depth2 = maxHeight - minHeight;
				}
				minHeight = INT_MAX;
				maxHeight = INT_MIN;
				state = 0;
			}
		}
	}

	CDLength = maxWidth1; // Horizontal
	CDWidth = maxWidth2; // Vertical
	depth = (depth1 + depth2) / 2.0;
	//Strip.Imgs[ICC.nIdx]->Im.DrawLine(colNo + ICC.x1+1, rowNo1 + ICC.y1, colNo + ICC.x1+1, rowNo2 + ICC.y1, 255, 0, 0);
	//cPic
	//TRACE("%d\n", 1);
}
//20230801 =============================