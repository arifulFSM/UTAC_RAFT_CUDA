#include "pch.h"

// #include "UI.h"
// #include "MATH/LSQ/LSFit.h"
// #include "MATH/Math.h"
// #include "MATH/spline.h"
// #include "WLI/Scan.h"
// #include "WLI/CAM/ICache.h"
// #include "SRC/DOSUtil.h"
#include "PSI/PsiCalc.h"

#include "LucamImg.h"

using namespace CAM;

#define SIGNF(x) ((x) > 0)

int IMGL::SImg::GetInten(int x, int y, eCH nChan) {
	return Im.GetIntensity(x, y, nChan);
}

COLORREF IMGL::SImg::GetPixRGB(int x, int y) {
	return Im.GetPixel(x, y);
}

COLORREF IMGL::SImg::GetPixRGB(float x, float y) {
	return Im.GetPixel(int(x * Im.GetWidth()), int(y * Im.GetHeight()));
}

IMGL::SImg::SImg() {}

IMGL::SImg::~SImg() {
	if (!Im.IsNull()) Im.Destroy();
}

void IMGL::SImg::Serialize(CArchive& ar) {
	BOOL bAlloc = FALSE;
	USHORT magic = 1;
	if (ar.IsStoring()) {
		ar << magic;
		ar << PzPos_um;
		if (!Im.IsNull()) {
			bAlloc = TRUE;
			ar << bAlloc;
			Im.Serialize(ar);
		}
		else {
			ar << bAlloc;
		}
	}
	else {
		ar >> magic;
		ar >> PzPos_um;
		ar >> bAlloc;
		if (bAlloc) {
			Im.Serialize(ar);
		}
	}
}

IMGL::SImgStrip::SImgStrip() {}

IMGL::SImgStrip::~SImgStrip() {
	DeallocAll();
}

// BOOL IMGL::SImgStrip::AddPhasePointRGB(IMGL::CIM16& Im16, int idx, int x, int y,
// 	CHNL nChan, std::vector<SFrP1>& vP) {
// 	//? Require algo.PShift_rad must be initialized  [8/19/2021 FSM]
// 	//float stepPz_nm = algo.PShift_rad * D.Props.PiePz_nm[iChan];
// 	switch (algo.algo) {
// 	case EALGO::STEP3A:
// 		Im16.AddPixel(x, y, GetPhase3a(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP3B:
// 		Im16.AddPixel(x, y, GetPhase3b(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP3C:
// 		Im16.AddPixel(x, y, GetPhase3c(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP3E:
// 		Im16.AddPixel(x, y, GetPhase3e(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP3NV:
// 		Im16.AddPixel(x, y, GetPhase3NV_rad(x, y, algo.PShift_rad, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP4A:
// 		Im16.AddPixel(x, y, GetPhase4a(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP4C:
// 		Im16.AddPixel(x, y, GetPhase4c(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP4NV:
// 		Im16.AddPixel(x, y, GetPhase4NV(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP5A:
// 		Im16.AddPixel(x, y, GetPhase5a(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP5B:
// 		Im16.AddPixel(x, y, GetPhase5b(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP5C:
// 		Im16.AddPixel(x, y, GetPhase5c(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP5NV:
// 		Im16.SetPixel(x, y, GetPhase5NV_rad(x, y, algo.PShift_rad, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP6A:
// 		Im16.AddPixel(x, y, GetPhase6a(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP6B:
// 		Im16.AddPixel(x, y, GetPhase6b(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP7A:
// 		Im16.AddPixel(x, y, GetPhase7a(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP7B:
// 		Im16.AddPixel(x, y, GetPhase7b(x, y, nChan, vP).phase);
// 		break;
// 	case EALGO::STEP8A:
// 		Im16.AddPixel(x, y, GetPhase8a(x, y, nChan, vP).phase);
// 		break;
// 	default: ASSERT(0); return FALSE; break;
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStrip_WRGB_ST(SFrin4& F1, int x, int y, IMGL::SROI& R) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	float r, g, b, w;
// 	double av1 = 0, av2 = 0, av3 = 0, av4 = 0;
//
// 	SFrP4& Fc = F1.F[st];
// 	int imx1 = 0, imx2 = 0, imx3 = 0, imx4 = 0;
// 	int imn1 = 0, imn2 = 0, imn3 = 0, imn4 = 0;
// 	float mx1 = -(FLT_MAX - 1), mx2 = -(FLT_MAX - 1), mx3 = -(FLT_MAX - 1), mx4 = -(FLT_MAX - 1);
// 	float mn1 = FLT_MAX, mn2 = FLT_MAX, mn3 = FLT_MAX, mn4 = FLT_MAX;
//
// 	F1.F.resize(sz);
// 	SImg** pI = &Imgs[st];
// 	for (int i = st; i < ed; i++, pI++) {
// 		SFrP4& Fc = F1.F[i];
// 		Fc.i = i; Fc.x_um = (*pI)->PzPos_um;
// 		COLORREF cr = (*pI)->GetPixRGB(x, y);
//
// 		r = GetRValue(cr);
// 		av1 += (Fc.y[int(CHNL::REDCH)] = r);
// 		if (r > mx1) { mx1 = r; imx1 = i; }
// 		if (r < mn1) { mn1 = r; imn1 = i; }
//
// 		g = GetGValue(cr);
// 		av2 += (Fc.y[int(CHNL::GRNCH)] = g);
// 		if (g > mx2) { mx2 = g; imx2 = i; }
// 		if (g < mn2) { mn2 = g; imn2 = i; }
//
// 		b = GetBValue(cr);
// 		av3 += (Fc.y[int(CHNL::BLUCH)] = b);
// 		if (b > mx3) { mx3 = b; imx3 = i; }
// 		if (b < mn3) { mn3 = b; imn3 = i; }
//
// 		w = (r + 2 * b + b) / 4;
// 		av4 += (Fc.y[int(CHNL::BLUCH)] = w);
// 		if (w > mx4) { mx4 = w; imx4 = i; }
// 		if (w < mn4) { mn4 = w; imn4 = i; }
// 	}
//
// 	float N = float(ed - st);
// 	IMGL::SFrin4::SSt& St = F1.St;
// 	St.Ave[int(CHNL::REDCH)] = float(av1 / N);
// 	St.Ave[int(CHNL::GRNCH)] = float(av2 / N);
// 	St.Ave[int(CHNL::BLUCH)] = float(av3 / N);
// 	St.Ave[int(CHNL::WHTCH)] = float(av4 / N);
//
// 	St.Ymax[int(CHNL::REDCH)] = mx1; St.Ymin[int(CHNL::REDCH)] = mn1;
// 	St.Imax[int(CHNL::REDCH)] = imx1; St.Imin[int(CHNL::REDCH)] = imn1;
//
// 	St.Ymax[int(CHNL::GRNCH)] = mx2; St.Ymin[int(CHNL::GRNCH)] = mn2;
// 	St.Imax[int(CHNL::GRNCH)] = imx2; St.Imin[int(CHNL::GRNCH)] = imn2;
//
// 	St.Ymax[int(CHNL::BLUCH)] = mx3; St.Ymin[int(CHNL::BLUCH)] = mn3;
// 	St.Imax[int(CHNL::BLUCH)] = imx3; St.Imin[int(CHNL::BLUCH)] = imn3;
//
// 	St.Ymax[int(CHNL::WHTCH)] = mx4; St.Ymin[int(CHNL::WHTCH)] = mn4;
// 	St.Imax[int(CHNL::WHTCH)] = imx4; St.Imin[int(CHNL::WHTCH)] = imn4;
//
// 	return TRUE;
// }

// BOOL IMGL::SImgStrip::CollectStripWRGBST(SFrin4& F1, int x, int y, IMGL::SROI& R, SIn& In) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	int imn[4] = { 0 }, imx[4] = { 0 };
// 	float v = -(FLT_MAX - 1),
// 		mn[4] = { FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX },
// 		mx[4] = { v,v,v,v };
// 	double av[4] = { 0 };
//
// 	if (In.bSpdUp && In.bActive) {
// 		st = In.I[0].i - 3; ed = In.I[In.nStp - 1].i + 3;
// 		if (st < 0) st = 0; if (ed >= sz) ed = sz - 1;
// 	}
// 	F1.F.resize(sz);
// 	SImg** pI = &Imgs[st];
// 	for (int i = st; i < ed; i++, pI++) {
// 		SFrP4& Fc = F1.F[i];
// 		Fc.i = i; Fc.x_um = (*pI)->PzPos_um;
// 		COLORREF cr = (*pI)->GetPixRGB(x, y);
// 		v = Fc.y[int(CHNL::REDCH)] = GetRValue(cr);
// 		if (v < mn[int(CHNL::REDCH)]) {
// 			mn[int(CHNL::REDCH)] = v; imn[int(CHNL::REDCH)] = i;
// 		}
// 		if (v > mx[int(CHNL::REDCH)]) {
// 			mx[int(CHNL::REDCH)] = v; imx[int(CHNL::REDCH)] = i;
// 		}
// 		av[int(CHNL::REDCH)] += v;
//
// 		v = Fc.y[int(CHNL::GRNCH)] = GetGValue(cr);
// 		if (v < mn[int(CHNL::GRNCH)]) {
// 			mn[int(CHNL::GRNCH)] = v; imn[int(CHNL::GRNCH)] = i;
// 		}
// 		if (v > mx[int(CHNL::GRNCH)]) {
// 			mx[int(CHNL::GRNCH)] = v; imx[int(CHNL::GRNCH)] = i;
// 		}
// 		av[int(CHNL::GRNCH)] += v;
//
// 		Fc.y[int(CHNL::BLUCH)] = GetBValue(cr);
// 		v = Fc.y[int(CHNL::BLUCH)] = GetGValue(cr);
// 		if (v < mn[int(CHNL::BLUCH)]) {
// 			mn[int(CHNL::BLUCH)] = v; imn[int(CHNL::BLUCH)] = i;
// 		}
// 		if (v > mx[int(CHNL::BLUCH)]) {
// 			mx[int(CHNL::BLUCH)] = v; imx[int(CHNL::BLUCH)] = i;
// 		}
// 		av[int(CHNL::BLUCH)] += v;
//
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 		Fc.y[int(CHNL::WHTCH)] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f;
// #else
// 		Fc.y[int(CHNL::WHTCH)] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f;
// #endif
// #else
// #ifdef HALFWHITE
// 		Fc.y[int(CHNL::WHTCH)] = (GetGValue(cr) + GetBValue(cr)) / 2.f;
// #else
// 		Fc.y[int(CHNL::WHTCH)] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f;
// #endif
// #endif
// 		v = Fc.y[int(CHNL::WHTCH)] = GetGValue(cr);
// 		if (v < mn[int(CHNL::WHTCH)]) {
// 			mn[int(CHNL::WHTCH)] = v; imn[int(CHNL::WHTCH)] = i;
// 		}
// 		if (v > mx[int(CHNL::WHTCH)]) {
// 			mx[int(CHNL::WHTCH)] = v; imx[int(CHNL::WHTCH)] = i;
// 		}
// 		av[int(CHNL::WHTCH)] += v;
// 	}
//
// 	for (int i = int(CHNL::WHTCH); i <= int(CHNL::BLUCH); i++) {
// 		F1.St.Ave[i] = float(av[i] / (ed - st));
// 		F1.St.Ymax[i] = mx[i]; F1.St.Imax[i] = imx[i];
// 		F1.St.Ymin[i] = mn[i]; F1.St.Imin[i] = imn[i];
// 	}
//
// 	return TRUE;
// }

// BOOL IMGL::SImgStrip::CollectStrip(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL Fr, CHNL To, SIn& In) {
// 	if (In.bActive) {
// 		R.i1 = In.I[0].i; R.i2 = In.I[In.nStp].i;
// 	}
// 	return CollectStripF(F1, x, y, R, Fr, To);
// }
//
// BOOL IMGL::SImgStrip::CollectStripF(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL Fr, CHNL To) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	if ((ed - st) > sz) return FALSE;
// 	double ave = 0;
// 	F1.F.resize(sz);
// 	for (int j = int(Fr); j <= int(To); j++) {
// 		SFrP4* Fc = &F1.F[st]; SImg** pI = &Imgs[st];
// 		switch (CHNL(j)) {
// 		case CHNL::WHTCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				COLORREF cr = (*pI)->GetPixRGB(x, y);
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f);
// #else
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f);
// #endif
// #else
// #ifdef HALFWHITE
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (GetGValue(cr) + GetBValue(cr)) / 2.f);
// #else
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f);
// #endif
// #endif
// 			}
// 			break;
// 		case CHNL::REDCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				ave += (Fc->y[int(CHNL::REDCH)] = GetRValue((*pI)->GetPixRGB(x, y)));
// 			}
// 			break;
// 		case CHNL::GRNCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				ave += (Fc->y[int(CHNL::GRNCH)] = GetGValue((*pI)->GetPixRGB(x, y)));
// 			}
// 			break;
// 		case CHNL::BLUCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				ave += (Fc->y[int(CHNL::BLUCH)] = GetBValue((*pI)->GetPixRGB(x, y)));
// 			}
// 			break;
// 		default: ASSERT(0); return FALSE; break;
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStripZM(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL nChan) {
// 	if ((nChan < CHNL::WHTCH) || (nChan > CHNL::BLUCH)) {
// 		ASSERT(0); return FALSE;
// 	}
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	if (sz < (ed - st)) { ASSERT(0);  return FALSE; }
// 	double ave = 0;
// 	F1.F.resize(sz);
// 	SImg** pI = &Imgs[st]; SFrP4* Fc = &F1.F[st];
// 	switch (nChan) {
// 	case CHNL::WHTCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			COLORREF cr = (*pI)->GetPixRGB(x, y);
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f);
// #else
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f);
// #endif
// #else
// #ifdef HALFWHITE
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (GetGValue(cr) + GetBValue(cr)) / 2.f);
// #else
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f);
// #endif
// #endif
// 		}
// 		break;
// 	case CHNL::REDCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			ave += (Fc->y[int(CHNL::REDCH)] = GetRValue((*pI)->GetPixRGB(x, y)));
// 		}
// 		break;
// 	case CHNL::GRNCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			ave += (Fc->y[int(CHNL::GRNCH)] = GetGValue((*pI)->GetPixRGB(x, y)));
// 		}
// 		break;
// 	case CHNL::BLUCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			ave += (Fc->y[int(CHNL::BLUCH)] = GetBValue((*pI)->GetPixRGB(x, y)));
// 		}
// 		break;
// 	default: ASSERT(0); return FALSE; break;
// 	}
// 	float N = float(ed - st);
// 	Fc = &F1.F[st];
// 	for (int i = st; i < ed; i++, Fc++) {
// 		Fc->y[int(nChan)] -= float(ave / N);
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStripZMST(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL nChan) {
// 	if ((nChan < CHNL::WHTCH) || (nChan > CHNL::BLUCH)) {
// 		ASSERT(0); return FALSE;
// 	}
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	if (sz < (ed - st)) { ASSERT(0);  return FALSE; }
//
// 	int imn, imx;
// 	double ave = 0;
// 	float v, mn = FLT_MAX, mx = -(FLT_MAX - 1);
//
// 	F1.F.resize(sz);
// 	SImg** pI = &Imgs[st]; SFrP4* Fc = &F1.F[st];
// 	switch (nChan) {
// 	case CHNL::WHTCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			COLORREF cr = (*pI)->GetPixRGB(x, y);
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 			v = (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f);
// #else
// 			v = (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f);
// #endif
// #else
// #ifdef HALFWHITE
// 			v = (Fc->y[int(CHNL::WHTCH)] = (GetGValue(cr) + GetBValue(cr)) / 2.f);
// #else
// 			v = (Fc->y[int(CHNL::WHTCH)] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f);
// #endif
// #endif
// 			ave += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	case CHNL::REDCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			v = (Fc->y[int(CHNL::REDCH)] = GetRValue((*pI)->GetPixRGB(x, y)));
// 			ave += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	case CHNL::GRNCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			v = (Fc->y[int(CHNL::GRNCH)] = GetGValue((*pI)->GetPixRGB(x, y)));
// 			ave += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	case CHNL::BLUCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			v = (Fc->y[int(CHNL::BLUCH)] = GetBValue((*pI)->GetPixRGB(x, y)));
// 			ave += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	default: ASSERT(0); return FALSE; break;
// 	}
// 	ave /= float(ed - st);
// 	Fc = &F1.F[st];
// 	for (int i = st; i < ed; i++, Fc++) {
// 		Fc->y[int(nChan)] -= float(ave);
// 	}
//
// 	F1.St.Ave[int(nChan)] = float(ave);
// 	F1.St.Ymax[int(nChan)] = float(mx - ave); F1.St.Imax[int(nChan)] = imx;
// 	F1.St.Ymin[int(nChan)] = float(mn - ave); F1.St.Imin[int(nChan)] = imn;
//
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStrip(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL nChan) {
// 	if ((nChan < CHNL::WHTCH) || (nChan > CHNL::BLUCH)) {
// 		ASSERT(0); return FALSE;
// 	}
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	if (sz < (ed - st)) { ASSERT(0);  return FALSE; }
// 	double ave = 0;
// 	F1.F.resize(sz);
// 	SImg** pI = &Imgs[st]; SFrP4* Fc = &F1.F[st];
// 	switch (nChan) {
// 	case CHNL::WHTCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			COLORREF cr = (*pI)->GetPixRGB(x, y);
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f);
// #else
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f);
// #endif
// #else
// #ifdef HALFWHITE
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (GetGValue(cr) + GetBValue(cr)) / 2.f);
// #else
// 			ave += (Fc->y[int(CHNL::WHTCH)] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f);
// #endif
// #endif
// 		}
// 		break;
// 	case CHNL::REDCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			ave += (Fc->y[int(CHNL::REDCH)] = GetRValue((*pI)->GetPixRGB(x, y)));
// 		}
// 		break;
// 	case CHNL::GRNCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			ave += (Fc->y[int(CHNL::GRNCH)] = GetGValue((*pI)->GetPixRGB(x, y)));
// 		}
// 		break;
// 	case CHNL::BLUCH:
// 		for (int i = st; i < ed; i++, pI++, Fc++) {
// 			Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 			ave += (Fc->y[int(CHNL::BLUCH)] = GetBValue((*pI)->GetPixRGB(x, y)));
// 		}
// 		break;
// 	default: ASSERT(0); return FALSE; break;
// 	}
// 	F1.St.Ave[int(nChan)] = float(ave / float(ed - st));
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStrip(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL Fr, CHNL To) {
// 	for (int j = int(Fr); j <= int(To); j++) {
// 		CollectStrip(F1, x, y, R, CHNL(j));
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStripT3(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL Fr, CHNL To) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	if (sz < (ed - st)) { ASSERT(0); return FALSE; }
// 	F1.InitT3(15);
// 	F1.F.resize(sz);
// 	for (int j = int(Fr); j <= int(To); j++) {
// 		double ave = 0;
// 		CLo15& T3Lo = *F1.T3Lo[j];
// 		CHi15& T3Hi = *F1.T3Hi[j];
// 		T3Hi.Clear(); T3Lo.Clear();
// 		SImg** pI = &Imgs[st];  SFrP4* Fc = &F1.F[st];
// 		switch (CHNL(j)) {
// 		case CHNL::WHTCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				COLORREF cr = (*pI)->GetPixRGB(x, y);
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f);
// #else
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f);
// #endif
// #else
// #ifdef HALFWHITE
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (GetGValue(cr) + GetBValue(cr)) / 2.f);
// #else
// 				ave += (Fc->y[int(CHNL::WHTCH)] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f);
// #endif
// #endif
// 				T3Lo.Add(i, Fc->x_um, Fc->y[j]);
// 				T3Hi.Add(i, Fc->x_um, Fc->y[j]);
// 			}
// 			break;
// 		case CHNL::REDCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				ave += (Fc->y[int(CHNL::REDCH)] = GetRValue((*pI)->GetPixRGB(x, y)));
// 				T3Lo.Add(i, Fc->x_um, Fc->y[j]);
// 				T3Hi.Add(i, Fc->x_um, Fc->y[j]);
// 			}
// 			break;
// 		case CHNL::GRNCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				ave += (Fc->y[int(CHNL::GRNCH)] = GetGValue((*pI)->GetPixRGB(x, y)));
// 				T3Lo.Add(i, Fc->x_um, Fc->y[j]);
// 				T3Hi.Add(i, Fc->x_um, Fc->y[j]);
// 			}
// 			break;
// 		case CHNL::BLUCH:
// 			for (int i = st; i < ed; i++, pI++, Fc++) {
// 				Fc->i = i; Fc->x_um = (*pI)->PzPos_um;
// 				ave += (Fc->y[int(CHNL::BLUCH)] = GetBValue((*pI)->GetPixRGB(x, y)));
// 				T3Lo.Add(i, Fc->x_um, Fc->y[j]);
// 				T3Hi.Add(i, Fc->x_um, Fc->y[j]);
// 			}
// 			break;
// 		default: ASSERT(0); return FALSE; break;
// 		}
// 		F1.St.Ave[j] = float(ave / float(ed - st));
// 		T3Lo.Range(0.06f);
// 		T3Hi.Range(0.06f);
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStripST(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL Fr, CHNL To) {
// 	for (int j = int(Fr); j <= int(To); j++) {
// 		if (!CollectStripST(F1, x, y, R, CHNL(j))) return FALSE;
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CollectStripST(SFrin4& F1, int x, int y, IMGL::SROI& R, CHNL nChan) {
// 	if ((nChan < CHNL::WHTCH) || (nChan > CHNL::BLUCH)) {
// 		ASSERT(0); return FALSE;
// 	}
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int st = R.i1, ed = R.i2;
// 	if (sz < (ed - st)) { ASSERT(0); return FALSE; }
// 	F1.F.resize(sz);
//
// 	int imn, imx;
// 	int iChan = int(nChan);
// 	double av = 0;
// 	float v, mn = FLT_MAX, mx = -(FLT_MAX - 1);
// 	SFrP4* pP = &F1.F[st]; SImg** pI = &Imgs[st];
// 	switch (nChan) {
// 	case CHNL::WHTCH:
// 		for (int i = st; i < ed; i++, pI++, pP++) {
// 			pP->i = i; pP->x_um = (*pI)->PzPos_um;
// 			COLORREF cr = (*pI)->GetPixRGB(x, y);
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 			v = pP->y[iChan] = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f;
// #else
// 			v = pP->y[iChan] = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f;
// #endif
// #else
// #ifdef HALFWHITE
// 			v = pP->y[iChan] = (GetGValue(cr) + GetBValue(cr)) / 2.f;
// #else
// 			v = pP->y[iChan] = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f;
// #endif
// #endif
// 			av += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	case CHNL::REDCH:
// 		for (int i = st; i < ed; i++, pI++, pP++) {
// 			pP->i = i; pP->x_um = (*pI)->PzPos_um;
// 			v = pP->y[iChan] = GetRValue((*pI)->GetPixRGB(x, y));
// 			av += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	case CHNL::GRNCH:
// 		for (int i = st; i < ed; i++, pI++, pP++) {
// 			pP->i = i; pP->x_um = (*pI)->PzPos_um;
// 			v = pP->y[iChan] = GetGValue((*pI)->GetPixRGB(x, y));
// 			av += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	case CHNL::BLUCH:
// 		for (int i = st; i < ed; i++, pI++, pP++) {
// 			pP->i = i; pP->x_um = (*pI)->PzPos_um;
// 			v = pP->y[iChan] = GetBValue((*pI)->GetPixRGB(x, y));
// 			av += v;
// 			if (v > mx) { mx = v; imx = i; }
// 			if (v < mn) { mn = v; imn = i; }
// 		}
// 		break;
// 	default: ASSERT(0); return FALSE; break;
// 	}
//
// 	F1.St.Ave[iChan] = float(av / (ed - st));
// 	F1.St.Ymax[iChan] = mx; F1.St.Imax[iChan] = imx;
// 	F1.St.Ymin[iChan] = mn; F1.St.Imin[iChan] = imn;
//
// 	return TRUE;
// }

// float IMGL::SImgStrip::HeightXY(SPointf* pPt) {
// 	if (Im16_um.IsNull()) return BADDATA;
// 	//! pPt is in normalize unit [7/8/2021 FSM]
// 	if ((pPt->x_um < 0) || (pPt->x_um > 1.f)) {
// 		ASSERT(0); return BADDATA;
// 	}
// 	if ((pPt->y < 0) || (pPt->y > 1.f)) {
// 		ASSERT(0); return BADDATA;
// 	}
// 	return Im16_um.GetPixel(int(Im16_um.GetWidth() * pPt->x_um), int(Im16_um.GetHeight() * pPt->y));
// }

// void IMGL::SImgStrip::RemoveZero(float* pBuf, size_t sz) {
// 	float* pCpy = new float[sz]; if (!pCpy) return;
// 	memcpy(pCpy, pBuf, sz * sizeof(float));
// 	float* p = pCpy + 1;
// 	float* q = pBuf + 1;
// 	for (size_t i = 1; i < sz - 1; i++, p++, q++) {
// 		if (*p == 0) {
// 			*q = (*(p - 1) + *(p + 1)) / 2.f;
// 		}
// 	}
// 	memcpy(pCpy, pBuf, sz * sizeof(float));
// 	p = pCpy + 2;
// 	q = pBuf + 2;
// 	for (size_t i = 2; i < sz - 2; i++, p++, q++) {
// 		if (*p == 0) {
// 			*q = (*(p - 2) + *(p + 2)) / 2.f;
// 		}
// 	}
// 	delete[]pCpy;
// }

// void IMGL::SImgStrip::LevelRough(float* pBuf, size_t sz) {
// 	float* pBuf2 = new float[sz]; if (!pBuf2) return;
// 	memcpy(pBuf2, pBuf, sz * sizeof(float));
// 	MTH::CLSFit LS;
// 	float* p = pBuf2;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if (*p) {
// 			LS.Add(float(i), *p);
// 		}
// 	}
// 	double v, m, c;
// 	LS.Get(m, c); LS.Reset();
// 	p = pBuf2;
// 	for (int i = 0; i < sz; i++, p++) {
// 		v = m * i + c;
// 		if (fabs(*p - v) > 1.f) {
// 			*p = float(v);
// 		}
// 		LS.Add(float(i), *p);
// 	}
// 	LS.Get(m, c);
// 	p = pBuf;
// 	for (int i = 0; i < sz; i++, p++) {
// 		v = (m * i + c);
// 		if (HOp.bRemoveWaveness) {
// 			if (fabs(*p - v) < 1.f) *p -= float(v); else *p = 0;
// 		}
// 		else { if (fabs(*p - v) > 1.f) *p = float(v); }
// 	}
// 	delete[]pBuf2;
// }
//
// void IMGL::SImgStrip::Level(float* pBuf, size_t sz) {
// 	MTH::CLSFit LS;
// 	double v, m, c;
// 	float* p = pBuf;
// 	p = pBuf;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if (*p) LS.Add(float(i), *p);
// 	}
// 	LS.Get(m, c);
// 	p = pBuf;
// 	for (int i = 0; i < sz; i++, p++) {
// 		v = (m * i + c);
// 		if (*p) *p -= float(v);
// 		else *p = float(v);
// 	}
// }
//
// void IMGL::SImgStrip::LevelAve(float* pBuf, size_t sz) {
// 	MTH::CLSFit LS;
// 	float sum = 0;
// 	float* p = pBuf;
// 	p = pBuf;
// 	for (int i = 0; i < sz; i++, p++) {
// 		sum += *p;
// 	}
// 	sum /= float(sz);
// 	p = pBuf;
// 	for (int i = 0; i < sz; i++, p++) {
// 		*p -= sum;
// 	}
// }

// float* IMGL::SImgStrip::GetFringeData(IMGL::SFrings& F0, CHNL nChan) {
// 	int sz = int(F0.F1.F.size()); if (sz < 1) return nullptr;
// 	float* pbuf = new float[sz]; if (!pbuf) return nullptr;
// 	float* p = pbuf;
// 	SFrP4* pF = &F0.F1.F[0];
// 	for (int i = 0; i < sz; i++, p++, pF++) {
// 		*p = pF->y[int(nChan)];
// 	}
// 	return pbuf;
// }

// float IMGL::SImgStrip::GetPShift_rad() {
// 	// unit: percent of pie [7/12/2021 FSM]
// 	switch (algo.algo) {
// 	case EALGO::STEP3A: return 2.f / 3.f; break;
// 	case EALGO::STEP3B: return 1.f / 2.f; break;
// 	case EALGO::STEP3C: return 1.f / 2.f; break;
// 	case EALGO::STEP3E: return 1.f / 2.f; break;
// 	case EALGO::STEP3NV:return 1.f / 3.f; break;
// 	case EALGO::STEP4A:
// 	case EALGO::STEP4C:
// 	case EALGO::STEP4NV:return 1.f / 2.f; break;
// 	case EALGO::STEP5A:
// 	case EALGO::STEP5B:
// 	case EALGO::STEP5C:
// 	case EALGO::STEP5NV:return 1.f / 2.f; break;
// 	case EALGO::STEP6A: return 1.f / 3.f; break;
// 	case EALGO::STEP6B: return 1.f / 2.f; break;
// 	case EALGO::STEP7A:
// 	case EALGO::STEP7B: return 1.f / 2.f; break;
// 	case EALGO::STEP8A: return 1.f / 2.f; break;
// 	default: ASSERT(0); return 0; break;
// 	}
// }

// void IMGL::SImgStrip::GaussFilter(IMGL::SFrings& F0, short wdw, CHNL nChan) {
// 	int sz = int(F0.F1.F.size()); if (sz < 1) return;
// 	CMath M;
// 	float* K1 = M.Gaussian(wdw);
// 	float* pdat = GetFringeData(F0, nChan); if (!pdat) return;
// 	M.MKAver(pdat, sz, K1, wdw / 2);
// 	float* p = pdat;
// 	SFrP4* pF = &F0.F1.F[0];
// 	for (int i = 0; i < sz; i++, p++, pF++) {
// 		pF->y[int(nChan)] = *p;
// 	}
// 	delete[]K1; delete[]pdat;
// }

// void IMGL::SImgStrip::LevelGauss(float* pBuf, size_t sz, short wdw) {
// 	if (!pBuf) return; if (sz < 1) return;
// 	CMath M;
// 	float* K1 = M.Gaussian(wdw);
// 	M.MKAver(pBuf, int(sz), K1, wdw / 2);
// 	delete[]K1;
// }
//
// void IMGL::SImgStrip::LevelStep(float* pBuf, size_t sz) {
// 	float* pBuf2 = new float[sz]; if (!pBuf2) return;
// 	memcpy(pBuf2, pBuf, sz * sizeof(float));
// 	MTH::CLSFit LS;
// 	double v, m, c;
// 	float ave = 0;
// 	float* p = pBuf;
// 	int t = 0;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if (*p) { ave += *p; t++; }
// 	}
// 	ave /= float(t);
// 	p = pBuf2;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if (*p && (*p < ave)) LS.Add(float(i), *p);
// 	}
// 	LS.Get(m, c);
// 	p = pBuf;
// 	for (int i = 0; i < sz; i++, p++) {
// 		v = (m * i + c);
// 		if (*p == 0) *p = float(v);
// 		if (*p) { *p -= float(v); }
// 	}
// 	delete[]pBuf2;
// }
//
// void IMGL::SImgStrip::StatSmooth(float* pBufY, size_t sz) {
// 	MTH::CLSFit LS;
// 	float* p = pBufY;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if (*p) { LS.Add(float(i), *p); }
// 	}
// 	double m, c;
// 	LS.Get(m, c); LS.Reset();
//
// 	SStat S;
// 	S.CalcStat(pBufY, int(sz));
//
// 	float ave = S.Ave;
// 	float lmt = S.Stdev * 0.75f;
// 	float* pBufX = new float[sz];
// 	p = pBufY;
// 	float* q = pBufX;
// 	for (UINT i = 0; i < sz; i++, p++, q++) {
// 		*q = float(i);
// 		if (fabs(*p - (m * i + c)) > lmt) *p = 0;
// 	}
// 	tk::spline s;
// 	s.set_points(pBufX, pBufY, int(sz));
// 	p = pBufY;
// 	for (int i = 0; i < sz; i++, p++) {
// 		*p = float(s(i));
// 	}
// 	delete[]pBufX;
// }

// bool IMGL::SImgStrip::Despike(SPkPt* pBuf, int sz) {
// 	if (!pBuf || (sz < 1)) return false;
// 	float* p1 = new float[sz], * p2 = new float[sz];
//
// 	SPkPt* p;
// 	float aver = 0, * q, * r;
// 	p = (pBuf + 1); q = p1;
// 	// compute gradient [12/11/2020 FSM]
// 	for (int i = 1; i < sz - 1; i++, p++, q++) {
// 		*q = float(fabs(((p - 1)->y - (p + 1)->y))); // take gradient [12/11/2020 FSM]
// 		aver += *q;
// 	}
// 	aver /= float(sz - 2);
// 	float thres = aver / 2.0f;
// 	// remove spike [12/11/2020 FSM]
// 	p = (pBuf + 1); q = p1;
// 	for (int i = 1; i < sz - 1; i++, p++, q++) {
// 		if (*q < thres) continue;
// 		p->y = 0; // edge found [12/11/2020 FSM]
// 	}
// 	// interpolate all out of range values [12/11/2020 FSM]
// 	p = (pBuf + 1); q = p1; r = p2;
// 	int N = 0;
// 	for (int i = 1; i < sz - 1; i++, p++) {
// 		if (p->y) {
// 			*(r++) = float(i); *(q++) = p->y; N++;
// 		}
// 	}
// 	if (N > 2) {
// 		tk::spline s;
// 		s.set_points(p2, p1, N);
// 		p = pBuf + 1;
// 		for (int i = 1; i < sz - 1; i++, p++) {
// 			p->y = float(s(i));
// 		}
// 		pBuf->y = (pBuf + 1)->y;
// 		(pBuf + sz - 1)->y = (pBuf + sz - 2)->y;
// 	}
// 	delete[]p1; delete[]p2;
// 	return true;
// }

// float* IMGL::SImgStrip::SplineExpand(float* buf, int sz, int nsz) {
// 	// caller must  [12/17/2020 FSM]
// 	if (sz == nsz) return buf;
// 	float* bufY = new float[nsz]; if (!bufY) return NULL;
// 	float sf = sz / float(nsz);
// 	tk::spline s;
// 	s.set_points(buf, sz);
// 	float* p = bufY;
// 	for (int i = 0; i < nsz; i++, p++) {
// 		*p = float(s(i * sf));
// 	}
// 	delete[]buf;
// 	return bufY;
// }

// void IMGL::SImgStrip::Reconstruct(IMGL::CIM16& Im16_um) {
// 	if (Im16_um.IsNull()) return;
// 	int wd, ht; Im16_um.GetDim(wd, ht);
// 	IMGL::CIM16 ImX(wd, ht);
// 	IMGL::CIM16 ImY(wd, ht);
// 	if (ImX.IsNull() || ImX.IsNull()) return;
// 	int szwd = wd * sizeof(float);
// 	int szht = ht * sizeof(float);
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		float* bufX = new float[wd];
// 		float* bufY = new float[wd];
// 		if (bufX && bufY) {
// 			float* pPx = Im16_um.GetPixelAddress(0, y);
// 			int N = 0;
// 			float* pX = bufX, * pY = bufY;
// 			for (int i = 0; i < wd; i++, pPx++) {
// 				if (*pPx) {
// 					*(pX++) = float(i); *(pY++) = *pPx; N++;
// 				}
// 			}
// 			if (N > 2) {
// 				tk::spline s;
// 				s.set_points(bufX, bufY, N);
// 				float* pO = ImX.GetPixelAddress(0, y);
// 				for (int x = 0; x < wd; x++, pO++) {
// 					*pO = float(s(x));
// 				}
// 			}
// 		}
// 		if (bufX) delete[]bufX; if (bufY) delete[]bufY;
// 	}
// #pragma omp parallel for
// 	for (int x = 0; x < wd; x++) {
// 		float* bufX = new float[ht];
// 		float* bufY = new float[ht];
// 		if (bufX && bufY) {
// 			float* pX = bufX, * pY = bufY;
// 			int N = 0;
// 			for (int y = 0; y < ht; y++) {
// 				float pPx = Im16_um.GetPixel(x, y);
// 				if (pPx) {
// 					*(pX++) = float(y); *(pY++) = pPx; N++;
// 				}
// 			}
// 			if (N > 2) {
// 				tk::spline s;
// 				s.set_points(bufX, bufY, N);
// 				for (int y = 0; y < ht; y++) {
// 					ImY.SetPixel(x, y, float(s(y)));
// 				}
// 			}
// 		}
// 		if (bufX) delete[]bufX; if (bufY) delete[]bufY;
// 	}
// 	Im16_um.SetBuffer(ImX, ImY); // replace original image [12/12/2020 FSM]
// }

// void IMGL::SImgStrip::CreateStrips_um(float xStart_um, float xStop_um, int nFrames, int wd, int ht) {
// 	DeallocAll();
// 	float sf = (xStop_um - xStart_um) / float(nFrames);
// 	for (int i = 0; i < nFrames; i++) {
// 		IMGL::SImg* Im = new IMGL::SImg; if (!Im) return;
// 		Im->PzPos_um = xStart_um + i * sf;
// 		Im->Im.Create(wd, ht, 24);
// 		if (Im->Im.IsNull()) { delete Im; return; }
// 		Add(Im);
// 	}
// }

// CString IMGL::SImgStrip::CalculateRoughness(SPeakArray* pPkA) {
// 	// find average [12/22/2020 FSM]
// 	double sum = 0, sum2 = 0;
// 	int sz = pPkA->size(); if (sz < 1) return CString();
// 	SPkPt* pPks = pPkA->GetPeak(); if (!pPks) return CString();
// 	for (int i = 0; i < sz; i++, pPks++) {
// 		sum += pPks->y;
// 	}
// 	float ave = float(sum / float(sz));
// 	pPks = pPkA->GetPeak();
// 	sum = 0;
// 	sum2 = 0;
// 	float Rp = pPks->y - ave, Rv = Rp;
// 	for (int i = 0; i < sz; i++, pPks++) {
// 		double v = pPks->y - ave;
// 		if (v > 0) {
// 			if (v > Rp) { Rp = float(v); }
// 		}
// 		else if (v < 0) {
// 			if (v < Rv) { Rv = float(v); }
// 		}
// 		sum += fabs(v);
// 		sum2 += (v * v);
// 	}
// 	float Ra = float(sum / float(sz));
// 	float Rq = float(sqrt(sum2 / float(sz)));
// 	CString str;
// 	str.Format(
// 		L"Result (nm):\nRa = %.1f\nRq = %.1f\nRp = %.1f\nRv = %.1f\nRz = %.1f",
// 		Ra, Rq, Rp, Rv, Rp - Rv);
// 	return str;
// }

// CString IMGL::SImgStrip::CalculateStepHeight(SPeakArray* pPkA) {
// 	return CString();
// }

float IMGL::SImgStrip::GetZRange_um() {
	int sz = int(Imgs.size()); if (sz < 2) return 0;
	return Imgs[sz - 1]->PzPos_um - Imgs[0]->PzPos_um;
}

float IMGL::SImgStrip::GetStepSize_nm() {
	int nI = nIdx;
	int sz = int(Imgs.size()); if (sz < 1) return 0;
	if ((nI - 2) < 0) return 0;
	if ((nI + 2) >= sz) return 0;
	float dStep_nm = 0;
	dStep_nm += (Imgs[nI - 1]->PzPos_um - Imgs[nI - 2]->PzPos_um) * 1e3f;
	dStep_nm += (Imgs[nI]->PzPos_um - Imgs[nI - 1]->PzPos_um) * 1e3f;
	dStep_nm += (Imgs[nI + 1]->PzPos_um - Imgs[nI]->PzPos_um) * 1e3f;
	dStep_nm += (Imgs[nI + 2]->PzPos_um - Imgs[nI + 1]->PzPos_um) * 1e3f;
	return dStep_nm / 4.f;
}

float IMGL::SImgStrip::Get2PieStepDist_nm() {
	int sz = int(Imgs.size()); if (sz < 2) return 0;
	double sum_um = 0;
	for (int i = 1; i < sz; i++) {
		sum_um += (Imgs[i]->PzPos_um - Imgs[i - 1]->PzPos_um);
	}
	return float(sum_um / float(sz - 1)) * 1e3f;
}

float IMGL::SImgStrip::Interpolate(float x0, float x, float x1, float y0, float y1) {
	return (x - x0) / (x1 - x0) * (y1 - y0) + y0;
}

// float IMGL::SImgStrip::SplineI(int x, int y, int n, float fPos, CHNL nChan) {
// 	tk::spline SP;
// 	SImg* p;
// 	n--;
// 	p = Imgs[n++]; SP.add_points(p->PzPos_um, p->Im.GetIntensity(x, y, nChan));
// 	p = Imgs[n++]; SP.add_points(p->PzPos_um, p->Im.GetIntensity(x, y, nChan));
// 	p = Imgs[n++]; SP.add_points(p->PzPos_um, p->Im.GetIntensity(x, y, nChan));
// 	p = Imgs[n]; SP.add_points(p->PzPos_um, p->Im.GetIntensity(x, y, nChan));
// 	SP.compute();
// 	return float(SP(fPos));
// }

void IMGL::SImgStrip::GetIntenV2(int x, int y, float* fI, short N, eCH nChan, std::vector<SFrP1>& vP) {
	for (int i = 0; i < N; i++) {
		int v = vP[i].i;
		fI[i] = Interpolate(Imgs[v]->PzPos_um, vP[i].x_um, Imgs[v + 1]->PzPos_um,
			float(Imgs[v]->GetInten(x, y, nChan)), float(Imgs[v + 1]->GetInten(x, y, nChan)));
	}
}

// BOOL IMGL::SImgStrip::GetIntensity_nm(int x, int y, float Step_nm, int idx, float* fI, short N, CHNL nChan) {
// 	short N1 = N / 2;
// 	short st = 0, sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	float iPos_um, fPos_um;
// 	if (N % 2) { iPos_um = Imgs[idx]->PzPos_um; }
// 	else { iPos_um = Imgs[idx]->PzPos_um - (Step_nm * 1e-3f) / 2.f; }
// 	//////////////////////////////////////////////////////////////////////////
// 	// seek to far left most position [3/2/2021 FSM]
// 	short c = 0;
// 	fPos_um = iPos_um - N1 * (Step_nm * 1e-3f);
// 	for (int i = idx; i >= 0; i--) {
// 		SImg* pI = Imgs[i];
// 		if (pI->PzPos_um < fPos_um) {
// 			fI[c++] = Interpolate(pI->PzPos_um, fPos_um, Imgs[i + 1]->PzPos_um,
// 				float(pI->GetInten(x, y, nChan)), float(Imgs[i + 1]->GetInten(x, y, nChan)));
// 			st = i + 1;
// 			break;
// 		}
// 	}
// 	if (st == 0) return FALSE;
// 	// find all remaining Is to the right [3/28/2021 FSM]
// 	iPos_um = fPos_um;
// 	for (int i = 1; i < N; i++) {
// 		fPos_um = iPos_um + i * (Step_nm * 1e-3f);
// 		for (int j = st; j < sz; j++) {
// 			if (Imgs[j]->PzPos_um > fPos_um) {
// 				fI[c++] = Interpolate(Imgs[j - 1]->PzPos_um, fPos_um, Imgs[j]->PzPos_um,
// 					float(Imgs[j - 1]->GetInten(x, y, nChan)), float(Imgs[j]->GetInten(x, y, nChan)));
// 				st = j + 1;
// 				break;
// 			}
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	if (c == N) return TRUE;
// 	return FALSE;
// }

// float IMGL::SImgStrip::GetInten3v1_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan) {
// 	fI[0] = fI[1] = fI[2] = 0;
// 	int sz = int(Imgs.size()); if ((idx < 0) || (idx >= sz)) return 0;
// 	fI[1] = float(Imgs[idx]->GetInten(x, y, nChan));
// 	int n = idx - 1;
// 	float nPos_um = Imgs[idx]->PzPos_um - Step_nm * 1e-3f;
// 	for (int i = 0; i < idx; i++, n--) {
// 		if (Imgs[n]->PzPos_um < nPos_um) {
// 			fI[0] = Interpolate(Imgs[n]->PzPos_um, nPos_um, Imgs[n + 1]->PzPos_um,
// 				float(Imgs[n]->GetInten(x, y, nChan)), float(Imgs[n + 1]->GetInten(x, y, nChan)));
// 			break;
// 		}
// 	}
// 	n = idx + 1;
// 	nPos_um = Imgs[idx]->PzPos_um + Step_nm * 1e-3f;
// 	for (int i = idx + 1; i < sz; i++, n++) {
// 		if (Imgs[n]->PzPos_um > nPos_um) {
// 			fI[2] = Interpolate(Imgs[n - 1]->PzPos_um, nPos_um, Imgs[n]->PzPos_um,
// 				float(Imgs[n - 1]->GetInten(x, y, nChan)), float(Imgs[n]->GetInten(x, y, nChan)));
// 			break;
// 		}
// 	}
// 	return 1.0f;
// }
//
// BOOL IMGL::SImgStrip::GetInten3_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan) {
// 	fI[0] = fI[1] = fI[2] = 0;
// 	int sz = int(Imgs.size()); if ((idx < 0) || (idx >= sz)) return 0;
// 	short c = 0;
// 	short st = idx + 1;
// 	fI[c++] = float(Imgs[idx]->GetInten(x, y, nChan));
// 	for (int j = 1; j < 3; j++) {
// 		float nPos_um = Imgs[idx]->PzPos_um + j * (Step_nm * 1e-3f);
// 		for (int i = st; i < sz; i++) {
// 			SImg* pI = Imgs[i];
// 			if (pI->PzPos_um > nPos_um) {
// 				fI[c++] = Interpolate(Imgs[i - 1]->PzPos_um, nPos_um, pI->PzPos_um,
// 					float(Imgs[i - 1]->GetInten(x, y, nChan)), float(pI->GetInten(x, y, nChan)));
// 				st = i;
// 				break;
// 			}
// 		}
// 	}
// 	if (c < 3) return FALSE;
// 	return TRUE;
// }
//
// float IMGL::SImgStrip::GetInten4v1_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan) {
// 	fI[0] = fI[1] = fI[2] = 0;
// 	int sz = int(Imgs.size()); if ((idx < 0) || (idx >= sz)) return 0;
// 	fI[1] = float(Imgs[idx]->GetInten(x, y, nChan));
// 	int n = idx - 1;
// 	float nPos_um = Imgs[idx]->PzPos_um - Step_nm * 1e-3f;
// 	for (int i = 0; i < idx; i++, n--) {
// 		if (Imgs[n]->PzPos_um < nPos_um) {
// 			fI[0] = Interpolate(Imgs[n]->PzPos_um, nPos_um, Imgs[n + 1]->PzPos_um,
// 				float(Imgs[n]->GetInten(x, y, nChan)), float(Imgs[n + 1]->GetInten(x, y, nChan)));
// 			break;
// 		}
// 	}
// 	int c = 2;
// 	n = idx + 1;
// 	for (int j = 1; j <= 2; j++) {
// 		float nPos_um = Imgs[idx]->PzPos_um + j * Step_nm * 1e-3f;
// 		for (int i = idx + 1; i < sz; i++, n++) {
// 			if (Imgs[n]->PzPos_um > nPos_um) {
// 				fI[c++] = Interpolate(Imgs[n - 1]->PzPos_um, nPos_um, Imgs[n]->PzPos_um,
// 					float(Imgs[n - 1]->GetInten(x, y, nChan)), float(Imgs[n]->GetInten(x, y, nChan)));
// 				break;
// 			}
// 		}
// 	}
// 	return 1.0f;
// }
//
// float IMGL::SImgStrip::GetInten4_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan) {
// 	fI[0] = fI[1] = fI[2] = fI[3] = 0;
// 	int sz = int(Imgs.size()); if ((idx < 0) || (idx >= sz)) return 0;
// 	short c = 0;
// 	short st = idx + 1;
// 	fI[c++] = float(Imgs[idx]->GetInten(x, y, nChan));
// 	for (int j = 1; j < 4; j++) {
// 		float nPos_um = Imgs[idx]->PzPos_um + j * Step_nm * 1e-3f;
// 		for (int i = st; i < sz; i++) {
// 			SImg* pI = Imgs[i];
// 			if (pI->PzPos_um > nPos_um) {
// 				fI[c++] = Interpolate(Imgs[i - 1]->PzPos_um, nPos_um, pI->PzPos_um,
// 					float(Imgs[i - 1]->GetInten(x, y, nChan)), float(pI->GetInten(x, y, nChan)));
// 				st = i;
// 				break;
// 			}
// 		}
// 	}
// 	if (c < 4) return FALSE;
// 	return TRUE;
// }
//
// float IMGL::SImgStrip::GetInten5_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan) {
// 	fI[0] = fI[1] = fI[2] = 0;
// 	int sz = int(Imgs.size()); if ((idx < 0) || (idx >= sz)) return 0;
// 	fI[2] = float(Imgs[idx]->GetInten(x, y, nChan));
// 	int c = 1;
// 	int n = idx - 1;
// 	for (int j = 1; j <= 2; j++) {
// 		float nPos_um = Imgs[idx]->PzPos_um - j * Step_nm * 1e-3f;
// 		for (int i = 0; i < idx; i++, n--) {
// 			if (Imgs[n]->PzPos_um < nPos_um) {
// 				fI[c--] = Interpolate(Imgs[n]->PzPos_um, nPos_um, Imgs[n + 1]->PzPos_um,
// 					float(Imgs[n]->GetInten(x, y, nChan)), float(Imgs[n + 1]->GetInten(x, y, nChan)));
// 				break;
// 			}
// 		}
// 	}
// 	c = 3;
// 	n = idx + 1;
// 	for (int j = 1; j <= 2; j++) {
// 		float nPos_um = Imgs[idx]->PzPos_um + j * Step_nm * 1e-3f;
// 		for (int i = idx + 1; i < sz; i++, n++) {
// 			if (Imgs[n]->PzPos_um > nPos_um) {
// 				fI[c++] = Interpolate(Imgs[n - 1]->PzPos_um, nPos_um, Imgs[n]->PzPos_um,
// 					float(Imgs[n - 1]->GetInten(x, y, nChan)), float(Imgs[n]->GetInten(x, y, nChan)));
// 				break;
// 			}
// 		}
// 	}
//
// 	return 1.0f;
// }
//
// void IMGL::SImgStrip::Resampling(int x, int y, std::vector<SPointf>& FSpec, CHNL nChan) {
// 	FSpec.clear();
// 	float sum = 0;
// 	int sz = int(Imgs.size()); if (sz < 1) return;
// 	SPointf* pP = new SPointf[sz];
// 	SPointf* p = pP;
// 	for (int i = 0; i < sz; i++, p++) {
// 		SImg* pI = Imgs[i];
// 		p->x_um = pI->PzPos_um;
// 		sum += (p->y = float(pI->GetInten(x, y, nChan)));
// 	}
// 	sum /= float(sz);
// 	SPoint2 Max; Max.y = -(FLT_MAX - 1);
// 	p = pP;
// 	float x0_um = -1;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if (p->x_um <= x0_um) { p->x_um = -1; continue; }
// 		x0_um = p->x_um;
// 		float y0 = float(fabs(p->y) - sum);
// 		if (y0 > Max.y) { Max.y = y0; Max.x_um = p->x_um; }
// 		p->y = y0;
// 	}
// 	tk::spline s; s.clear(100);
// 	float st_um = Max.x_um - (1.325f * 0.280f);
// 	float ed_um = Max.x_um + (1.325f * 0.280f);
// 	p = pP; x0_um = -1;
// 	for (int i = 0; i < sz; i++, p++) {
// 		if ((p->x_um >= st_um) && (p->x_um <= ed_um)) {
// 			if (p->x_um <= x0_um) { p->x_um = -1; continue; }
// 			x0_um = p->x_um;
// 			s.add_points(p->x_um, p->y);
// 		}
// 	}
// 	delete[]pP;
// 	//////////////////////////////////////////////////////////////////////////
// 	s.compute();
// 	FILE* fp = fopen("C:\\TEMP\\FSPEC.CSV", "wb");
// 	int n = 100;
// 	float sf_um = (ed_um - st_um) / float(n);
// 	SPointf Q;
// 	for (int i = 0; i < n; i++) {
// 		float v = st_um + sf_um * i;
// 		Q.x_um = v; Q.y = float(s(v));
// 		FSpec.push_back(Q);
// 		if (fp) fprintf(fp, "%f,%f\n", FSpec[i].x_um, FSpec[i].y);
// 	}
// 	if (fp) fclose(fp);
// }

float IMGL::SImgStrip::GetStripRange_um() {
	int sz = int(Imgs.size()); if (sz < 1) return 0;
	return Imgs[sz - 1]->PzPos_um - Imgs[0]->PzPos_um;
}

// float* IMGL::SImgStrip::GetBufferX(SPointf* pPt, int& sz) {
// 	// caller must deallocate buffer [12/13/2020 FSM]
// 	if ((pPt->y < 0) || (pPt->y > 1.f)) { ASSERT(0); return nullptr; }
// 	if (Im16_um.IsNull()) return nullptr;
// 	int wd, ht; Im16_um.GetDim(wd, ht);
// 	sz = wd;
// 	float* pBuf = new float[sz]; if (!pBuf) return nullptr;
// 	memcpy(pBuf, Im16_um.GetPixelAddress(0, int(pPt->y * ht)), wd * sizeof(float));
// 	return pBuf;
// }

// float* IMGL::SImgStrip::GetHeightX(SPointf* pPt, float* pBuf, size_t sz) {
// 	if (Im16_um.IsNull() || !pBuf) return NULL;
// 	int wd, ht; Im16_um.GetDim(wd, ht);
// 	if (sz < wd) { ASSERT(0); return NULL; }
// 	memcpy(pBuf, Im16_um.GetPixelAddress(0, int(pPt->y * ht)), sz * sizeof(float));
// 	return pBuf;
// }

// float* IMGL::SImgStrip::GetBufferY(SPointf* pPt, int& sz) {
// 	// caller must deallocate buffer [12/13/2020 FSM]
// 	if (Im16_um.IsNull()) return NULL;
// 	sz = Im16_um.GetHeight(); if (sz < 1) return NULL;
// 	float* pBuf = new float[sz]; if (!pBuf) return NULL;
// 	float* p = pBuf;
// 	int x = int(sz * pPt->x_um);
// 	for (int y = 0; y < sz; y++) {
// 		*(p++) = Im16_um.GetPixel(x, y);
// 	}
// 	return pBuf;
// }

// float* IMGL::SImgStrip::GetHeightY(SPointf* pPt, float* pBuf, size_t sz) {
// 	if (Im16_um.IsNull() || !pBuf) return NULL;
// 	int wd, ht; Im16_um.GetDim(wd, ht);
// 	if (sz < ht) return NULL;
// 	float* p = pBuf;
// 	int x = int(wd * pPt->x_um);
// 	for (int y = 0; y < ht; y++) {
// 		*(p++) = Im16_um.GetPixel(x, y);
// 	}
// 	return pBuf;
// }

// BOOL IMGL::SImgStrip::GenHeightMap_CG_CSI_MAXP(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// Simple method base on max peak position [10/2/2021 yuenl]
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	//////////////////////////////////////////////////////////////////////////
// 	float VisLim = Rcp.VisLim;
// 	int N = 3;
// 	int nQW = 1, iChan = int(nChan), ivChan = iChan + int(CHNL::WVISCH);
// 	IMGL::SFrD D; D.SetV(this);
// 	IMGL::SROI R = R = UI.GetROI(this); BOOL bPChg = Rcp.GetPChg();
// 	//////////////////////////////////////////////////////////////////////////
// 	// CICache initialization [10/19/2021 yuenl]
// 	CICache IC(sz);
// 	//{
// 	//	SFrin4 Fx;
// 	//	CollectStripST(Fx, wd / 2, ht / 2, IMGL::SROI(sz), nChan);
// 	//	for (int i = 0; i < sz; i++) {
// 	//		Fx.GetIntensityRange(D, nChan, i, IC.vI[i], IC.vI[i]);
// 	//	}
// 	//}
// 	//////////////////////////////////////////////////////////////////////////
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		int idx; SFrin4 Fx;
// 		float* pPx = Im16_um.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) {
// 			if (CollectStripST(Fx, x, y, R, nChan)) {
// 				if (Rcp.b2Peaks) {
// 					idx = Find2PeaksIndex(Fx, nChan, bPChg);
// 					if (idx < 1) { *pPx = BADDATA; continue; }
// 				}
// 				else { if (bPChg) idx = Fx.St.Imin[iChan]; else idx = Fx.St.Imax[iChan]; }
// 				if (Fx.FindPhaseIndex(D, nChan, idx, bPChg, IC.vI[idx], IC.vI[idx])) { // to obtain visibility [10/20/2021 yuenl]
// 					if (Fx.F[idx].y[ivChan] > VisLim) {
// 						int N = 5; double sumx = 0, sumx2 = 0, sumxy = 0, sumy = 0;
// 						if (!((idx < (N / 2 + 1)) || (idx >= sz - N / 2 - 1))) {
// 							IMGL::SFrP4* pFc1 = &Fx.F[idx - N / 2 - 1], * pFc2 = pFc1 + 2, * pFc0 = pFc1 + 1;
// 							for (int i = 0; i < N; i++, pFc0++, pFc1++, pFc2++) {
// 								float x = pFc0->x_um, y = (pFc2->y[iChan] - pFc1->y[iChan]) / (pFc2->x_um - pFc1->x_um);
// 								sumx += x; sumx2 += x * x; sumxy += x * y; sumy += y;
// 							}
// 							float v = float(-(sumy * sumx2 - sumx * sumxy) / (N * sumxy - sumx * sumy));
// 							*pPx = (100.f - v); continue;
// 						}
// 					}
// 				}
// 			}
// 			*pPx = BADDATA;
// 		}
// 	}
// 	return TRUE;
// }

// int IMGL::SImgStrip::Find2PeaksIndex(IMGL::SFrin4& F1, CHNL nChan, BOOL bPChg) {
// 	int idx = -1;
// 	IMGL::SFrin4 F2, F3;
// 	F1.Shrink(nChan, F2, bPChg);
// 	F2.Shrink(nChan, F3, bPChg);
// 	F3.RejectPeaks(nChan, Con.CalibCtWLen_nm[int(nChan)] * 5e-3f / 2.f, bPChg);
// 	if (F3.F.size() > Rcp.PK2.nPeak) {
// 		idx = F3.F[Rcp.PK2.nPeak].i;
// 	}
// 	return idx;
// }

// BOOL IMGL::SImgStrip::GenHeightMap_CSI_CG(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// CG method base on max peak position [10/2/2021 yuenl]
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	//////////////////////////////////////////////////////////////////////////
// 	int nQW = 1, iChan = int(nChan);
// 	IMGL::SFrD D; D.SetV(this);
// 	IMGL::SROI R = R = UI.GetROI(this); BOOL bPChg = Rcp.GetPChg();
// 	//////////////////////////////////////////////////////////////////////////
// 	int ivChan = iChan + int(CHNL::WVISCH);
// 	//////////////////////////////////////////////////////////////////////////
//
// 	//////////////////////////////////////////////////////////////////////////
// 	// CICache initialization [10/19/2021 yuenl]
// 	CICache IC(sz);
// 	{
// 		SFrin4 Fx;
// 		CollectStripST(Fx, wd / 2, ht / 2, IMGL::SROI(sz), nChan);
// 		for (int i = 0; i < sz; i++) {
// 			Fx.GetIntensityRange(D, nChan, i, IC.vI[i], IC.vI[i]);
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		int idx; SFrin4 F;
// 		//CICache Cz; Cz.Alloc(R.Size());
// 		float* pPx = Im16_um.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) {
// 			if (CollectStripST(F, x, y, R, nChan)) {
// 				if (Rcp.b2Peaks) { idx = Find2PeaksIndex(F, nChan, bPChg); }
// 				else { if (bPChg) idx = F.St.Imin[iChan]; else idx = F.St.Imax[iChan]; }
// 				if (idx >= 0) {
// 					float v = F.FindCG_um(nChan, idx, nQW, IC.vI[idx], R);
// 					if (v != BADDATA) { *pPx = (100.f - v); continue; }
// 				}
// 			}
// 			*pPx = BADDATA;
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenHeightMap_CG_CSI_MAXV(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// CG method base on max visibility position [10/2/2021 yuenl]
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	//////////////////////////////////////////////////////////////////////////
// 	int iChan = int(nChan);
// 	int ivChan = iChan + int(CHNL::WVISCH);
// 	IMGL::SFrD D; D.SetV(this);
// 	IMGL::SROI R = R = UI.GetROI(this); BOOL bPChg = Rcp.GetPChg();
// 	//////////////////////////////////////////////////////////////////////////
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SFrin4 F; //MTH::CLSFit Fit; Fit.D.reserve(2 * wdw + 1);
// 		IMGL::SROI RR = R;
// 		CZCache Cz; Cz.Alloc(RR.Size());
// 		float* pPx = Im16_um.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) { // 1290, 70, 32
// 			if (CollectStripST(F, x, y, RR, nChan)) {
// 				int idx;
// 				if (Rcp.b2Peaks) { idx = Find2PeaksIndex(F, nChan, bPChg); }
// 				else { if (!bPChg) idx = F.St.Imax[iChan]; else idx = F.St.Imin[iChan]; }
// 				if ((idx < 0) || (idx >= sz)) { *pPx = BADDATA; continue; }
// 				RR.SetA(idx - 11, idx + 11, sz);
// 				F.FindPhaseLine(D, nChan, bPChg, RR); // to obtain visibility [10/12/2021 FSM]
// 				RR.SetA(RR.i1 + 2, RR.i2 - 2, sz); // shrink region [10/27/2021 yuenl]
// 				F.Smooth(CHNL(ivChan), 1, 2, RR); // required by F.FindMaxVisPosMaxP_um() [10/28/2021 yuenl]
// 				F.Stats(CHNL(ivChan), RR); // required by F.FindMaxVisPosMaxP_um() [10/28/2021 yuenl]
// 				idx = F.St.Imax[ivChan];
// 				if ((idx < 0) || (idx >= sz)) { *pPx = BADDATA; continue; }
// 				if (F.F[idx].y[ivChan] > Rcp.VisLim) {
// 					//*pPx = Fx.FindMaxVisPosCG_um(nChan, RR);
// 					*pPx = F.FindMaxVisPosMaxP_um(nChan, idx);
// 					continue;
// 				}
// 				//if (idx >= 0) {
// 				//	float v = F.FindMaxVisFast(nChan, D, RR, idx, bPChg, Fit, wdw);
// 				//	if (v != BADDATA || (v >= 0)) { *pPx = (100.f - v); continue; }
// 				//}
// 			}
// 			*pPx = BADDATA;
// 		}
// 	}
// 	return TRUE;
// }

// void IMGL::SImgStrip::LinearizeX() {
// 	MTH::CLSFit Fit;
// 	int sz = int(Imgs.size());
// 	for (int i = 0; i < sz; i++) {
// 		Fit.Add(float(i), Imgs[i]->PzPos_um);
// 	}
// 	float m, c;
// 	Fit.Get(m, c);
// 	for (int i = 0; i < sz; i++) {
// 		Imgs[i]->PzPos_um = m * i + c;
// 	}
// }

// BOOL IMGL::SImgStrip::GenHeightMap_CSI_PSI_P(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	Im16_um.Create(wd, ht); if (Im16_um.IsNull()) return FALSE;
//
// 	int iChan = int(nChan);
// 	int ipChan = iChan + int(CHNL::WPHASECH);
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::SFrD D; D.SetV(this);
// 	//////////////////////////////////////////////////////////////////////////
//
// 	BOOL bPChg = Rcp.GetPChg();
// 	float dht_um = D.dht_um[iChan];
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	//////////////////////////////////////////////////////////////////////////
// 	// CICache initialization [10/19/2021 yuenl]
// 	CICache IC(sz);
// 	{
// 		SFrin4 Fx;
// 		CollectStripST(Fx, wd / 2, ht / 2, IMGL::SROI(sz), nChan);
// 		for (int i = 0; i < sz; i++) {
// 			Fx.GetIntensityRange(D, nChan, i, IC.vI[i], IC.vI[i]);
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
//
// 	//? Note: OMP produce slower code [6/21/2021 FSM]
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SIn O;
// 		int idx;
// 		SFrin4 F;
// 		float* pPx = Im16_um.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) { // 1290
// 			if (CollectStripST(F, x, y, R, nChan)) {
// 				if (Rcp.b2Peaks) { idx = Find2PeaksIndex(F, nChan, bPChg); }
// 				else { if (!bPChg) idx = F.St.Imax[iChan]; else idx = F.St.Imin[iChan]; }
// 				if (idx >= 0) {
// 					if (F.FindPhaseIndex(D, nChan, idx, bPChg, IC.vI[idx], O)) {
// 						SFrP4& Fc = F.F[idx];
// 						float v = Fc.x_um - (Fc.y[ipChan] * dht_um);
// 						if (v != BADDATA) {
// 							*pPx = (100.f - v); continue;
// 						}
// 					}
// 				}
// 			}
// 			*pPx = BADDATA;
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenHeightMap_CSI_PSI_V(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	Im16_um.Create(wd, ht); if (Im16_um.IsNull()) return FALSE;
//
// 	int iChan = int(nChan);
// 	int ipChan = iChan + int(CHNL::WPHASECH);
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::SFrD D; D.SetV(this);
// 	//////////////////////////////////////////////////////////////////////////
//
// 	BOOL bPChg = Rcp.GetPChg();
// 	float dht_um = D.dht_um[iChan];
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	//////////////////////////////////////////////////////////////////////////
// 	// CICache initialization [10/19/2021 yuenl]
// 	CICache IC(sz);
// 	{
// 		SFrin4 Fx;
// 		CollectStripST(Fx, wd / 2, ht / 2, IMGL::SROI(sz), nChan);
// 		for (int i = 0; i < sz; i++) {
// 			Fx.GetIntensityRange(D, nChan, i, IC.vI[i], IC.vI[i]);
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
//
// 	//? Note: OMP produce slower code [6/21/2021 FSM]
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SIn O;
// 		int idx;
// 		SFrin4 F;
// 		float* pPx = Im16_um.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) { // 1290
// 			if (CollectStripST(F, x, y, R, nChan)) {
// 				if (Rcp.b2Peaks) { idx = Find2PeaksIndex(F, nChan, bPChg); }
// 				else { if (!bPChg) idx = F.St.Imax[iChan]; else idx = F.St.Imin[iChan]; }
// 				if (idx >= 0) {
// 					if (F.FindPhaseIndex(D, nChan, idx, bPChg, IC.vI[idx], O)) {
// 						SFrP4& Fc = F.F[idx];
// 						float v = Fc.x_um - (Fc.y[ipChan] * dht_um);
// 						if (v != BADDATA) {
// 							*pPx = (100.f - v); continue;
// 						}
// 					}
// 				}
// 			}
// 			*pPx = BADDATA;
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenHeightMap_WL_PSI(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	ClearIm16_rad(); //! phase maps are not use here  [7/8/2021 FSM]
// 	//return GenWLPSI1Map(nChan);
// 	//return GenWLPSI2Map(nChan);
// 	return GenMap_WL_PSI3(Im16_um, nChan);
// }
//
// BOOL IMGL::SImgStrip::GenHeightMap_WL_PSI(IMGL::CIM16& Im16_um, CHNL nChan, int nPeak) {
// 	ClearIm16_rad(); //! phase maps are not use here  [7/8/2021 FSM]
// 	return GenMap_WL_PSI4(Im16_um, nChan, nPeak);
// }
//
// BOOL IMGL::SImgStrip::GenHeightMap_TWL1_PSI(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	int iChan = int(nChan);
// 	//////////////////////////////////////////////////////////////////////////
// 	SFrD D; D.SetV(this); bool bPChg = Rcp.GetPChg();
// 	//////////////////////////////////////////////////////////////////////////
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		int idx;
// 		SFrin4 F;
// 		IMGL::SROI R; R = UI.GetROI(this);
// 		//SIn I; I.bSpdUp = false;
// 		float* pPix1 = Im16_um.GetPixelAddress(0, y); if (!pPix1) continue;
// 		for (int x = 0; x < wd; x++, pPix1++) { // 1290
// 			if (CollectStripST(F, x, y, R, CHNL::REDCH, CHNL::GRNCH)) {
// 				if (!bPChg) idx = F.St.Imax[int(CHNL::GRNCH)]; else idx = F.St.Imin[int(CHNL::GRNCH)];
// 				F.PSMPhaseRetrieveEx(D, CHNL::REDCH, CHNL::GRNCH, idx, bPChg);
// 				F.FindPhasePzPointE(idx);
// 				IMGL::SFrP4& Fc = F.F[idx];
// 				float hc = Con.EqvCtWLenRG_nm * Fc.y[int(CHNL::EPHASECH)] / PIE4;
// 				int Ng = int(2 * hc / Con.CalibCtWLen_nm[int(CHNL::GRNCH)]);
// 				float fg = Fc.y[int(CHNL::GPHASECH)] / PIE2;
// 				Fc.y[int(CHNL::ERSLCH)] = hc;
// 				if (Fc.y[int(CHNL::GVISCH)] > Rcp.VisLim) {
// 					*pPix1 = 50.f + ((Ng + fg) * Con.CalibCtWLen_nm[int(CHNL::GRNCH)] / 2) * 1e-3f; // unit: um [10/12/2021 yuenl]
// 				}
// 				else *pPix1 = BADDATA;
// 			}
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenHeightMap_TWL2_PSI(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	int iChan = int(nChan);
// 	//////////////////////////////////////////////////////////////////////////
// 	SFrD D; D.SetV(this); bool bPChg = Rcp.GetPChg();
// 	//////////////////////////////////////////////////////////////////////////
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		int idx;
// 		SFrin4 F;
// 		IMGL::SROI R; R = UI.GetROI(this);
// 		float* pPix1 = Im16_um.GetPixelAddress(0, y); if (!pPix1) continue;
// 		for (int x = 0; x < wd; x++, pPix1++) { // 1290
// 			if (CollectStripST(F, x, y, R, CHNL::REDCH, CHNL::BLUCH)) {
// 				if (!bPChg) idx = F.St.Imax[int(CHNL::GRNCH)]; else idx = F.St.Imin[int(CHNL::GRNCH)];
// 				F.PSMPhaseRetrieveEx(D, CHNL::REDCH, CHNL::BLUCH, idx, bPChg);
// 				F.FindPhasePzPointE(idx);
// 				float rwnum = 1 / Con.CalibCtWLen_nm[int(CHNL::REDCH)];
// 				float gwnum = 1 / Con.CalibCtWLen_nm[int(CHNL::GRNCH)];
// 				float bwnum = 1 / Con.CalibCtWLen_nm[int(CHNL::BLUCH)];
// 				IMGL::SFrP4& Fc = F.F[idx];
// 				MTH::CLSFit Fit;
// 				Fit.Add(rwnum, Fc.y[int(CHNL::RPHASECH)]);
// 				Fit.Add(gwnum, Fc.y[int(CHNL::GPHASECH)]);
// 				Fit.Add(bwnum, Fc.y[int(CHNL::BPHASECH)]);
// 				float za = float(Fit.GetM() / PIE4);
// 				float y = Fc.y[int(CHNL::GRNCH)];
// 				Fc.y[int(CHNL::EPHASECH)] = y - PIE2 * int((y - PIE4 * gwnum * za) / PIE2); // corrected green phase [9/30/2021 yuenl]
// 				Fc.y[int(CHNL::ERSLCH)] = PIE2 * int((y - PIE4 * gwnum * za) / PIE2); // fringe order [9/30/2021 yuenl]
// 				if (Fc.y[int(CHNL::GVISCH)] > Rcp.VisLim) {
// 					*pPix1 = 50.f + Fc.y[int(CHNL::ERSLCH)] / (PIE4 * gwnum);
// 				}
// 				else *pPix1 = BADDATA;
// 			}
// 		}
// 	}
// 	return TRUE;
// }

// BOOL IMGL::SImgStrip::GenZMap(IMGL::CIM16& Im16_um, CHNL nChan, int pos, BOOL bHorz) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	if (bHorz) {
// 		//if (!Im16_um.Create(wd, 2 * sz)) return FALSE;
// 		if (!Im16_um.Create(wd, sz)) return FALSE;
// 	}
// 	else {
// 		//if (!Im16_um.Create(ht, 2 * sz)) return FALSE;
// 		if (!Im16_um.Create(ht, sz)) return FALSE;
// 	}
//
// 	int iChan = int(nChan);
//
// 	if (bHorz) {
// 		SImg** pIm = &Imgs[0];
// 		for (int z = 0; z < sz; z++, pIm++) {
// 			float* pPix1 = Im16_um.GetPixelAddress(0, z);
// 			//float* pPix1 = Im16_um.GetPixelAddress(0, 2 * z);
// 			//float* pPix2 = Im16_um.GetPixelAddress(0, 2 * z + 1);
// 			for (int x = 0; x < wd; x++, pPix1++/*, pPix2++*/) {
// 				COLORREF cr = (*pIm)->Im.GetPixel(x, pos);
// 				switch (nChan) {
// 				case CHNL::WHTCH:
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 					* pPix1 = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f;
// #else
// 					* pPix1 = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f;
// #endif
// #else
// #ifdef HALFWHITE
// 					* pPix1 = (GetGValue(cr) + GetBValue(cr)) / 2.f;
// #else
// 					* pPix1 = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f;
// #endif
// #endif
// 					//* pPix2 = *pPix1;
// 					break;
// 				case CHNL::REDCH:
// 					*pPix1 = GetRValue(cr);
// 					//*pPix2 =*pPix1;
// 					break;
// 				case CHNL::GRNCH:
// 					*pPix1 = GetGValue(cr);
// 					//*pPix2 =*pPix1;
// 					break;
// 				case CHNL::BLUCH:
// 					*pPix1 = GetBValue(cr);
// 					//*pPix2 =*pPix1;
// 					break;
// 				default: ASSERT(!"Invalid CHNL"); break;
// 				}
// 			}
// 		}
// 	}
// 	else {
// 		SImg** pIm = &Imgs[0];
// 		for (int z = 0; z < sz; z++, pIm++) {
// 			float* pPix1 = Im16_um.GetPixelAddress(0, z);
// 			//float* pPix1 = Im16_um.GetPixelAddress(0, 2 * z);
// 			//float* pPix2 = Im16_um.GetPixelAddress(0, 2 * z + 1);
// 			for (int y = 0; y < ht; y++, pPix1++/*, pPix2++*/) {
// 				COLORREF cr = (*pIm)->Im.GetPixel(pos, y);
// 				switch (nChan) {
// 				case CHNL::WHTCH:
// #ifdef FULLWHITE
// #ifdef HALFWHITE
// 					* pPix1 = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f;
// #else
// 					* pPix1 = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f;
// #endif
// #else
// #ifdef HALFWHITE
// 					* pPix1 = (GetGValue(cr) + GetBValue(cr)) / 2.f;
// #else
// 					* pPix1 = (2 * GetGValue(cr) + GetBValue(cr)) / 3.f;
// #endif
// #endif
// 					//*pPix2 = *pPix1;
// 					break;
// 				case CHNL::REDCH:
// 					*pPix1 = GetRValue(cr);
// 					//*pPix2 = *pPix1;
// 					break;
// 				case CHNL::GRNCH:
// 					*pPix1 = GetGValue(cr);
// 					//*pPix2 = *pPix1;
// 					break;
// 				case CHNL::BLUCH:
// 					*pPix1 = GetBValue(cr);
// 					//*pPix2 = *pPix1;
// 					break;
// 				default: ASSERT(!"Invalid CHNL"); break;
// 				}
// 			}
// 		}
// 	}
// 	return 0;
// }

// BOOL IMGL::SImgStrip::GenPhaseMap(IMGL::CIM16& Im16_um, CHNL nChan, int idx) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	//int w = wd / UI.nXYR, h = ht / UI.nXYR;
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::SFrD D; D.SetV(this);
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	//////////////////////////////////////////////////////////////////////////
// 	int nStp = GetNStep();
// 	std::vector<IMGL::SFrP1> Pos;
// 	float lamdaPz_nm = Con.CWLen_nm[int(nChan)] / 2.f; // half a wavelength == PIE [3/11/2021 FSM]
// 	float piePz_nm = lamdaPz_nm / 2.f;
// 	float stepPz_nm = piePz_nm * GetPShift_rad();
// 	float PShift_rad = (stepPz_nm / piePz_nm) * PIE;
// 	if (!CalcStartStops_nm(stepPz_nm, idx, nStp, Pos)) return FALSE;
// 	//////////////////////////////////////////////////////////////////////////
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		float* pPix = Im16_um.GetPixelAddress(0, y);
// 		switch (algo.algo) {
// 		case EALGO::STEP3NV:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase3NV_rad(x, y, PShift_rad, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP3A:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase3a(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP3B:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase3b(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP3C:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase3c(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP3E:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase3e(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP4A:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase4a(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP4C:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase4c(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP4NV:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase4NV(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP5A:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase5a(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP5B:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase5b(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP5C:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase5c(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP5NV:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase5NV_rad(x, y, PShift_rad, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP6A:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase6a(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP6B:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase6b(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP7A:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase7a(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP7B:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase7b(x, y, nChan, Pos).phase; }
// 			break;
// 		case EALGO::STEP8A:
// 			for (int x = 0; x < wd; x++, pPix++) { *pPix = GetPhase8a(x, y, nChan, Pos).phase; }
// 			break;
// 		default: ASSERT(0); break;
// 		}
// 	}
//
// #if 0
// 	int ipChan = int(nChan) + int(CHNL::WPHASECH);
// 	//#pragma omp parallel for
// 	for (int y = 0; y < ht; y += UI.nXYR) {
// 		SIn I; SFrin4 F; SFrP4 Fc;
// 		float* pPx = Im16_rad.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x += UI.nXYR, pPx++) { // 1290
// 			if (CollectStripRGB(F, x, y, R, I)) {
// 				SFrP4& Fc = F.F[nIdx];
// 				if (F.FindPhasePzPoint(D, nChan, Fc, nIdx, R, I)) {
// 					*pPx = Fc.y[ipChan];
// 					if (*pPx != BADDATA) {
// 						if (*pPx < 0) *pPx += PIE2;
// 						continue;
// 					}
// 				}
// 			}
// 			*pPx = 0;
// 		}
// 	}
// #endif
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenPhaseMap(IMGL::CIM16& Im16Red, IMGL::CIM16& Im16Grn, IMGL::CIM16& Im16Blu, IMGL::CIM16& Im16D) {
// 	int wd, ht; Im16Red.GetDim(wd, ht);
// 	if (!Im16D.Create(wd, ht)) return FALSE;
// 	if (!Im16_rad.Create(wd, ht)) return FALSE;
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		float* pDes1 = Im16D.GetPixelAddress(0, y);
// 		float* pSr1 = Im16Red.GetPixelAddress(0, y);
// 		float* pSr2 = Im16Grn.GetPixelAddress(0, y);
// 		float* pSr3 = Im16Blu.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pDes1++, pSr1++, pSr2++, pSr3++) { // 1290
// 			if ((*pSr1 == BADDATA) || (*pSr2 == BADDATA) || (*pSr3 == BADDATA)) {
// 				*pDes1 = 0; continue;
// 			}
//
// #if 1
// 			if (*pSr1 < 0) *pSr1 += PIE2;
// 			if (*pSr2 < 0) *pSr2 += PIE2;
// 			*pDes1 = *pSr1 - *pSr2; if (*pDes1 < 0) *pDes1 += PIE2;
// #endif
// #if 0
// 			if (*pSr1 < 0) *pSr1 += PIE2;
// 			if (*pSr2 < 0) *pSr2 += PIE2;
// 			if (*pSr3 < 0) *pSr3 += PIE2;
// 			float P1 = *pSr1 - *pSr2; if (P1 < 0) P1 += PIE2;
// 			float P2 = *pSr2 - *pSr3; if (P2 < 0) P2 += PIE2;
// 			float P3 = P1 - P2; if (P3 < 0) P3 += PIE2;
// 			*pDes1 = P3;
// #endif
// 		}
// 	}
// 	Im16D.Smooth(3);
// #if 1
// 	float N = Con.EqvCtWLenRG_nm / Con.CalibCtWLen_nm[int(CHNL::GRNCH)];
// 	for (int y = 0; y < ht; y++) {
// 		float* pDes1 = Im16D.GetPixelAddress(0, y);
// 		float* pDes2 = Im16_um.GetPixelAddress(0, y);
// 		float* pSr2 = Im16Grn.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pDes1++, pDes2++, pSr2++) {
// 			if (*pDes1 == BADDATA) { *pDes2 = 0; continue; }
// 			int m = int(N * (*pDes1) / PIE2 - (*pSr2) / PIE2);
// 			*pDes2 = PIE2 * m + *pSr2;
// 		}
// 	}
// #endif
// #if 0
// 	float N = Con.EqvCtWLenRGB_nm / Con.CalibCtWLen_nm[int(CHNL::GRNCH)];
// 	for (int y = 0; y < ht; y++) {
// 		float* pDes1 = Im16D.GetPixelAddress(0, y);
// 		float* pDes2 = Im16_rad.GetPixelAddress(0, y);
// 		float* pDes3 = Im16_um.GetPixelAddress(0, y);
// 		float* pSr2 = Im16Grn.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pDes1++, pDes2++, pDes3++, pSr2++) { // 1290
// 			if ((*pSr2 == BADDATA) || (*pDes1 == BADDATA)) {
// 				*pDes2 = 0; *pDes3 = 0; continue;
// 			}
// 			int m = int(N * (*pDes1) / PIE2 - *pSr2 / PIE2);
// 			*pDes2 = PIE2 * m;
// 			*pDes3 = PIE2 * m + *pSr2;
// 		}
// 	}
// #endif
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenPhaseMapEqv(IMGL::CIM16& Im16_rad, CHNL fr, CHNL to) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	//int w = wd / UI.nXYR, h = ht / UI.nXYR;
// 	if (!Im16_rad.Create(wd, ht)) return FALSE;
// 	//ClearIm16_rad(); //! phase map is not use  [7/8/2021 FSM]
//
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::SFrD D; D.SetV(this); BOOL bPChg = Rcp.GetPChg();
// 	SROI R; R = UI.GetROI(this);
// 	//////////////////////////////////////////////////////////////////////////
// 	int ipfrChan = int(fr) + int(CHNL::WPHASECH);
// 	int iptoChan = int(to) + int(CHNL::WPHASECH);
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SFrin4 F;
// 		SIn Ifr, Ito, O;
// 		float* pPx = Im16_rad.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) { // 1290
// 			if (CollectStrip(F, x, y, R, fr, to)) {
// 				F.FindPhaseIndex(D, fr, nIdx, bPChg, Ifr, O);
// 				F.FindPhaseIndex(D, to, nIdx, bPChg, Ito, O);
// 				SFrP4& Fc = F.F[nIdx];
// 				float p = Fc.y[ipfrChan], q = Fc.y[iptoChan];
// 				if ((p == BADDATA) || (q == BADDATA)) {
// 					*pPx = BADDATA; continue;
// 				}
// 				*pPx = -(p - q);
// 				if (*pPx < 0) *pPx += PIE2;
// 			}
// 			*pPx = 0;
// 		}
// 	}
// 	return TRUE;
// }

void IMGL::SImgStrip::ClearImAll() {
	ImBG.Destroy();
	Im16_um.Destroy();
	ClearIm16_rad();
	ClearIm16ABCD();
}

void IMGL::SImgStrip::ClearIm16_rad() {
	Im16_rad.Destroy();
	Im16Eq_rad.Destroy();
}

void IMGL::SImgStrip::ClearIm16ABCD() {
	Im16A.Destroy(); Im16B.Destroy();
	Im16C.Destroy(); Im16D.Destroy();
}

// BOOL IMGL::SImgStrip::GenMap_WL_PSI1(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// first revision of WLPSI [6/10/2021 FSM]
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
//
// 	int iChan = int(nChan);
// 	int wd, ht; GetDim(wd, ht);
// 	//int w = wd / UI.nXYR, h = ht / UI.nXYR;
// 	if (!Im16_um.IsNull()) Im16_um.Destroy();
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
// 	Im16_rad.Destroy(); //! phase map is not use here  [7/8/2021 FSM]
// 	Im16Eq_rad.Destroy();
//
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::SFrD D; D.SetV(this); bool bPChg = Rcp.GetPChg();
// 	IMGL::SROI R = UI.GetROI(this);
// 	sz = R.Size();
// 	//////////////////////////////////////////////////////////////////////////
//
// 	if (!CalcWLen(nChan)) return FALSE;
//
// 	float dht_um = D.dht_um[iChan] * 1e-3f;
// 	float lamdaPz_nm = Con.CWLen_nm[int(nChan)] / 2.f; // Phase shift step [3/30/2021 FSM]
// 	float piePz_nm = lamdaPz_nm / 2.f;
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		if (y >= ht) continue;
// 		int yy = y;
// 		float* pPix1 = Im16_um.GetPixelAddress(0, y);
// 		SIn I, O; SFrin4 F1; IMGL::SFrP1 F;
// 		for (int x = 0; x < wd; x++) { // 1290
// 			if (x >= wd) continue;
// 			if (CollectStrip(F1, x, y, R, nChan, nChan)) {
// 				F1.ZeroMean(nChan, R);
// 				F1.WLPSI1PointEx(nChan, F, D, bPChg, I, O);
// 				F.y *= dht_um;
// 				if (F.x_um) { *(pPix1++) = (100.f - F.x_um - F.y); }
// 				else { *(pPix1++) = 50.f; }
// 			}
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenMap_WL_PSI3(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// Second revision of WLPSI [6/10/2021 FSM]
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
//
// 	int wd, ht; GetDim(wd, ht);
// 	//int w = wd / UI.nXYR, h = ht / UI.nXYR;
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	int iChan = int(nChan);
// 	int ipChan = iChan + int(CHNL::WPHASECH);
// 	int ivChan = iChan + int(CHNL::WVISCH);
// 	//////////////////////////////////////////////////////////////////////////
// 	SFrD D; D.SetV(this); bool bPChg = Rcp.GetPChg();
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	sz = R.Size();
// 	D.nS = sz / 6; if (D.nS < 9) D.nS = 9;
// 	float dht_um = D.dht_um[iChan];
// 	//////////////////////////////////////////////////////////////////////////
// 	// CICache initialization [10/19/2021 yuenl]
// 	CICache IC(sz);
// 	{
// 		SFrin4 Fx;
// 		CollectStripST(Fx, wd / 2, ht / 2, IMGL::SROI(sz), nChan);
// 		for (int i = 0; i < sz; i++) {
// 			Fx.GetIntensityRange(D, nChan, i, IC.vI[i], IC.vI[i]);
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		int idx;
// 		SIn O;
// 		SFrin4 F;
// 		//IMGL::SROI R = D.R;
// 		float* pPix1 = Im16_um.GetPixelAddress(0, y); if (!pPix1) continue;
// 		for (int x = 0; x < wd; x++, pPix1++) { // 1290
// 			if (CollectStripST(F, x, y, R, nChan)) {
// 				if (!bPChg) idx = F.St.Imax[iChan]; else idx = F.St.Imin[iChan];
// 				if (F.FindPsi0XEx_um(D, nChan, idx, bPChg, 2, MTH::CLSFit(), IC.vI[idx], O)) {
// 					IMGL::SFrP4& Fc = F.F[idx];
// 					if (Fc.y[ivChan] > Rcp.VisLim) {
// 						*pPix1 = 100.f - (Fc.x_um - (Fc.y[ipChan] * dht_um));
// 						//!  Adjust ROI to NPhaseShift / 2 + 3 [6/10/2021 FSM]
// 						continue;
// 					}
// 				}
// 				*pPix1 = BADDATA; //R = D.R;
// 			}
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenMap_WL_PSI4(IMGL::CIM16& Im16_um, CHNL nChan, int nPeak) {
// 	// multi-peaks version [6/10/2021 FSM]
// 	if (nPeak < 0) return FALSE;
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
//
// 	int wd, ht; GetDim(wd, ht);
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
//
// 	int iChan = int(nChan);
// 	int ipChan = iChan + int(CHNL::WPHASECH);
// 	int ivChan = iChan + int(CHNL::WVISCH);
// 	//////////////////////////////////////////////////////////////////////////
// 	SFrD D; D.SetV(this); bool bPChg = Rcp.GetPChg();
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	sz = R.Size();
// 	D.nS = sz / 6; if (D.nS < 9) D.nS = 9;
// 	float dht_um = D.dht_um[iChan];
// 	//////////////////////////////////////////////////////////////////////////
//
// 	//////////////////////////////////////////////////////////////////////////
// 	// CICache initialization [10/19/2021 yuenl]
// 	CICache IC(sz);
// 	{
// 		SFrin4 Fx;
// 		CollectStripST(Fx, wd / 2, ht / 2, IMGL::SROI(sz), nChan);
// 		for (int i = 0; i < sz; i++) {
// 			Fx.GetIntensityRange(D, nChan, i, IC.vI[i], IC.vI[i]);
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
//
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SFrin4 F, F1, F2;
// 		SIn O;
// 		//IMGL::SROI R = D.R;
// 		float* pPix1 = Im16_um.GetPixelAddress(0, y); if (!pPix1) continue;
// 		for (int x = 0; x < wd; x++, pPix1++) { // 1290
// 			if (CollectStripST(F, x, y, R, nChan)) {
// 				F.Shrink(nChan, F1, bPChg);
// 				F1.Shrink(nChan, F2, bPChg);
// 				F2.RejectPeaks(nChan, Con.CalibCtWLen_nm[int(nChan)] * 5e-3f / 2.f, bPChg);
// 				if (nPeak < R.Size()) {
// 					int idx = F2.F[nPeak].i;
// 					if (F.FindPsi0XEx_um(D, nChan, idx, bPChg, 2, MTH::CLSFit(), IC.vI[idx], O)) {
// 						IMGL::SFrP4& Fc = F.F[idx];
// 						if (Fc.y[ivChan] > Rcp.VisLim) {
// 							*pPix1 = (100.f - Fc.x_um - (Fc.y[ipChan] * dht_um));
// 							//!  Adjust ROI to NPhaseShift / 2 + 3 [6/10/2021 FSM]
// 							continue;
// 						}
// 					}
// 				}
// 				*pPix1 = BADDATA; //R = D.R;
// 			}
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenMap_WL_PSI2(IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// Third revision of WLPSI [6/10/2021 FSM]
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
//
// 	int iChan = int(nChan);
// 	int wd, ht; GetDim(wd, ht);
//
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
// 	Im16_rad.Destroy();
// 	Im16Eq_rad.Destroy();
//
// 	//////////////////////////////////////////////////////////////////////////
// 	SFrD D; D.SetV(this); bool bPChg = Rcp.GetPChg();
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	sz = R.Size();
// 	//////////////////////////////////////////////////////////////////////////
//
// 	if (!CalcWLen(nChan)) return FALSE;
//
// 	// NOTE OMP generate slower code [6/10/2021 FSM]
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SFrin4 F1; IMGL::SFrP1 F;
// 		SFrD D1 = D; //? mandatory  [8/9/2021 FSM]
// 		//IMGL::SROI R1 = D1.R; //? mandatory  [8/9/2021 FSM]
// 		int yy = y;
// 		float* pPix1 = Im16_um.GetPixelAddress(0, y); if (!pPix1) continue;
// 		for (int x = 0; x < wd; x++, pPix1++) { // 1290
// 			if (CollectStrip(F1, x, y, R, nChan, nChan)) {
// 				if (F1.WLPSI2PointEx(nChan, bPChg, F, D)) {
// 					*pPix1 = (100.f - F.x_um); // convert to nm [6/15/2021 FSM]
// 				}
// 				else {
// 					// retry with full range [6/10/2021 FSM]
// 					//R1 = D1.R;
// 					if (CollectStrip(F1, x, y, R, nChan, nChan)) {
// 						if (F1.WLPSI2PointEx(nChan, bPChg, F, D1)) {
// 							*pPix1 = (100.f - F.x_um); // convert to nm [6/15/2021 FSM]
// 						}
// 						else { /*R1 = D1.R;*/ *pPix1 = 0; pPix1++; continue; }
// 					}
// 					else { /*R1 = D1.R;*/ *pPix1 = 0; pPix1++; continue; }
// 				}
// 				// TODO: Adjust 25 to NPhaseShift / 2 + 3 [6/10/2021 FSM]
// 				//if (!bPChg) {
// 				//	int Im = F1.St.Imax[int(nChan)];
// 				//	R1.i1 = Im - (D1.nS / 2 + 5); R1.i2 = Im + (D1.nS / 2 + 5);
// 				//}
// 				//else {
// 				//	int Im = F1.St.Imin[int(nChan)];
// 				//	R1.i1 = Im - (D1.nS / 2 + 5); R1.i2 = Im + (D1.nS / 2 + 5);
// 				//}
// 				//if (R1.i1 < 0) {
// 				//	R1.i1 = 0; R1.i2 = D1.nS + 10;
// 				//}
// 				//if (R1.i2 >= sz) {
// 				//	R1.i2 = sz - 1; R1.i1 = R1.i2 - D1.nS + 10;
// 				//	if (R1.i1 < 0) R1.i1 = 0;
// 				//}
// 			}
// 		}
// 	}
// 	return TRUE;
// }

IMGL::CIM* IMGL::SImgStrip::GetIm8() { return &Im16_um.Im8; }

IMGL::CIM* IMGL::SImgStrip::GetImg(int idx) {
	if ((idx < 0) || (idx >= int(Imgs.size()))) return nullptr;
	return &Imgs[idx]->Im;
}

void IMGL::SImgStrip::Add(SImg* pIM) {
	Imgs.push_back(pIM);
}

void IMGL::SImgStrip::DeallocAll() {
	size_t sz = Imgs.size();
	for (size_t i = 0; i < sz; i++) {
		delete Imgs[i];
	}
	Imgs.clear();
}

void IMGL::SImgStrip::Dump(char* fname, float* Buf, short sz) {
	FILE* fp = fopen(fname, "wb"); if (!fp) return;
	fprintf(fp, "X,Y\n");
	for (int i = 0; i < sz; i++, Buf++) {
		fprintf(fp, "%d,%.4f\n", i + 1, *Buf);
	}
	fclose(fp);
}

// void IMGL::SImgStrip::DumpFringeC(char* fname, SFrings& F0, int sz, CHNL nChan) {
// 	FILE* fp = fopen(fname, "wb"); if (!fp) return;
// 	fprintf(fp, "#,value\n");
// 	SFrP4* pF = &F0.F1.F[0];
// 	for (size_t i = 0; i < sz; i++, pF++) {
// 		fprintf(fp, "%.4f,%.2f\n", pF->x_um, pF->y[int(nChan)]);
// 	}
// 	fclose(fp);
// }
//
// void IMGL::SImgStrip::DumpFringes(char* fname, SFrings& F0, int sz) {
// 	FILE* fp = fopen(fname, "wb"); if (!fp) return;
// 	fprintf(fp, "#,White, Red, Green, Blue\n");
// 	int TyW = int(CHNL::WHTCH), TyR = int(CHNL::REDCH);
// 	int TyG = int(CHNL::GRNCH), TyB = int(CHNL::BLUCH);
// 	SFrP4* pF = &F0.F1.F[0];
// 	for (size_t i = 0; i < sz; i++, pF++) {
// 		fprintf(fp, "%.4f,%.2f,%.2f,%.2f,%.2f\n",
// 			pF->x_um, pF->y[TyW], pF->y[TyR], pF->y[TyG], pF->y[TyB]);
// 	}
// 	fclose(fp);
// }

BOOL IMGL::SImgStrip::GetDim(int& wd, int& ht) {
	int bpp; wd = 0; ht = 0;
	if (Imgs.size() < 1) return FALSE;
	Imgs[0]->Im.GetDim(wd, ht, bpp);
	return TRUE;
}

//BOOL IMGL::SImgStrip::GenHeightRef() {
//	if (Im16_um.IsNull()) return FALSE;
//	IMGL::CIM16 Im16B_um;
//	if (Im16_um.WavinessH(Im16B_um)) {
//		if (Im16B_um.WavinessV(UI.ImRef)) {
//			UI.ImRef.Level(); UI.ImRef.ZeroShift(); return TRUE;
//		}
//	}
//	return FALSE;
//}

//BOOL IMGL::SImgStrip::GenHeightRefH() {
//	if (Im16_um.IsNull()) return FALSE;
//	IMGL::CIM16 Im16B_um;
//	if (Im16_um.WavinessH(Im16B_um)) {
//		UI.ImRef.Level(); UI.ImRef.ZeroShift(); return TRUE;
//	}
//	return FALSE;
//}

//BOOL IMGL::SImgStrip::GenHeightRefV() {
//	if (Im16_um.IsNull()) return FALSE;
//	IMGL::CIM16 Im16B_um;
//	if (Im16_um.WavinessV(Im16B_um)) {
//		UI.ImRef.Level(); UI.ImRef.ZeroShift(); return TRUE;
//	}
//	return FALSE;
//}

// BOOL IMGL::SImgStrip::BGP(IMGL::CIM16& Im16_rad, IMGL::CIM16& Im16_um) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int wd, ht; GetDim(wd, ht);
// 	//int w = wd / UI.nXYR, h = ht / UI.nXYR;
// 	Im16_um.Create(wd, ht); if (Im16_um.IsNull()) return FALSE;
// 	ClearIm16_rad(); //! phase map is not use  [7/8/2021 FSM]
//
// 	int iChan = int(CHNL::GRNCH);
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::SFrD D; D.SetV(this);
// 	IMGL::SROI R; R = UI.GetROI(this);
// 	sz = R.Size();
// 	//////////////////////////////////////////////////////////////////////////
//
// 	BOOL bPChg = Rcp.GetPChg();
// 	//IMGL::SROI R = D.R;
//
// 	//////////////////////////////////////////////////////////////////////////
// 	// Preparation [8/30/2021 FSM]
// 	SFrin4 F;
// 	if (!CollectStripST(F, wd / 2, ht / 2, R, CHNL::GRNCH, CHNL::BLUCH)) return FALSE;
// 	sz = int(F.F.size()); //? sz recalculated due to R changes number of data points [8/30/2021 FSM]
// 	int dw = sz / 10;
// 	if (dw < 5) dw = 5;
// 	//////////////////////////////////////////////////////////////////////////
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		SIn I; SFrin4 F;
// 		IMGL::SFrD D1 = D;
// 		int idx;
// 		float v = 0;
// 		float* pPx = Im16_um.GetPixelAddress(0, y);
// 		for (int x = 0; x < wd; x++, pPx++) {
// 			if (CollectStripST(F, x, y, R, CHNL::REDCH, CHNL::GRNCH)) {
// 				if (!bPChg) idx = F.St.Imax[iChan]; else idx = F.St.Imin[iChan];
// 				if (idx < 0) idx = 0; if (idx >= sz) idx = sz - 1;
// 				int st = idx - dw, ed = idx + dw;
// 				if (st < 0) st = 0; if (ed < 0) ed = st + 5; if (ed >= sz) ed = sz - 1;
// 				F.FindPhaseLineE(D, CHNL::REDCH, CHNL::GRNCH, st, ed, bPChg);
// 				F.UnwrapPhaseLine_rad(CHNL::RPHASECH, int(Imgs.size()) / 2, st, ed);
// 				F.UnwrapPhaseLine_rad(CHNL::GPHASECH, int(Imgs.size()) / 2, st, ed);
// 				F.FindBestIndexBG(st, ed, v);
// 				if (v != BADDATA) { *pPx = (100.f - v); continue; }
// 			}
// 			*pPx = BADDATA;
// 		}
// 	}
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::GenHeightPSIMap(int idx, IMGL::CIM16& Im16_rad, IMGL::CIM16& Im16_um, CHNL nChan) {
// 	errStr = "";
// 	// Calculate [2/21/2021 FSM]
// 	int sz = int(Imgs.size());
// 	if ((sz < 5) || (idx < 2) || (idx >= sz - 5)) {
// 		errStr = _T("<<Index range error>> "); Im16_um.Destroy(); Im16_rad.Destroy();  return FALSE;
// 	}
// 	int wd, ht; GetDim(wd, ht); Im16_rad.Create(wd, ht);
// 	if (Im16_rad.IsNull()) { errStr = _T("<<Memory alloc error>> "); Im16_um.Destroy(); Im16_rad.Destroy(); return FALSE; }
// 	//////////////////////////////////////////////////////////////////////////
// 	short sifAve = UI.sifAve; if (sifAve < 1) sifAve = 1;
// 	if (sifAve > 1) sifAve = (sz - 4);
// 	idx -= (sifAve / 2);
// 	if (idx < 2) idx = 2;
// 	int st = idx; int ed = st + sifAve;
// 	if ((ed + 2) >= sz) ed = sz - 2;
// 	sifAve = (ed - st);
// 	if (sifAve < 1) { return FALSE; }
// 	if (sifAve > 32) { sifAve = 32; }
// 	//////////////////////////////////////////////////////////////////////////
// 	IMGL::CIM16** ImPhMapAry = new IMGL::CIM16 * [sifAve]; // Phase map array [8/6/2021 FSM]
// 	for (int i = 0; i < sifAve; i++) {
// 		ImPhMapAry[i] = new IMGL::CIM16; ImPhMapAry[i]->Create(wd, ht);
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	CString err = _T("<<X range error>> ");
// 	//////////////////////////////////////////////////////////////////////////
// 	int nStp = GetNStep();
// 	float lamdaPz_nm = Con.CWLen_nm[int(nChan)] / 2.f; // half a wavelength == PIE [3/11/2021 FSM]
// 	float piePz_nm = lamdaPz_nm / 2.f;
// 	float stepPz_nm = piePz_nm * GetPShift_rad();
// 	float PShift_rad = (stepPz_nm / piePz_nm) * PIE;
// 	//////////////////////////////////////////////////////////////////////////
// 	std::vector<IMGL::SFrP1> vPos;
// 	for (int z = 0; z < sifAve; z++) {
// 		if (!CalcStartStops_nm(stepPz_nm, idx, nStp, vPos)) {
// 			idx++; if (idx >= sz) return FALSE;
// 			continue;
// 		}
// #pragma omp parallel for
// 		for (int y = 0; y < ht; y++) {
// 			IMGL::CIM16* Im16 = ImPhMapAry[z];
// 			float* pPix = Im16->GetPixelAddress(0, y);
// 			switch (algo.algo) {
// 			case EALGO::STEP3NV:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase3NV_rad(x, y, PShift_rad, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP3A:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase3a(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP3B:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase3b(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP3C:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase3c(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP3E:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase3e(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP4A:
// 				for (int x = 0; x < wd; x++) {
// 					Im16->AddPixel(x, y, GetPhase4a(x, y, nChan, vPos).phase);
// 				}
// 				break;
// 			case EALGO::STEP4C:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase4c(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP4NV:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase4NV(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP5A:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase5a(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP5B:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase5b(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP5C:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase5c(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP5NV:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase5NV_rad(x, y, PShift_rad, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP6A:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase6a(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP6B:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase6b(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP7A:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase7a(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP7B:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase7b(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			case EALGO::STEP8A:
// 				for (int x = 0; x < wd; x++, pPix++) {
// 					*pPix += GetPhase8a(x, y, nChan, vPos).phase;
// 				}
// 				break;
// 			default:ASSERT(0); break;
// 			}
// 		}
// 		idx++;
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	if (UI.bUnwrapHeight) {
// 		for (int z = 0; z < sifAve; z++) {
// 			ImPhMapAry[z]->UnwrapEx();
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	Im16_rad = *ImPhMapAry[0]; delete ImPhMapAry[0];
// 	for (int z = 1; z < sifAve; z++) {
// 		Im16_rad.Add(ImPhMapAry[z]); delete ImPhMapAry[z];
// 	}
// 	delete[] ImPhMapAry;
//
// 	if (sifAve > 1) Im16_rad.Devide(sifAve);
// 	//////////////////////////////////////////////////////////////////////////
// 	if (UI.bUnwrapHeight) {
// 		Im16_rad.UnwrapEx();
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	//! Convert Im16_rad to Im16_nm
// 	float dht_um = (Con.CWLen_nm[int(nChan)] * 1e-3f) / (4 * PIE);
// 	Im16_um.ConvertPhase2Height_um(dht_um, Im16_rad);// convert radian to micrometer [2/26/2021 FSM]
// 													 //////////////////////////////////////////////////////////////////////////
// 	return TRUE;
// }

// BOOL IMGL::SImgStrip::CSI(CHNL nChan) {
// 	if (GenHeightMap_CSI_PSI_P(Im16_um, UI.nChan)) return TRUE;
// 	return FALSE;
// }
//
// BOOL IMGL::SImgStrip::WLPSI(CHNL nChan) {
// 	if (GenHeightMap_WL_PSI(Im16_um, UI.nChan)) return TRUE;
// 	return FALSE;
// }
//
// BOOL IMGL::SImgStrip::FOM(int idx, IMGL::CIM16& Im16_um, CHNL nChan) {
// 	// No full sanity check [2/25/2021 FSM]
// 	errStr = "";
// 	// Calculate [2/21/2021 FSM]
// 	int sz = int(Imgs.size());
// 	if ((sz < 5) || (idx < 2) || (idx >= sz - 2)) {
// 		errStr = _T("<<Index range error>> ");  return FALSE;
// 	}
// 	int wd, ht; if (!GetDim(wd, ht)) return FALSE;
// 	if (!Im16_um.Create(wd, ht)) return FALSE;
// 	if (!Im16_rad.Create(wd, ht)) return FALSE;
// 	if (!Im16Eq_rad.Create(wd, ht)) return FALSE;
// 	//////////////////////////////////////////////////////////////////////////
// 	float lamdaPz_nm = Con.CWLen_nm[int(nChan)] / 2.f; // half a wavelength == PIE [3/11/2021 FSM]
// 	float piePz_nm = lamdaPz_nm / 2.f;
// 	float stepPz_nm;
// 	//CString err = _T("<<X range error>> ");
// 	stepPz_nm = IMGL::SFrin4::GetStepPz_nm(algo.algo, piePz_nm, GetZRange_um());
// 	float PShift_rad = (stepPz_nm / piePz_nm) * PIE;
// 	//////////////////////////////////////////////////////////////////////////
// 	std::vector<IMGL::SFrP1> Pos;
// 	CalcStartStops_nm(stepPz_nm, idx, GetNStep(), Pos);
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		GetPhaseLineRG(Im16_rad, Im16Eq_rad, nChan, y, wd, PShift_rad, Pos);
// 	}
// 	if (UI.bUnwrapHeight) {
// 		Im16_rad.UnwrapEx(); Im16Eq_rad.UnwrapEx();
// 	}
// 	// second pass : assemble into meaningful HeightMap [7/9/2021 FSM]
// 	float CWLch_nm = Con.CWLen_nm[int(nChan)];
// 	float CWLch2_nm = CWLch_nm / 2.f;
// 	float CWLeqPie2 = Con.EqvCtWLenRG_nm / PIE2;
// 	float CWLchPie2 = Con.CWLen_nm[int(nChan)] / PIE2;
// #pragma omp parallel for
// 	for (int y = 0; y < ht; y++) {
// 		for (int x = 0; x < wd; x++) {
// 			float rsltCh = Im16_rad.GetPixel(x, y) * CWLchPie2;
// 			float rsltEq = int(Im16Eq_rad.GetPixel(x, y) * CWLeqPie2 / CWLch_nm) * CWLch_nm;
// 			float v = (rsltCh - rsltEq);
// 			if (abs(v) > CWLch2_nm) { if (v < 0) rsltCh += CWLch_nm; else rsltCh -= CWLch_nm; }
// 			Im16_um.AddPixel(x, y, rsltCh); //? NOTE: Im16_um is in radian [8/6/2021 FSM]
// 		}
// 	}
// 	return TRUE;
// }

BOOL IMGL::SImgStrip::ValidateIdx() {
	if (nIdx < 0) nIdx = 0;
	if (nIdx >= int(Imgs.size())) nIdx = int(Imgs.size() - 1);
	return TRUE;
}

BOOL IMGL::SImgStrip::IdxValid() {
	if ((nIdx >= 0) && (nIdx < Imgs.size())) return TRUE;
	return FALSE;
}

// void IMGL::SImgStrip::SetIdxMaxPeak(CHNL nChan, BOOL bPChg) {
// 	int sz = int(Imgs.size()); if (sz < 1) return;
// 	int wd, ht; GetDim(wd, ht);
// 	int x = wd / 2, y = ht / 2;
// 	SFrin4 F1;
// 	int v, iChan = int(nChan);
// 	int max = -1, imax = -1;
// 	if (bPChg) { max = 256; }
// 	for (int i = 0; i < sz - 1; i++) {
// 		SImg* pI = Imgs[i];
// 		COLORREF cr = pI->GetPixRGB(x, y);
// 		switch (nChan) {
// 		case CHNL::WHTCH: v = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4; break;
// 		case CHNL::REDCH: v = GetRValue(cr); break;
// 		case CHNL::GRNCH: v = GetGValue(cr); break;
// 		case CHNL::BLUCH: v = GetBValue(cr); break;
// 		default: ASSERT(!"Invalid CHNL"); break;
// 		}
// 		if (bPChg) {
// 			if (v < max) {
// 				max = v; imax = i;
// 			}
// 		}
// 		else {
// 			if (v > max) {
// 				max = v; imax = i;
// 			}
// 		}
// 	}
// 	if (imax != -1) {
// 		nIdx = imax;
// 	}
// }
//
// int IMGL::SImgStrip::GetNStep() {
// 	switch (algo.algo) {
// 	case EALGO::STEP3A:
// 	case EALGO::STEP3B:
// 	case EALGO::STEP3C:
// 	case EALGO::STEP3E:
// 	case EALGO::STEP3NV: return 3; break;
// 	case EALGO::STEP4A:
// 	case EALGO::STEP4C:
// 	case EALGO::STEP4NV: return 4; break;
// 	case EALGO::STEP5A:
// 	case EALGO::STEP5B:
// 	case EALGO::STEP5C:
// 	case EALGO::STEP5NV: return 5; break;
// 	case EALGO::STEP6A:
// 	case EALGO::STEP6B:  return 6; break;
// 	case EALGO::STEP7A:
// 	case EALGO::STEP7B:  return 7; break;
// 	case EALGO::STEP8A:  return 8; break;
// 	default: ASSERT(0); break;
// 	}
// 	return 0;
// }

void IMGL::SImgStrip::Serialize(CArchive& ar) {
	USHORT magic = 3;
	if (ar.IsStoring()) {
		ar << magic;
		ar << NSlice;
		size_t sz = Imgs.size();
		ar << sz;
		for (size_t i = 0; i < sz; i++) {
			Imgs[i]->Serialize(ar);
		}
		ImBG.Serialize(ar);
	}
	else {
		ar >> magic;
		if (magic > 1) { ar >> NSlice; }
		else NSlice = 1;
		size_t sz;
		ar >> sz;
		if (sz > 0) DeallocAll();
		for (size_t i = 0; i < sz; i++) {
			SImg* pIm = new SImg;
			pIm->Serialize(ar);
			Imgs.push_back(pIm);
		}
		if (magic > 2) {
			ImBG.Serialize(ar);
		}
	}
}

// BOOL IMGL::SImgStrip::CalcStartStops_nm(float stepPz_nm, int idx, int N, std::vector<SFrP1>& vP) {
// 	vP.clear(); vP.reserve(N);
// 	short N1 = N / 2;
// 	short st = 0, sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	float iPos_um, fPos_um;
// 	if (idx < 0) idx = sz / 2; if (idx >= sz) idx = sz / 2;
// 	if (N % 2) iPos_um = Imgs[idx]->PzPos_um;
// 	else iPos_um = Imgs[idx]->PzPos_um - (stepPz_nm * 1e-3f) / 2.f;
// 	//////////////////////////////////////////////////////////////////////////
// 	// seek to far left most position [3/2/2021 FSM]
// 	SFrP1 F;
// 	short c = 0;
// 	fPos_um = iPos_um - N1 * stepPz_nm * 1e-3f;
// 	for (int i = idx; i >= 0; i--) {
// 		if (Imgs[i]->PzPos_um < fPos_um) {
// 			F.x_um = fPos_um; F.i = i;
// 			vP.push_back(F); st = i + 1; break;
// 		}
// 	}
// 	if (st == 0) return FALSE;
// 	// find all remaining Is to the right [3/28/2021 FSM]
// 	iPos_um = fPos_um;
// 	for (int i = 1; i < N; i++) {
// 		fPos_um = iPos_um + i * stepPz_nm * 1e-3f;
// 		for (int j = st; j < sz; j++) {
// 			if (Imgs[j]->PzPos_um > fPos_um) {
// 				F.x_um = fPos_um; F.i = j - 1;
// 				vP.push_back(F); st = j + 1; break;
// 			}
// 		}
// 	}
// 	//////////////////////////////////////////////////////////////////////////
// 	if (vP.size() == N) return TRUE;
// 	return FALSE;
// }

short IMGL::SImgStrip::GetWidth() {
	if (Imgs.size()) {
		return Imgs[0]->Im.GetWidth();
	}
	return 0;
}

short IMGL::SImgStrip::GetHeight() {
	if (Imgs.size()) {
		return Imgs[0]->Im.GetHeight();
	}
	return 0;
}

void IMGL::SImgStrip::Integrate() {
	if (NSlice < 2) return;
	int sz = int(Imgs.size()); if (sz < 1) return;
	int wd, ht; GetDim(wd, ht);
	// integrate buffer [2/24/2021 FSM]
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wd; x++) {
			for (int i = 0; i < sz; i += NSlice) {
				int R = 0, G = 0, B = 0;
				SImg* Im0 = Imgs[i];
				Im0->Im.RGBAddTo(x, y, R, G, B);
				for (int j = 1; j < NSlice; j++) {
					SImg* Im1 = Imgs[i + j];
					Im0->PzPos_um += Im1->PzPos_um;
					Im1->Im.RGBAddTo(x, y, R, G, B);
				}
				Im0->PzPos_um /= NSlice;
				Im0->Im.SetPixel(x, y, BYTE(R / NSlice), BYTE(G / NSlice), BYTE(B / NSlice));
			}
		}
	}
	// remove and delete excess from Imgs [2/24/2021 FSM]
	std::vector<SImg*>vIm;
	for (int i = 0; i < sz; i += NSlice) {
		vIm.push_back(Imgs[i]);
		for (int j = 1; j < NSlice; j++) {
			delete Imgs[i + j];
		}
	}
	Imgs.clear();
	NSlice = 1;
	Imgs = vIm; // pointer copy only [2/24/2021 FSM]
	nIdx = int(Imgs.size() / 2);
}

void IMGL::SImgStrip::Integrate(int NFrame) {
	if (NFrame > NSlice) return;
	if (NSlice < 2) return;
	if (NFrame < 1) NFrame = NSlice;
	int sz = int(Imgs.size()); if (sz < 1) return;
	int wd, ht; GetDim(wd, ht);
	// integrate buffer [2/24/2021 FSM]
#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		for (int x = 0; x < wd; x++) {
			for (int i = 0; i < sz; i += NSlice) {
				int R = 0, G = 0, B = 0;
				SImg* Im0 = Imgs[i];
				Im0->Im.RGBAddTo(x, y, R, G, B);
				for (int j = 1; j < NFrame; j++) {
					SImg* Im1 = Imgs[i + j];
					Im0->PzPos_um += Im1->PzPos_um;
					Im1->Im.RGBAddTo(x, y, R, G, B);
				}
				Im0->PzPos_um /= NFrame;
				Im0->Im.SetPixel(x, y, BYTE(R / NFrame), BYTE(G / NFrame), BYTE(B / NFrame));
			}
		}
	}
	// remove and delete excess from Imgs [2/24/2021 FSM]
	std::vector<SImg*>vIm;
	for (int i = 0; i < sz; i += NSlice) {
		vIm.push_back(Imgs[i]);
		for (int j = 1; j < NSlice; j++) {
			delete Imgs[i + j];
		}
	}
	Imgs.clear();
	NSlice = 1;
	Imgs = vIm; // pointer copy only [2/24/2021 FSM]
	nIdx = int(Imgs.size() / 2);
}

// int IMGL::SImgStrip::FindMaxPeakIdx(SFrin4& F1, CHNL nChan, int x, int y) {
// 	int sz = int(Imgs.size()); if (sz < 1) return 0;
// 	IMGL::SROI Rt(sz);
// 	if (CollectStripST(F1, x, y, Rt, nChan, nChan)) {
// 		return F1.St.Imax[int(nChan)];
// 	}
// 	return 0;
// }
//
// int IMGL::SImgStrip::FindMaxVisIdx(CHNL nChan, BOOL bPChg, SFrin4& F1, IMGL::SFrD& D, int x, int y) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	IMGL::SROI R(sz);
// 	int ivChan = int(nChan) + int(CHNL::WVISCH);
// 	if (CollectStripST(F1, x, y, R, nChan, nChan)) {
// 		F1.FindPhaseLine(D, nChan, bPChg, R);
// 		return F1.GetMaxIdx(CHNL(ivChan), R);
// 	}
// 	return 0;
// }
//
// BOOL IMGL::SImgStrip::FindBestROI(IMGL::SROI& R, CHNL nChan, float fPad) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
// 	int idx, padd, wd, ht; GetDim(wd, ht);
// 	int dx = wd / 32; int dy = ht / 32;
// 	dx = dy = 8;
// 	int imax = 0, imin = 0;
// 	float max = BADDATA, min = -BADDATA;
// 	//////////////////////////////////////////////////////////////////////////
// 	// initialization [9/28/2021 yuenl]
// 	SFrin4 F1;
// 	IMGL::SROI Rt(sz);
// 	if (CollectStrip(F1, wd / 2, ht / 2, Rt, nChan, nChan)) {
// 		//F1.ZeroMeanST(nChan, Rt);
// 		idx = F1.St.Imax[int(nChan)]; if (Rcp.GetPChg()) idx = F1.St.Imin[int(nChan)];
// 		SIn In; In.bSpdUp = false;
// 		F1.FindCG_um(nChan, idx, 1, In, Rt);
// 		padd = int((F1.Rx.Fend.i - F1.Rx.Fbeg.i) + fPad + 2.5f);
// 	}
// 	else return FALSE;
// 	//////////////////////////////////////////////////////////////////////////
// 	for (int y = 0; y < ht; y += dy) {
// 		for (int x = 0; x < wd; x += dx) {
// 			idx = FindMaxPeakIdx(F1, nChan, x, y);
// 			float v = F1.F[idx].y[int(nChan)];
// 			if (v > max) { max = v; imax = idx; }
// 			if (v < min) { min = v; imin = idx; }
// 		}
// 	}
// 	// last line [4/15/2021 FSM]
// 	int y = ht - 1;
// 	for (int x = 0; x < wd; x += dx) { // 1290
// 		idx = FindMaxPeakIdx(F1, nChan, x, y);
// 		float v = F1.F[idx].y[int(nChan)];
// 		if (v > max) { max = v; imax = idx; }
// 		if (v < min) { min = v; imin = idx; }
// 	}
// 	// right edge [4/15/2021 FSM]
// 	int x = wd - 1;
// 	for (int y = 0; y < ht; y += dy) {
// 		idx = FindMaxPeakIdx(F1, nChan, x, y);
// 		float v = F1.F[idx].y[int(nChan)];
// 		if (v > max) { max = v; imax = idx; }
// 		if (v < min) { min = v; imin = idx; }
// 	}
//
// 	if ((max != BADDATA) && (min != BADDATA)) {
// 		int sz = int(Imgs.size());
// 		imin -= padd; if (imin < 0) imin = 0;
// 		imax += padd; if (imax >= sz) imax = sz - 1;
// 		R.i1 = imin; R.i2 = imax;
// 		R.x1_um = Imgs[imin]->PzPos_um; R.x2_um = Imgs[imax]->PzPos_um;
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// BOOL IMGL::SImgStrip::FindBestROI(IMGL::SROI& R, CHNL nChan, float fPad) {
// 	int sz = int(Imgs.size()); if (sz < 1) return FALSE;
//
// 	int wd, ht; GetDim(wd, ht);
// 	int dx = wd / 16; int dy = ht / 12;
// 	int imax = 0, imin = 0;
// 	int iChan = int(nChan);
// 	float v, max = BADDATA, min = -BADDATA;
// 	int idx, padd = 8;
//
// 	SFrP1 F;
// 	SFrin4 F1;
// 	IMGL::SROI Rt(sz);
// 	CZCache Cz; Cz.Alloc(sz);
// 	if (CollectStrip(F1, wd / 2, ht / 2, Rt, nChan, nChan)) {
// 		F1.ZeroMeanSt(nChan, Rt);
// 		idx = F1.St.Imax[iChan]; if (Rt.bPChg) idx = F1.St.Imin[iChan];
// 		F1.FindCG_um(nChan, idx, 1, Cz);
// 		padd = int((F1.Rx.Fend.i - F1.Rx.Fbeg.i) * fPad + 0.5f);
// 	}
// 	for (int y = 0; y < ht; y += dy) {
// 		if (y >= ht) continue;
// 		for (int x = 0; x < wd; x += dx) { // 1290
// 			if (x >= wd) continue;
// 			if (CollectStrip(F1, x, y, Rt, nChan, nChan)) {
// 				F1.ZeroMeanSt(nChan, Rt);
// 				if (Rt.bPChg) idx = F1.St.Imin[iChan];
// 				else idx = F1.St.Imax[iChan];
// 				v = F1.F[idx].x_um;
// 				if (v > max) { max = v; imax = idx; }
// 				if (v < min) { min = v; imin = idx; }
// 			}
// 		}
// 	}
// 	// last line [4/15/2021 FSM]
// 	int y = ht - 1;
// 	for (int x = 0; x < wd; x += dx) { // 1290
// 		if (x >= wd) continue;
// 		if (CollectStrip(F1, x, y, Rt, nChan, nChan)) {
// 			F1.ZeroMeanSt(nChan, Rt);
// 			if (Rt.bPChg) idx = F1.St.Imin[iChan];
// 			else idx = F1.St.Imax[iChan];
// 			v = F1.F[idx].x_um;
// 			if (v > max) { max = v; imax = idx; }
// 			if (v < min) { min = v; imin = idx; }
// 		}
// 	}
// 	// right edge [4/15/2021 FSM]
// 	int x = wd - 1;
// 	for (int y = 0; y < ht; y += dy) {
// 		if (y >= ht) continue;
// 		if (CollectStrip(F1, x, y, Rt, nChan, nChan)) {
// 			F1.ZeroMeanSt(nChan, Rt);
// 			if (Rt.bPChg) idx = F1.St.Imin[iChan];
// 			else idx = F1.St.Imax[iChan];
// 			v = F1.F[idx].x_um;
// 			if (v > max) { max = v; imax = idx; }
// 			if (v < min) { min = v; imin = idx; }
// 		}
// 	}
//
// 	//IMGL::SROI& R = UI.GetR1();
// 	//R.Clear();
// 	if ((max != BADDATA) && (min != BADDATA)) {
// 		int sz = int(Imgs.size());
// 		imin -= padd; if (imin < 0) imin = 0;
// 		imax += padd; if (imax >= sz) imax = sz - 1;
// 		R.i1 = imin; R.i2 = imax;
// 		R.x1_um = Imgs[imin]->PzPos_um; R.x2_um = Imgs[imax]->PzPos_um;
// 		//UI.SetR1(R);
// 		return TRUE;
// 	}
// 	return FALSE;
// }

// int IMGL::SImgStrip::GetBestIndex(CHNL nChan, IMGL::SROI& R) {
// 	int wd, ht; GetDim(wd, ht);
// 	int sz = int(Imgs.size()); if (sz < 1) return 0;
// 	int dx = wd / 16; int dy = ht / 12;
// 	bool bPChg = Rcp.GetPChg();
// 	int n = 0, sum = 0;
// 	for (int y = 0; y < ht; y += dy) {
// 		for (int x = 0; x < wd; x += dx) {
// 			IMGL::SFrin4 F;
// 			if (!CollectStripST(F, x, y, R, nChan, nChan)) continue;
// 			if (bPChg) sum += F.St.Imin[int(nChan)];
// 			else sum += F.St.Imax[int(nChan)];
// 			n++;
// 		}
// 	}
// 	if (n) {
// 		int idx = sum / n;
// 		if ((idx < 3) || (idx >= (sz - 4))) return sz / 2;
// 		return idx;
// 	}
// 	return sz / 2;
// }

//////////////////////////////////////////////////////////////////////////
// void IMGL::SImgStrip::GetPhaseLineRG(IMGL::CIM16& Im16_rad, IMGL::CIM16& Im16Eq_rad,
// 	CHNL nChan, int y, int wd, float alpha_rad, std::vector<IMGL::SFrP1>& Pos) {
// 	float phaseCh, phaseRd, phaseGr, phaseEq;
// 	switch (algo.algo) {
// 	case EALGO::STEP3NV:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase3NV_rad(x, y, alpha_rad, nChan, Pos).phase;
// 			phaseRd = GetPhase3NV_rad(x, y, alpha_rad, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase3NV_rad(x, y, alpha_rad, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP3A:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase3a(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase3a(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase3a(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP3B:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase3b(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase3b(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase3b(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP3C:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase3c(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase3c(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase3c(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP3E:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase3e(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase3e(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase3e(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP4A:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase4a(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase4a(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase4a(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP4C:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase4c(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase4c(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase4c(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP4NV:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase4NV(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase4NV(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase4NV(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP5A:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase5a(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase5a(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase5a(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP5B:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase5b(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase5b(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase5b(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP5C:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase5c(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase5c(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase5c(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP5NV:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase5NV_rad(x, y, alpha_rad, nChan, Pos).phase;
// 			phaseRd = GetPhase5NV_rad(x, y, alpha_rad, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase5NV_rad(x, y, alpha_rad, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP6A:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase6a(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase6a(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase6a(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP6B:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase6b(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase6b(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase6b(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP7A:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase7a(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase7a(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase7a(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP7B:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase7b(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase7b(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase7b(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	case EALGO::STEP8A:
// 		for (int x = 0; x < wd; x++) {
// 			phaseCh = GetPhase8a(x, y, nChan, Pos).phase;
// 			phaseRd = GetPhase8a(x, y, CHNL::REDCH, Pos).phase;
// 			phaseGr = GetPhase8a(x, y, CHNL::GRNCH, Pos).phase;
// 			phaseEq = CAng::SubR(phaseGr, phaseRd);
// 			Im16_rad.AddPixel(x, y, phaseCh); Im16Eq_rad.AddPixel(x, y, phaseEq);
// 		}
// 		break;
// 	default:ASSERT(0); break;
// 	}
// }

// BOOL IMGL::SImgStrip::CalcCenterWLenAll(CHNL nChan) {
// 	int wd, ht; GetDim(wd, ht);
// 	int n[int(CHNL::BLUCH) + 1] = { 0 };
// 	int dx = wd / 25; int dy = ht / 25;
//
// 	IMGL::SROI R = UI.GetROI(this);
// 	bool bPChg = Rcp.GetPChg();
// 	int idx = GetBestIndex(nChan, R);
// 	for (int i = int(CHNL::WHTCH); i <= int(CHNL::BLUCH); i++) {
// 		Con.CalibCtWLen_nm[i] = 0;
// 	}
// 	int sz = R.i2 - R.i1;
// 	//#pragma omp parallel for
// 	for (int y = dy; y < (ht - dy); y += dy) {
// 		for (int x = dx; x < (wd - dx); x += dx) {
// 			IMGL::SFrin4 F;
// 			if (CollectStrip(F, x, y, R, CHNL::WHTCH, CHNL::BLUCH)) {
// 				F.ZeroMeanRGBT3(nChan, R);
// 				for (int i = int(CHNL::WHTCH); i <= int(CHNL::BLUCH); i++) {
// 					Con.CalibCtWLen_nm[i] += F.CalceWLenPoint_nm(CHNL(i), idx, bPChg);
// 					n[i]++;
// 				}
// 			}
// 		}
// 	}
// 	for (int i = int(CHNL::WHTCH); i <= int(CHNL::BLUCH); i++) {
// 		Con.CalibCtWLen_nm[i] /= float(n[i]);
// 	}
// 	Con.EqvCtWLenRG_nm = Con.EqvWLen(Con.CalibCtWLen_nm[int(CHNL::REDCH)], Con.CalibCtWLen_nm[int(CHNL::GRNCH)]);
// 	Con.EqvCtWLenGB_nm = Con.EqvWLen(Con.CalibCtWLen_nm[int(CHNL::GRNCH)], Con.CalibCtWLen_nm[int(CHNL::BLUCH)]);
// 	Con.EqvCtWLenRB_nm = Con.EqvWLen(Con.CalibCtWLen_nm[int(CHNL::REDCH)], Con.CalibCtWLen_nm[int(CHNL::BLUCH)]);
// 	Con.EqvCtWLenRGB_nm = Con.EqvWLen(Con.EqvCtWLenRG_nm, Con.EqvCtWLenGB_nm);
// 	return TRUE;
// }
//
// BOOL IMGL::SImgStrip::CalcWLen(CHNL nChan) {
// 	int wd, ht; GetDim(wd, ht);
// 	int dx = wd / 10; int dy = ht / 10;
//
// 	IMGL::SROI R; R.Set(int(Imgs.size()));
// 	BOOL bPChg = Rcp.GetPChg();
// 	int sz = int(Imgs.size());
// 	int idx = sz / 2;
// 	Con.CalibCtWLen_nm[int(nChan)] = 0;
//
// 	IMGL::SFrin4 F;
// 	for (int y = dy; y < (ht - dy); y += dy) {
// 		for (int x = dx; x < (wd - dx); x += dx) {
// 			if (!CollectStrip(F, x, y, R, nChan, nChan)) return FALSE;
// 			F.ZeroMean(nChan, R);
// 			float v = float(fabs(F.CalceWLenPoint_nm(nChan, idx, bPChg)));
// 			if (v > 0) { Con.CalibCtWLen_nm[int(nChan)] += v; }
// 		}
// 	}
// 	return TRUE;
// }

SPhMo IMGL::SImgStrip::GetPhase3a(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[3];
	GetIntenV2(x, y, I, 3, nChan, vP);
	return C.Phase3a_rad(I[0], I[1], I[2]);
}

SPhMo IMGL::SImgStrip::GetPhase3b(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[3];
	GetIntenV2(x, y, I, 3, nChan, vP);
	return C.Phase3b_rad(I[0], I[1], I[2]);
}

SPhMo IMGL::SImgStrip::GetPhase3c(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[3];
	GetIntenV2(x, y, I, 3, nChan, vP);
	return C.Phase3c_rad(I[0], I[1], I[2]);
}

SPhMo IMGL::SImgStrip::GetPhase3e(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[3];
	GetIntenV2(x, y, I, 3, nChan, vP);
	return C.Phase3e_rad(I[0], I[1], I[2]);
}

SPhMo IMGL::SImgStrip::GetPhase3NV_rad(int x, int y, float PShift_rad, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[3];
	GetIntenV2(x, y, I, 3, nChan, vP);
	return C.Phase3NV_rad(I[0], I[1], I[2], PShift_rad);
}

SPhMo IMGL::SImgStrip::GetPhase4a(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[4];
	GetIntenV2(x, y, I, 4, nChan, vP);
	return C.Phase4a_rad(I[0], I[1], I[2], I[3]);
}

SPhMo IMGL::SImgStrip::GetPhase4c(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[4];
	GetIntenV2(x, y, I, 4, nChan, vP);
	return C.Phase4c_rad(I[0], I[1], I[2], I[3]);
}

SPhMo IMGL::SImgStrip::GetPhase4NV(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[4];
	GetIntenV2(x, y, I, 4, nChan, vP);
	return C.Phase4NV_rad(I[0], I[1], I[2], I[3]);
}

SPhMo IMGL::SImgStrip::GetPhase5a(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[5];
	GetIntenV2(x, y, I, 5, nChan, vP);
	return C.Phase5a_rad(I[0], I[1], I[2], I[3], I[4]);
}

SPhMo IMGL::SImgStrip::GetPhase5b(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[5];
	GetIntenV2(x, y, I, 5, nChan, vP);
	return C.Phase5b_rad(I[0], I[1], I[2], I[3], I[4]);
}

SPhMo IMGL::SImgStrip::GetPhase5c(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[5];
	GetIntenV2(x, y, I, 5, nChan, vP);
	return C.Phase5c_rad(I[0], I[1], I[2], I[3], I[4]);
}

SPhMo IMGL::SImgStrip::GetPhase5NV_rad(int x, int y, float piShift_rad, eCH nChan, std::vector<SFrP1>& vP) {
	// no sanity  check [3/8/2021 FSM]
	CPsiCalc C;
	float I[5];
	GetIntenV2(x, y, I, 5, nChan, vP);
	return C.Phase5NV_rad(I[0], I[1], I[2], I[3], I[4], piShift_rad);
}

SPhMo IMGL::SImgStrip::GetPhase6a(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[6];
	GetIntenV2(x, y, I, 6, nChan, vP);
	return C.Phase6a_rad(I[0], I[1], I[2], I[3], I[4], I[5]);
}

SPhMo IMGL::SImgStrip::GetPhase6b(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[6];
	GetIntenV2(x, y, I, 6, nChan, vP);
	return C.Phase6b_rad(I[0], I[1], I[2], I[3], I[4], I[5]);
}

SPhMo IMGL::SImgStrip::GetPhase7a(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[7];
	GetIntenV2(x, y, I, 7, nChan, vP);
	return C.Phase7a_rad(I[0], I[1], I[2], I[3], I[4], I[5], I[6]);
}

SPhMo IMGL::SImgStrip::GetPhase7b(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[7];
	GetIntenV2(x, y, I, 7, nChan, vP);
	return C.Phase7b_rad(I[0], I[1], I[2], I[3], I[4], I[5], I[6]);
}

SPhMo IMGL::SImgStrip::GetPhase8a(int x, int y, eCH nChan, std::vector<SFrP1>& vP) {
	CPsiCalc C;
	float I[8];
	GetIntenV2(x, y, I, 8, nChan, vP);
	return C.Phase8a_rad(I[0], I[1], I[2], I[3], I[4], I[5], I[6], I[7]);
}