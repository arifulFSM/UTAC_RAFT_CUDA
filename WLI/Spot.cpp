#include "pch.h"
#include <algorithm>

#include "WLI/Glbs.h"
#include "Spot.h"

CString SSpotCtx::Prop[6] = {
	L"Obj. 0=DARK 255=BRIGHT", L"Threshold", L"Obj. Size Limit", L"Max. Obj.",
	L"Window", L"Window Size"
};

void SSpotCtx::Serialize(CArchive& ar) {
	short magic = 1;
	if (ar.IsStoring()) {
		ar << magic;
		ar << Obj;
		ar << Thres;
		ar << lmt;
		ar << clmt;
		ar << Wdw;
		ar << Sz;
	}
	else {
		ar >> magic;
		ar >> Obj;
		ar >> Thres;
		ar >> lmt;
		ar >> clmt;
		ar >> Wdw;
		ar >> Sz;
	}
}

bool SSpot::FindSpot(IMGL::CIM& Im, IMGL::CIM& ImT, CPicWnd& cPicWnd) {
	// sum of difference method
	if (Im.IsNull() || ImT.IsNull()) return false;
	int iwd, iht, ibpp; Im.GetDim(iwd, iht, ibpp);
	int twd, tht, tbpp; ImT.GetDim(twd, tht, tbpp);
	if (!(twd % 2)) twd--; if (!(tht % 2)) tht--;
	int dht = tht / 2, dwd = twd / 2;
	//twd = 2 * dwd - 1; tht = 2 * dht - 1;
	int yed = iht - tht;
	int xed = iwd - twd;

	IMGL::CIM16 ImR;
	if (!ImR.Create(iwd, iht)) return false;
	float total = float(tht * twd);
#pragma omp parallel for
	for (int y = 0; y < yed; y++) {
		int v;
		float* pRst = ImR.GetPixelAddress(dwd / 2, y + dht);
		for (int x = 0; x < xed; x++, pRst++) {
			long sum = 0;
			for (int j = 0; j < tht; j++) {
				PBYTE pIm = (PBYTE)Im.GetPixelAddress(x, y + j);
				PBYTE pImT = (PBYTE)ImT.GetPixelAddress(0, j);
				for (int i = 0; i < twd; i++, pIm++, pImT++) {
					v = (*pIm * *pImT);
					sum += v;
				}
			}
			*pRst = float(sum / total);
		}
	}
	ImR.Make24BPPStretch(Im);
	ShowImage1(cPicWnd, Im);
	return true;
}

bool SSpot::DecodeBlob(IMGL::CIM& Im, SSpotCtx& Ctx, std::vector<IMGL::SCgI>& CG, CPicWnd& cPicWnd) {
	IMGL::SNxt nxt;
	IMGL::CIM2 Im2;

	if (Ctx.Obj) Im.BThres(Ctx.Thres, 0, 255); else Im.BThres(Ctx.Thres, 255, 0); ShowImage1(cPicWnd, Im);
	int repeat = 8;
	while (repeat > 0) {
		if (Im.Thining(Ctx.Wdw, Ctx.Sz, 255) < 384) {
			ShowImage1(cPicWnd, Im); break;
		}
		ShowImage1(cPicWnd, Im);
		repeat--;
	}
	if (!nxt.Segment(Im, Im2, 255)) return false; ShowImage2(cPicWnd, Im2);
	nxt.Identify(Im, Im2, Ctx.lmt, Ctx.clmt, CG, IMGL::SNxt::BLOB); ShowImage1(cPicWnd, Im);
	//nxt.Identify(Im, Im2, 8, 2, CG, IMGL::SNxt::BLOB);
	return true;
}

bool SSpot::DecodeStripe(IMGL::CIM& Im, SSpotCtx& Ctx, std::vector<IMGL::SCgI>& CG, CPicWnd& cPicWnd) {
	IMGL::SNxt nxt;
	IMGL::CIM2 Im2;

	if (Ctx.Obj) Im.BThres(Ctx.Thres, 0, 255); else Im.BThres(Ctx.Thres, 255, 0); //ShowImage1(cPicWnd, Im);
	while (1) {
		if (Im.Thining(Ctx.Wdw, Ctx.Sz, 255) < 512) {
			//ShowImage1(cPicWnd, Im);
			break;
		}
		//ShowImage1(cPicWnd, Im);
	}
	if (!nxt.Segment(Im, Im2, 255)) return false; //ShowImage2(cPicWnd, Im2);
	//nxt.Identify(Im, Im2, 256, 7, CG, IMGL::SNxt::STRIPE);
	nxt.Identify(Im, Im2, Ctx.lmt, Ctx.clmt, CG, IMGL::SNxt::STRIPE); //ShowImage1(cPicWnd, Im);
	return true;
}

bool SSpot::Replace(USHORT from, IMGL::CIM& Im, IMGL::CIM2& Im2, IMGL::SCgI& CG, COLORREF cr) {
	int wd, ht, bpp; Im.GetDim(wd, ht, bpp); if (bpp != 24) return false;
	if (!Im2.GetStripsStats(CG, from)) { return false; };
	Im.ReplacePixel(Im2, from, cr);
	Im.DrawCross(CG.cx, CG.cy, int(0.05f * wd), 255, 255, 0);
	return true;
}

void SSpot::ShowImage1(CPicWnd& cPicWnd, IMGL::CIM& Im) {
	/*cPicWnd.SetImg2(Im);*/ cPicWnd.Redraw(FALSE); Yld(10);
}

void SSpot::ShowImage2(CPicWnd& cPicWnd, IMGL::CIM2& Im2) {
	IMGL::CIM& Im = *cPicWnd.GetImg2();
	Im = Im2;
	cPicWnd.Redraw(FALSE); Yld(10);
}

void SSpot::Yld(DWORD dur) {
	MSG msg;
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < dur) {
		//? User message will be removed too
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}