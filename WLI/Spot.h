#pragma once

#include "IM/IM.h"
#include "WLI/PicWnd.h"

#define MAXSPOTCTX    2
#define MAXSPOTCTXITM 6

struct SSpotCtx {
	enum EOBJ { DARK = 0, BRIGHT = 255 };
	static CString Prop[MAXSPOTCTXITM]; // header
	BYTE Obj = BRIGHT, Thres = 127;
	int lmt = 256, clmt = 7;
	int Wdw = 5, Sz = 24;

	void Serialize(CArchive& ar);
};

struct SSpot {
	int x1 = 0, y1 = 0;
	int x2 = 0, y2 = 0;

	bool FindSpot(IMGL::CIM& Im, IMGL::CIM& ImT, CPicWnd& cPicWnd);
	bool DecodeBlob(IMGL::CIM& Im, SSpotCtx& Ctx, std::vector<IMGL::SCgI>& CG, CPicWnd& cPicWnd);
	bool DecodeStripe(IMGL::CIM& Im, SSpotCtx& Ctx, std::vector<IMGL::SCgI>& CG, CPicWnd& cPicWnd);
	bool Replace(USHORT from, IMGL::CIM& Im, IMGL::CIM2& Im2, IMGL::SCgI& CG, COLORREF cr);
	void ShowImage1(CPicWnd& cPicWnd, IMGL::CIM& Im);
	void ShowImage2(CPicWnd& cPicWnd, IMGL::CIM2& Im2);
	void Yld(DWORD dur);
};
