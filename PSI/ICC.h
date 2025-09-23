#pragma once

#include <string>
#include "ScanPar.h"

struct SICC {
	//////////////////////////////////////////////////////////////////////////
	// UI
	bool bRed = false, bGrn = false, bBlu = false, bWht = true;
	bool bPhs = false, bVis = false;
	bool bLed = false, bLaser = false;
	bool bShowProgress = false;
	bool bShutter = true; // 12052023 - Shutter Close
	//////////////////////////////////////////////////////////////////////////
	// UI:AcqDlg
	SScanPar SPar;
	//////////////////////////////////////////////////////////////////////////

	int nIdx = -1;
	std::wstring ImgRoot = _T("");
	std::wstring outfile = _T("");

	// 05302023 - Mortuja
	int x1, y1, x2, y2;
	int x3, y3, x4, y4;
	bool isHeightMapPlot = 0;
	std::vector<std::pair<std::pair<int, int>, double>>HeightDataOvrCord;
	std::vector<std::vector<double>>LineData; // 20230801
	bool isOriental;
	bool isRegionType;
	enum VHTType { HORIZONTAL, VERTICAL };
	enum RegType { LINE, AREA };
	//bool isHeightFuncEnable = 0;
};

#define UM_FILELOADED  WM_USER+1
#define UM_FILECLEARED WM_USER+2

extern SICC ICC;
