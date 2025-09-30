#include "pch.h"

#include <iostream>

#include "Strip.h"
#include "PSI/ICC.h"
#include "IM/fArchive.h"
#include "MTH/Ang.h"
#include "SRC/DOSUtil.h"
#include <kernel.h>

WLI::CStrip Strip;

bool WLI::CStrip::GetDim(int& x, int& y, int& b) {
	if (Imgs.size() < 1) {
		wd = ht = 0, bpp = 0;
		x = y = -1; return false;
	}
	bool ret = Imgs[0]->GetDim(wd, ht, b);
	x = wd; y = ht; bpp = b;
	return ret;
}

//20250916 
bool WLI::CStrip::GetDimCV(int& x, int& y, int& b) {
	if (CVImgs.size() < 1) {
		wd = ht = 0, bpp = 0;
		x = y = -1; return false;
	}
	bool ret;
	ret = CVImgs[0].first.empty() ? 0 : 1;

	wd = CVImgs[0].first.cols;
	ht = CVImgs[0].first.rows;
	int channels = CVImgs[0].first.channels();
	int depth = CVImgs[0].first.depth();
	b = channels * (depth == CV_8U ? 8 : depth == CV_16U ? 16 : 32);

	x = wd, y = ht, bpp = b;
	return ret;

}

void WLI::CStrip::ResetTime() {
	htime.clear();
}

void WLI::CStrip::AddTime(CHighTime tm) {
	htime.push_back(tm);
}

CString WLI::CStrip::TimeSpanStr(int ed, int st) {
	CHighTimeSpan tmSpan = htime[ed] - htime[st];
	return tmSpan.FormatA(L"%M m %S s %s ms");
}

WLI::SIms* WLI::CStrip::NewImgs(float xpos_um) {
	SIms* p = new SIms; if (!p) return nullptr;
	p->PzPos_um = xpos_um;
	Imgs.push_back(p);
	return p;
}

IMGL::CIM* WLI::CStrip::GetImg(int idx) {
	// no sanity check
	return &Imgs[idx]->Im;
}

WLI::SIms* WLI::CStrip::GetIms(int idx) {
	// no sanity check
	return Imgs[idx];
}

int WLI::CStrip::size() { return int(Imgs.size()); }
int WLI::CStrip::sizeCV() { return int(CVImgs.size()); } //20250916

int WLI::CStrip::GetMaxPeakIdx(int x, int y, WLI::FRP nChan, bool bPChg /*= false*/) {
	float v, max = -1, min = 999;
	int sz = int(Imgs.size()), idx = -1;
	switch (nChan) {
	case WLI::WHTA:
		for (int i = 0; i < sz; i++) {
			SIms* pI = Imgs[i]; if (pI->Im.IsNull()) return -1;
			COLORREF cr = pI->GetPixRGB(x, y);
			//v = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f;
			switch (WhtCalc) {
			case WLI::PCPTN:
				v = (sfR * GetRValue(cr) + sfG * GetGValue(cr) + sfB * GetBValue(cr));
				break;
			case WLI::R121:
				v = (GetRValue(cr) + 2 * GetGValue(cr) + GetBValue(cr)) / 4.f;
				break;
			case WLI::R131:
				v = (GetRValue(cr) + 3 * GetGValue(cr) + GetBValue(cr)) / 5.f;
				break;
			default:
				v = (GetRValue(cr) + GetGValue(cr) + GetBValue(cr)) / 3.f;
				break;
			}
			if (bPChg) { if (v < min) { min = v; idx = i; } }
			else { if (v > max) { max = v; idx = i; } };
		}
		break;
	case WLI::REDA:
		for (int i = 0; i < sz; i++) {
			SIms* pI = Imgs[i]; if (pI->Im.IsNull()) return -1;
			COLORREF cr = pI->GetPixRGB(x, y);
			v = GetRValue(cr);
			if (bPChg) { if (v < min) { min = v; idx = i; } }
			else { if (v > max) { max = v; idx = i; } };
		}
		break;
	case WLI::GRNA:
		for (int i = 0; i < sz; i++) {
			SIms* pI = Imgs[i]; if (pI->Im.IsNull()) return -1;
			COLORREF cr = pI->GetPixRGB(x, y);
			v = GetGValue(cr);
			if (bPChg) { if (v < min) { min = v; idx = i; } }
			else { if (v > max) { max = v; idx = i; } };
		}
		break;
	case WLI::BLUA:
		for (int i = 0; i < sz; i++) {
			SIms* pI = Imgs[i]; if (pI->Im.IsNull()) return -1;
			COLORREF cr = pI->GetPixRGB(x, y);
			v = GetBValue(cr);
			if (bPChg) { if (v < min) { min = v; idx = i; } }
			else { if (v > max) { max = v; idx = i; } };
		}
		break;
	}
	return idx;
}

float WLI::CStrip::GetZPos(int idx) {
	// no sanity check
	return Imgs[idx]->PzPos_um;
}

CString WLI::CStrip::GetReport() {
	CString str, st;
	str.Format(L"Number of frames: %d\n", size());
	st.Format(L"Image size: %d x %d x %d\n", wd, ht, bpp);
	str += st;
	st.Format(L"Wavelength (nm): R %.1f  G %.1f  B %.1f  W %.1f\n",
		2 * wlen_um[0] * 1e3f, 2 * wlen_um[1] * 1e3f, 2 * wlen_um[2] * 1e3f, 2 * wlen_um[3] * 1e3f
	);
	str += st;
	st.Format(L"Range: %.4f um,  Unit step: %.4f", zrange_um, UStep_um);
	str += st;
	return str;
}

bool WLI::CStrip::InitCalcCV()
{
	WLI::SFrng F;

	nSteps = int(CVImgs.size()); if (nSteps < 1) return false;
	zrange_um = CVImgs[nSteps - 1].second - CVImgs[0].second;
	UStep_um = zrange_um / nSteps;
	//GetDim(wd, ht, bpp);
	GetDimCV(wd, ht, bpp);

	int Ch1 = WLI::REDA, Ch2 = WLI::GRNA, Ch3 = WLI::BLUA, Ch4 = WLI::WHTA;
	SROI R(nSteps);
	wlen_um[Ch1] = 0; wlen_um[Ch2] = 0;
	wlen_um[Ch3] = 0; wlen_um[Ch4] = 0;

	int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
	int dh = ht / 7, dw = wd / 7;

	//if (ICC.isRegionType == ICC.LINE) { // 07122023
	//	if (ICC.isOriental == ICC.HORIZONTAL) ICC.y2 = ICC.y1; // 06222023
	//	else ICC.x2 = ICC.x1;
	//}

	for (int y = dh; y < ht; y += dh) {
		for (int x = dw; x < wd; x += dw) {
			//for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 06222023
			//	for (int x = ICC.x1; x <= ICC.x2; x++) { // Width
			//CollectRGBW(F, x, y, R);
			CollectRGBWCV(F, x, y, R);	// collect r channel, g channel, b channel and white value
			//F.MakeWhite(R, false, 0);
			// calculate max min or average for smoothed channel data
			F.Smooth(WLI::REDA, 1, 3, R); F.Smooth(WLI::GRNA, 1, 3, R);
			F.Smooth(WLI::BLUA, 1, 3, R); F.Smooth(WLI::WHTA, 1, 3, R);

			float wlr = F.Wavelength(WLI::REDA, 2);
			if (wlr > 0) { wlen_um[Ch1] += wlr; n1++; }
			float wlg = F.Wavelength(WLI::GRNA, 2);
			if (wlg > 0) { wlen_um[Ch2] += wlg; n2++; }
			float wlb = F.Wavelength(WLI::BLUA, 2);
			if (wlb > 0) { wlen_um[Ch3] += wlb; n3++; }
			float wlw = F.Wavelength(WLI::WHTA, 2);
			if (wlw > 0) { wlen_um[Ch4] += wlw; n4++; }
		}
	}
	if (n1) wlen_um[Ch1] /= float(n1); //else ASSERT(0);
	if (n2) wlen_um[Ch2] /= float(n2); //else ASSERT(0);
	if (n3) wlen_um[Ch3] /= float(n3); //else ASSERT(0);
	if (n4) wlen_um[Ch4] /= float(n4); //else ASSERT(0);
	wlen_um[Ch4 + 1] = (wlen_um[Ch1] * wlen_um[Ch2]) / fabs(wlen_um[Ch1] - wlen_um[Ch2]);
	return true;
}

bool WLI::CStrip::InitCalc() {
	WLI::SFrng F;

	nSteps = int(Imgs.size()); if (nSteps < 1) return false;
	zrange_um = Imgs[nSteps - 1]->PzPos_um - Imgs[0]->PzPos_um;
	UStep_um = zrange_um / nSteps;
	GetDim(wd, ht, bpp);

	int Ch1 = WLI::REDA, Ch2 = WLI::GRNA, Ch3 = WLI::BLUA, Ch4 = WLI::WHTA;
	SROI R(nSteps);
	wlen_um[Ch1] = 0; wlen_um[Ch2] = 0;
	wlen_um[Ch3] = 0; wlen_um[Ch4] = 0;

	int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
	int dh = ht / 7, dw = wd / 7;

	//if (ICC.isRegionType == ICC.LINE) { // 07122023
	//	if (ICC.isOriental == ICC.HORIZONTAL) ICC.y2 = ICC.y1; // 06222023
	//	else ICC.x2 = ICC.x1;
	//}

	for (int y = dh; y < ht; y += dh) {
		for (int x = dw; x < wd; x += dw) {
			//for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 06222023
			//	for (int x = ICC.x1; x <= ICC.x2; x++) { // Width
			CollectRGBW(F, x, y, R);	// collect r channel, g channel, b channel and white value
			//F.MakeWhite(R, false, 0);
			// calculate max min or average for smoothed channel data
			F.Smooth(WLI::REDA, 1, 3, R); F.Smooth(WLI::GRNA, 1, 3, R);
			F.Smooth(WLI::BLUA, 1, 3, R); F.Smooth(WLI::WHTA, 1, 3, R);

			float wlr = F.Wavelength(WLI::REDA, 2);
			if (wlr > 0) { wlen_um[Ch1] += wlr; n1++; }
			float wlg = F.Wavelength(WLI::GRNA, 2);
			if (wlg > 0) { wlen_um[Ch2] += wlg; n2++; }
			float wlb = F.Wavelength(WLI::BLUA, 2);
			if (wlb > 0) { wlen_um[Ch3] += wlb; n3++; }
			float wlw = F.Wavelength(WLI::WHTA, 2);
			if (wlw > 0) { wlen_um[Ch4] += wlw; n4++; }
		}
	}
	if (n1) wlen_um[Ch1] /= float(n1); //else ASSERT(0);
	if (n2) wlen_um[Ch2] /= float(n2); //else ASSERT(0);
	if (n3) wlen_um[Ch3] /= float(n3); //else ASSERT(0);
	if (n4) wlen_um[Ch4] /= float(n4); //else ASSERT(0);
	wlen_um[Ch4 + 1] = (wlen_um[Ch1] * wlen_um[Ch2]) / fabs(wlen_um[Ch1] - wlen_um[Ch2]);
	return true;
}

void WLI::CStrip::DeallocAll() {
	size_t sz = Imgs.size();
	for (size_t i = 0; i < sz; i++) {
		delete Imgs[i];
	}
	Imgs.clear();
}

//20250916

void WLI::CStrip::DeallocAllCV() {
	CVImgs.clear();
}

bool WLI::CStrip::HProfile(SFrng& F, WLI::FRP Ch, int y, SROI& R) {
	if (Im16um.IsNull()) return false;
	int wd = Im16um.GetWidth();
	double ave = 0;
	int imx = BADDATA, imn = -BADDATA;
	float mx = BADDATA, mn = -BADDATA;
	float* pZ = F.Z.Get(Ch, 0, wd);
	float* pX = F.Z.Get(WLI::ZAXS, 0, wd);
	for (int x = 0; x < wd; x++, pX++, pZ++) {
		*pX = float(x + 1);
		float v = *pZ = Im16um.GetPixel(x, y);
		ave += v;
		if (v > mx) { mx = v; imx = x; }
		else if (v < mn) { mn = v; imn = x; }
	}
	R.i1 = 0; R.i2 = wd;
	SStat& St = F.Z.St[int(Ch)];
	St.fmax = mx;  St.fmin = mn;
	St.imx = imx; St.imn = imn;
	St.fave = float(ave / float(wd));

	return true;
}

bool WLI::CStrip::VProfile(SFrng& F, WLI::FRP Ch, int x, SROI& R) {
	if (Im16um.IsNull()) return false;
	int ht = Im16um.GetHeight();
	double ave = 0;
	int imx = BADDATA, imn = -BADDATA;
	float mx = BADDATA, mn = -BADDATA;
	float* pZ = F.Z.Get(Ch, 0, ht);
	float* pX = F.Z.Get(WLI::ZAXS, 0, ht);
	for (int y = 0; y < ht; y++, pX++, pZ++) {
		*pX = float(y + 1);
		float v = *pZ = Im16um.GetPixel(x, y);
		ave += v;
		if (v > mx) { mx = v; imx = y; }
		else if (v < mn) { mn = v; imn = y; }
	}
	R.i1 = 0; R.i2 = ht;
	SStat& St = F.Z.St[int(Ch)];
	St.fmax = mx;  St.fmin = mn;
	St.imx = imx; St.imn = imn;
	St.fave = float(ave / float(ht));

	return true;
}

bool WLI::CStrip::CollectZCH(SFrng& F, int x, int y, SROI& R, WLI::FRP Ch) {
	// no sanity check from this point on
	int sz = Strip.size();
	int st = R.i1, ed = R.i2;
	if (st < 0) { st = 0; ASSERT(0); }
	if (ed > nSteps) { ed = nSteps; ASSERT(0); }
	if ((ed - st) > nSteps) { ASSERT(0);  return false; }

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	double avew = 0, aver = 0, aveg = 0, aveb = 0;
	SStat* pSt;
	SIms** pI = &Imgs[st];
	float* pX, * pZ1, * pZ2, * pZ3, * pZ4;

	// background Image need to modify this also ARIF
	bool bBg = false;
	if (!ImBG.IsNull()) {
		bBg = true;
		COLORREF cr = ImBG.GetPixel(x, y);
		aver = GetRValue(cr); aveg = GetGValue(cr); aveb = GetBValue(cr);
		avew = (aver + 2 * aveg + aveb) / 4;
	}

	float v1, v2, v3, v4;
	switch (Ch) {
	case WLI::WHTA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::REDA, st, nSteps);
		pZ2 = F.Z.Get(WLI::GRNA, st, nSteps);
		pZ3 = F.Z.Get(WLI::BLUA, st, nSteps);
		pZ4 = F.Z.Get(WLI::WHTA, st, nSteps);
		for (int i = st; i < ed; i++, pI++, pX++, pZ1++, pZ2++, pZ3++, pZ4++) {
			*pX = (*pI)->PzPos_um; 
			COLORREF cr = (*pI)->GetPixRGB(x, y);
			v1 = *pZ1 = GetRValue(cr);
			v2 = *pZ2 = GetGValue(cr);
			v3 = *pZ3 = GetBValue(cr);

			switch (WhtCalc) {
			case WLI::PCPTN:
				v4 = *pZ4 = (sfR * v1 + sfG * v2 + sfB * v3);
				break;
			case WLI::R121:
				v4 = *pZ4 = (v1 + 2 * v2 + v3) / 4.f;
				break;
			case WLI::R131:
				v4 = *pZ4 = (v1 + 3 * v2 + v3) / 5.f;
				break;
			default:
				v4 = *pZ4 = (v1 + v2 + v3) / 3.f;
				break;
			}
			if (!bBg) avew += v4;
			if (!bBg) aver += v1;
			if (!bBg) aveg += v2;
			if (!bBg) aveb += v3;
		}
		F.MaxMin(WLI::REDA, R, sz, true);
		F.MaxMin(WLI::GRNA, R, sz, true);
		F.MaxMin(WLI::BLUA, R, sz, true);
		F.MaxMin(WLI::WHTA, R, sz, true);

		pSt = &F.Z.St[int(REDA)];
		if (bBg) pSt->fave = float(aver);
		pSt++;
		if (bBg) pSt->fave = float(aveg);
		pSt++;
		if (bBg) pSt->fave = float(aveb);
		pSt++;
		if (bBg) pSt->fave = float(avew);
		break;
	case WLI::REDA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::REDA, st, nSteps);
		for (int i = st; i < ed; i++, pI++, pX++, pZ1++) { 
			*pX = (*pI)->PzPos_um;
			float v = GetRValue((*pI)->GetPixRGB(x, y));
			*pZ1 = v; //if (!bBg) aver += v;
		}
		F.MaxMin(WLI::REDA, R, sz, true);
		pSt = &F.Z.St[int(REDA)];
		if (bBg) pSt->fave = float(aver); //else pSt->fave = float(aver / (ed - st));
		break;
	case WLI::GRNA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::GRNA, st, nSteps);
		for (int i = st; i < ed; i++, pI++, pX++, pZ1++) {
			*pX = (*pI)->PzPos_um;
			float v = GetGValue((*pI)->GetPixRGB(x, y));
			*pZ1 = v; //if (!bBg) aveg += v;
		}
		F.MaxMin(WLI::GRNA, R, sz, true);
		pSt = &F.Z.St[int(GRNA)];
		if (bBg) pSt->fave = float(aveg); //else pSt->fave = float(aveg / (ed - st));
		break;
	case WLI::BLUA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::BLUA, st, nSteps);
		for (int i = st; i < ed; i++, pI++, pX++, pZ1++) {
			*pX = (*pI)->PzPos_um;
			float v = GetBValue((*pI)->GetPixRGB(x, y));

			*pZ1 = v; //if (!bBg) aveb += v;
		}
		F.MaxMin(WLI::BLUA, R, sz, true);
		pSt = &F.Z.St[int(BLUA)];
		if (bBg) pSt->fave = float(aveb); //else pSt->fave = float(aveb / (ed - st));
		break;
	default: assert(0); return false; break;
	}
	return true;
}

//20250916
bool WLI::CStrip::CollectZCHCV(SFrng& F, int x, int y, SROI& R, WLI::FRP Ch) {
	// no sanity check from this point on
	//int sz = Strip.size(); //ARIF COMMENTED 20250916
	int sz = Strip.sizeCV(); //20250916
	int st = R.i1, ed = R.i2;
	if (st < 0) { st = 0; ASSERT(0); }
	if (ed > nSteps) { ed = nSteps; ASSERT(0); }
	if ((ed - st) > nSteps) { ASSERT(0);  return false; }

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	double avew = 0, aver = 0, aveg = 0, aveb = 0;
	SStat* pSt;
	//SIms** pI = &Imgs[st]; //ARIF COMMENTED 20250916
	std::pair<cv::Mat,float>* pICV =& CVImgs[st]; //.first; //20250916
	float* pX, * pZ1, * pZ2, * pZ3, * pZ4;

	// background Image need to modify this also ARIF
	bool bBg = false;
	if (!ImBG.IsNull()) {
		bBg = true;
		COLORREF cr = ImBG.GetPixel(x, y);
		aver = GetRValue(cr); aveg = GetGValue(cr); aveb = GetBValue(cr);
		avew = (aver + 2 * aveg + aveb) / 4;
	}

	float v1, v2, v3, v4;
	switch (Ch) {
	case WLI::WHTA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::REDA, st, nSteps);
		pZ2 = F.Z.Get(WLI::GRNA, st, nSteps);
		pZ3 = F.Z.Get(WLI::BLUA, st, nSteps);
		pZ4 = F.Z.Get(WLI::WHTA, st, nSteps);
		for (int i = st; i < ed; i++, /*pI++,*/pICV++, pX++, pZ1++, pZ2++, pZ3++, pZ4++) {  //ARIF COMMENTED 20250916
			//ARIF COMMENTED 20250916
			//*pX = (*pI)->PzPos_um; 
			//COLORREF cr = (*pI)->GetPixRGB(x, y);
			//v1 = *pZ1 = GetRValue(cr);
			//v2 = *pZ2 = GetGValue(cr);
			//v3 = *pZ3 = GetBValue(cr);
	
			//20250916
			*pX = (*pICV).second;
			cv::Vec3b bgrPixel = CVImgs[i].first.at<cv::Vec3b>(y, x);
			COLORREF cr = RGB(bgrPixel[2], bgrPixel[1], bgrPixel[0]);
			// collect rgb data in a points
			v1 = *pZ1 = GetRValue(cr);
			v2 = *pZ2 = GetGValue(cr);
			v3 = *pZ3 = GetBValue(cr);
			//
			
			switch (WhtCalc) {
			case WLI::PCPTN:
				v4 = *pZ4 = (sfR * v1 + sfG * v2 + sfB * v3);
				break;
			case WLI::R121:
				v4 = *pZ4 = (v1 + 2 * v2 + v3) / 4.f;
				break;
			case WLI::R131:
				v4 = *pZ4 = (v1 + 3 * v2 + v3) / 5.f;
				break;
			default:
				v4 = *pZ4 = (v1 + v2 + v3) / 3.f;
				break;
			}
			if (!bBg) avew += v4;
			if (!bBg) aver += v1;
			if (!bBg) aveg += v2;
			if (!bBg) aveb += v3;
		}
		F.MaxMin(WLI::REDA, R, sz, true);
		F.MaxMin(WLI::GRNA, R, sz, true);
		F.MaxMin(WLI::BLUA, R, sz, true);
		F.MaxMin(WLI::WHTA, R, sz, true);

		pSt = &F.Z.St[int(REDA)];
		if (bBg) pSt->fave = float(aver);
		pSt++;
		if (bBg) pSt->fave = float(aveg);
		pSt++;
		if (bBg) pSt->fave = float(aveb);
		pSt++;
		if (bBg) pSt->fave = float(avew);
		break;
	case WLI::REDA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::REDA, st, nSteps);
		for (int i = st; i < ed; i++, /*pI++,*/ pICV++, pX++, pZ1++) { //ARIF COMMENTED 20250916
			//ARIF COMMENTED 20250916
			//*pX = (*pI)->PzPos_um;
			//float v = GetRValue((*pI)->GetPixRGB(x, y));
			
			//20250916
			*pX = (*pICV).second;
			float v = CVImgs[i].first.at<cv::Vec3b>(y, x)[2]; //get red value 
			//
			*pZ1 = v; //if (!bBg) aver += v;
		}
		F.MaxMin(WLI::REDA, R, sz, true);
		pSt = &F.Z.St[int(REDA)];
		if (bBg) pSt->fave = float(aver); //else pSt->fave = float(aver / (ed - st));
		break;
	case WLI::GRNA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::GRNA, st, nSteps);
		for (int i = st; i < ed; i++, /*pI++,*/ pICV++, pX++, pZ1++) { //ARIF COMMENTED 20250916
			//ARIF COMMENTED 20250916
			//*pX = (*pI)->PzPos_um;
			//float v = GetGValue((*pI)->GetPixRGB(x, y));

			//20250916
			*pX = (*pICV).second;
			float v = CVImgs[i].first.at<cv::Vec3b>(y, x)[1]; //get green value
			//
			*pZ1 = v; //if (!bBg) aveg += v;
		}
		F.MaxMin(WLI::GRNA, R, sz, true);
		pSt = &F.Z.St[int(GRNA)];
		if (bBg) pSt->fave = float(aveg); //else pSt->fave = float(aveg / (ed - st));
		break;
	case WLI::BLUA:
		pX = F.Z.Get(WLI::ZAXS, st, nSteps);
		pZ1 = F.Z.Get(WLI::BLUA, st, nSteps);
		for (int i = st; i < ed; i++, /*pI++,*/pICV++, pX++, pZ1++) { //ARIF COMMENTED 20250916
			//ARIF COMMENTED 20250916
			//*pX = (*pI)->PzPos_um;
			//float v = GetBValue((*pI)->GetPixRGB(x, y));

			//20250916
			*pX = (*pICV).second;
			float v = CVImgs[i].first.at<cv::Vec3b>(y, x)[0];
			//
			*pZ1 = v; //if (!bBg) aveb += v;
		}
		F.MaxMin(WLI::BLUA, R, sz, true);
		pSt = &F.Z.St[int(BLUA)];
		if (bBg) pSt->fave = float(aveb); //else pSt->fave = float(aveb / (ed - st));
		break;
	default: assert(0); return false; break;
	}
	return true;
}

bool WLI::CStrip::MakeZW(SFrng& F, const SROI& R) {
	//! no sanity check
	int sz = int(Imgs.size());
	int st = R.i1, ed = R.i2;
	double aver = 0;

	SIms** pI = &Imgs[st];
	float* pZ1 = F.Z.Get(WLI::REDA, st, sz); // Beginning of frame point
	float* pZ2 = F.Z.Get(WLI::GRNA, st, sz); // Beginning of frame point
	float* pZ3 = F.Z.Get(WLI::BLUA, st, sz); // Beginning of frame point
	float* pZ4 = F.Z.Get(WLI::WHTA, st, sz); // Beginning of frame point
	for (int i = st; i < ed; i++, pI++, pZ1++, pZ2++, pZ3++, pZ4++) {
		//*pZ4 = ((*pZ1) + (*pZ2) + (*pZ3)) / 3.f;
		switch (WhtCalc) {
		case WLI::PCPTN:
			*pZ4 = (sfR * (*pZ3) + sfG * (*pZ2) + sfB * (*pZ1));
			break;
		case WLI::R121:
			*pZ4 = (*pZ3 + 2 * (*pZ2) + *pZ1) / 4.f;
			break;
		case WLI::R131:
			*pZ4 = (*pZ3 + 3 * (*pZ2) + *pZ1) / 5.f;
			break;
		default:
			*pZ4 = (*pZ3 + *pZ2 + *pZ1) / 3.f;
			break;
		}
	}
	F.MaxMin(WLI::WHTA, R, sz, true);

	return true;
}

bool WLI::CStrip::CollectZRG(SFrng& F, int x, int y, SROI& R) {
	int sz = int(Imgs.size()); if (sz < 1) return false;
	int st = R.i1, ed = R.i2;
	if (sz < (ed - st)) { assert(0);  return false; }
	double aver = 0, aveg = 0;
	SIms** pI = &Imgs[st];

	float* pX = F.Z.Get(WLI::ZAXS, st, sz); // Beginning of frame point
	float* pZ1 = F.Z.Get(WLI::REDA, st, sz); // Beginning of frame point
	float* pZ2 = F.Z.Get(WLI::GRNA, st, sz); // Beginning of frame point

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////
	// copy data from image to z-buffer
	// calculate average, max and min amplitude & index
	bool bBg = false;
	if (!ImBG.IsNull()) {
		bBg = true;
		COLORREF cr = ImBG.GetPixel(x, y);
		aver = GetRValue(cr); aveg = GetGValue(cr); //aveb = GetBValue(cr);
	}
	int cnt = 0;
	for (int i = st; i < ed; i++, pI++, pX++, pZ1++, pZ2++) {
		*pX = (*pI)->PzPos_um;
		COLORREF cr = (*pI)->GetPixRGB(x, y);
		float v = GetRValue(cr); *pZ1 = v;
		v = GetGValue(cr); *pZ2 = v;
		cnt++;
	}
	F.MaxMin(WLI::REDA, R, sz, !bBg);
	F.MaxMin(WLI::GRNA, R, sz, !bBg);

	if (bBg) {
		SStat* St = &F.Z.St[int(WLI::REDA)];
		St->fave = float(aver);
		St++;
		St->fave = float(aveg);
	}
	return true;
}

bool WLI::CStrip::CollectRGBW(SFrng& F, int x, int y, SROI& R) {
	int sz = int(Imgs.size()); if (sz < 1) return false;
	int st = R.i1, ed = R.i2;
	int cnt = ed - st;
	if (cnt < 1) { assert(0);  return false; }
	SIms** pI = &Imgs[st];

	// resize the vector for store pixel data
	float* pX = F.Z.Get(WLI::ZAXS, st, sz);
	float* pZ1 = F.Z.Get(WLI::REDA, st, sz);
	float* pZ2 = F.Z.Get(WLI::GRNA, st, sz);
	float* pZ3 = F.Z.Get(WLI::BLUA, st, sz);
	float* pZ4 = F.Z.Get(WLI::WHTA, st, sz);

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	float v1, v2, v3, v4;
	double aver = 0, aveg = 0, aveb = 0, avew = 0;
	//////////////////////////////////////////////////////////////////////////
	// copy data from image to z-buffer
	// calculate average, max and min amplitude & index
	bool bBg = false;
	if (!ImBG.IsNull()) {
		bBg = true;
		COLORREF cr = ImBG.GetPixel(x, y);
		aver = GetRValue(cr); aveg = GetGValue(cr); aveb = GetBValue(cr);
	}
	// collect data for stack of image
	for (int i = st; i < ed; i++, pI++, pX++, pZ1++, pZ2++, pZ3++, pZ4++) {
		*pX = (*pI)->PzPos_um;
		COLORREF cr = (*pI)->GetPixRGB(x, y); 
		// collect rgb data in a points
		v1 = *pZ1 = GetRValue(cr);
		v2 = *pZ2 = GetGValue(cr);
		v3 = *pZ3 = GetBValue(cr);
		//v4 = *pZ4 = (v1 + v2 + v3) / 3.f;
		
		// calculating white light value in those points
		switch (WhtCalc) {	//WhtCalc
		case WLI::PCPTN:
			v4 = *pZ4 = (sfR * v1 + sfG * v2 + sfB * v3);
			break;
		case WLI::R121:
			v4 = *pZ4 = (v1 + 2 * v2 + v3) / 4.f;
			break;
		case WLI::R131:
			v4 = *pZ4 = (v1 + 3 * v2 + v3) / 5.f;
			break;
		default:
			v4 = *pZ4 = (v1 + v2 + v3) / 3.f;
			break;
		}
	}

	// collect max min for each channel
	F.MaxMin(WLI::REDA, R, sz, !bBg);
	F.MaxMin(WLI::GRNA, R, sz, !bBg);
	F.MaxMin(WLI::BLUA, R, sz, !bBg);
	F.MaxMin(WLI::WHTA, R, sz, true);

	// set background average for each channel
	SStat* St = &F.Z.St[int(WLI::REDA)];
	if (bBg) St->fave = float(aver);
	St++; if (bBg) St->fave = float(aveg);
	St++; if (bBg) St->fave = float(aveb);
	//St++; St->fave = float(avew / cnt);

	return true;
}

//20250916
bool WLI::CStrip::CollectRGBWCV(SFrng& F, int x, int y, SROI& R) {
	int sz = int(CVImgs.size()); if (sz < 1) return false;
	int st = R.i1, ed = R.i2;
	int cnt = ed - st;
	if (cnt < 1) { assert(0);  return false; }
	//SIms** pI = &Imgs[st];

	// resize the vector for store pixel data
	float* pX = F.Z.Get(WLI::ZAXS, st, sz);
	float* pZ1 = F.Z.Get(WLI::REDA, st, sz);
	float* pZ2 = F.Z.Get(WLI::GRNA, st, sz);
	float* pZ3 = F.Z.Get(WLI::BLUA, st, sz);
	float* pZ4 = F.Z.Get(WLI::WHTA, st, sz);

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	float v1, v2, v3, v4;
	double aver = 0, aveg = 0, aveb = 0, avew = 0;
	//////////////////////////////////////////////////////////////////////////
	// copy data from image to z-buffer
	// calculate average, max and min amplitude & index
	bool bBg = false;
	if (!ImBG.IsNull()) {
		bBg = true;
		COLORREF cr = ImBG.GetPixel(x, y);
		aver = GetRValue(cr); aveg = GetGValue(cr); aveb = GetBValue(cr);
	}
	// collect data for stack of image
	for (int i = st; i < ed; i++, CVImgs[st], pX++, pZ1++, pZ2++, pZ3++, pZ4++) {
		//*pX = (*pI)->PzPos_um;
		*pX = CVImgs[i].second;
		cv::Vec3b bgrPixel = CVImgs[i].first.at<cv::Vec3b>(y, x);
		COLORREF cr = RGB(bgrPixel[2], bgrPixel[1], bgrPixel[0]);
		//COLORREF cr = (*pI)->GetPixRGB(x, y);
		// collect rgb data in a points
		v1 = *pZ1 = GetRValue(cr);
		v2 = *pZ2 = GetGValue(cr);
		v3 = *pZ3 = GetBValue(cr);
		//v4 = *pZ4 = (v1 + v2 + v3) / 3.f;

		// calculating white light value in those points
		switch (WhtCalc) {	//WhtCalc
		case WLI::PCPTN:
			v4 = *pZ4 = (sfR * v1 + sfG * v2 + sfB * v3);
			break;
		case WLI::R121:
			v4 = *pZ4 = (v1 + 2 * v2 + v3) / 4.f;
			break;
		case WLI::R131:
			v4 = *pZ4 = (v1 + 3 * v2 + v3) / 5.f;
			break;
		default:
			v4 = *pZ4 = (v1 + v2 + v3) / 3.f;
			break;
		}
	}

	// collect max min for each channel
	F.MaxMin(WLI::REDA, R, sz, !bBg);
	F.MaxMin(WLI::GRNA, R, sz, !bBg);
	F.MaxMin(WLI::BLUA, R, sz, !bBg);
	F.MaxMin(WLI::WHTA, R, sz, true);

	// set background average for each channel
	SStat* St = &F.Z.St[int(WLI::REDA)];
	if (bBg) St->fave = float(aver);
	St++; if (bBg) St->fave = float(aveg);
	St++; if (bBg) St->fave = float(aveb);
	//St++; St->fave = float(avew / cnt);

	return true;
}

bool WLI::CStrip::GenHMapV5(RCP::SRecipe& Rcp) {
	int sz = size();
	if (!Im16um.Create(wd, ht)) return false;

	WLI::SPSpar PsP;
	const WLI::FRP Ch = WHTA;

	PsP.SetConst(wlen_um[WLI::REDA], wlen_um[WLI::GRNA], wlen_um[WLI::WHTA], UStep_um);
	int inc = PsP.Inc[int(Ch)], inc2 = 2 * inc, inc3 = 3 * inc, inc4 = 4 * inc;
	int wdw = 2 * (inc - 1) + 1; if (wdw < 3) wdw = 3;
	bool bPChg = Rcp.bPChg;

	short nSmo = 0;
	if (Rcp.bSmo) nSmo = Rcp.nSmo;
	if (Rcp.bSmoHvy) nSmo = 3 * Rcp.nSmo;

	int idx = 0;
	int sz10 = sz / 8; if (sz10 < inc2) sz10 = inc2;
	//float xmx, xmn;
	float PS1sin, PS2sin, wlen = Strip.wlen_um[Ch], wlf;
	SROI R(sz), R1;
	R.EnsureValid(inc, sz);

	//if (ICC.isRegionType == ICC.LINE) { // 07122023
	//	if (ICC.isOriental == ICC.HORIZONTAL) ICC.y2 = ICC.y1;
	//	else ICC.x2 = ICC.x1;
	//}

	switch (Rcp.Mthd) {
	case RCP::PS0:
		PsP.SetConst(Strip.wlen_um[WLI::REDA], Strip.wlen_um[WLI::GRNA], Strip.wlen_um[WLI::WHTA], Strip.UStep_um);
		inc = PsP.Inc[int(Ch)]; if (inc < 1) { ASSERT(0); return false; }
		inc2 = 2 * inc; inc3 = 3 * inc; inc4 = 4 * inc;
#pragma omp parallel for
		for (int y = 0; y < ht; y++) { // 05302023 - Mortuja
			//for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			int idx;
			WLI::SFrng F; SROI R1, R2;
			SStat* pSt = &F.Z.St[Ch];
			for (int x = 0; x < wd; x++) { // 05302023 - Mortuja
				//for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (!Rcp.bFindPChg) {
					if (bPChg) idx = pSt->imn; else idx = pSt->imx;
				}
				else {
					if ((pSt->fmax - pSt->fave) >= (pSt->fave - pSt->fmin)) {
						idx = pSt->imx; bPChg = false;
					}
					else { idx = pSt->imn; bPChg = true; }
				}
				if (!R.InRange(idx)) { Im16um.SetPixel(x, y, BADDATA); continue; }
				if (nSmo) {
					if (R1.SetA(idx - sz10, idx + sz10, inc, sz)) {
						F.Smooth(Ch, nSmo, 3, R1);
					}
				}
				R2.SetI(idx, inc3, sz);
				//if (!R2.EnsureValid(inc, sz)) { Im16um.SetPixel(x, y, BADDATA); continue; }
				float Rsl;
				if (F.PhasePV5(Ch, WLI::PHS1, PsP, R2)) {
					Rsl = F.PeakPhas(WLI::PHS1, idx - 2, idx + 2, bPChg, sz);
				}
				else Rsl = BADDATA;
				Im16um.SetPixel(x, y, Rsl);
			}
		}
		break;
	case RCP::PSI:
		PsP.Set(Ch, Strip.wlen_um[Ch], Strip.UStep_um);
		wlf = Strip.wlen_um[Ch] / 2.f * PIE;
		inc = PsP.Inc[int(Ch)]; if (inc < 1) ASSERT(0);
		inc2 = 2 * inc; inc3 = 3 * inc; inc4 = 4 * inc;
		PS1sin = PsP.PSsin[Ch];
		idx = GetMaxPeakIdx(wd / 2, ht / 2, Ch);
		if (idx < (inc2) || idx >(sz - inc2)) idx = sz / 2;
		R.Set(idx - inc3, idx + inc3);
#pragma omp parallel for
		//for (int y = 0; y < ht; y++) {
		for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			WLI::SFrng F;
			//for (int x = 0; x < wd; x++) {
			for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (nSmo) { F.Smooth(Ch, nSmo, 3, R); }
				Im16um.SetPixel(x, y, F.PeakPSI5(Ch, idx, inc, PS1sin, sz));
			}
		}
		/*if (Rcp.bUnwrap)*/ Im16um.Unwrap(IMGL::LR);
		Im16um.Mult(wlf);
		break;
	case RCP::TW1:
		PsP.SetConst(Strip.wlen_um[REDA], Strip.wlen_um[GRNA], Strip.wlen_um[WHTA], Strip.UStep_um);
		inc = PsP.Inc[int(WLI::GRNA)]; if (inc < 1) ASSERT(0);
		inc2 = 2 * inc;
		idx = ICC.nIdx;
		if ((idx < inc2) || (idx >= sz - inc2)) idx = sz / 2;
		PS1sin = PsP.PSsin[FRP::REDA]; PS2sin = PsP.PSsin[FRP::GRNA];
		R1.SetA(idx - sz10, idx + sz10, inc, sz);
#pragma omp parallel for
		//for (int y = 0; y < ht; y++) {
		for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			WLI::SFrng F; SROI R1; float phG = 0;
			//for (int x = 0; x < wd; x++) {
			for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (nSmo) {
					F.Smooth(WLI::REDA, nSmo, 3, R);
					F.Smooth(WLI::GRNA, nSmo, 3, R);
				}
				Im16um.SetPixel(x, y, F.TW1Z(F.PhaseI5(PsP, idx, phG), phG, PsP));
			}
		}
		break;
	case RCP::VIS:
	default:
		PsP.Set(Ch, Strip.wlen_um[Ch], Strip.UStep_um);
		inc = PsP.Inc[int(Ch)]; if (inc < 1) ASSERT(0);
		inc2 = 2 * inc; inc4 = int(2.5 * inc);
		//xmn = Strip.Imgs[0]->PzPos_um;
		//xmx = Strip.Imgs[sz - 1]->PzPos_um;
#pragma omp parallel for
		//for (int y = 0; y < ht; y++) {
		for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			WLI::SFrng F; SROI Rt, R1;
			SStat* pSt1 = &F.Z.St[Ch];
			SStat* pSt2 = &F.Z.St[WLI::VIS1];
			//for (int x = 0; x < wd; x++) {
			for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (bPChg) R1.SetI(F.Z.St[Ch].imn, inc4, sz);
				else R1.SetI(F.Z.St[Ch].imx, inc4, sz);
				F.PhasePV5(Ch, WLI::PHS1, PsP, R1);
				//F.VisiV5(Ch, PsP, R1);
				F.Smooth(WLI::VIS1, 1, 7, R1);
				Rt.SetI(F.Z.St[WLI::VIS1].imx, inc2, sz);
				float v = F.PeakGrad(WLI::VIS1, Rt.i1, Rt.i2, sz);
				Im16um.SetPixel(x, y, v);
			}
		}
		break;
	}
	return true;
}


//20250916
bool WLI::CStrip::GenHMapV5CV(RCP::SRecipe& Rcp) {
	//int sz = size();
	int sz = sizeCV();
	if (!Im16um.Create(wd, ht)) return false;
	CVIm16um = cv::Mat::zeros(ht, wd, CV_32F);

	WLI::SPSpar PsP;
	const WLI::FRP Ch = WHTA;

	PsP.SetConst(wlen_um[WLI::REDA], wlen_um[WLI::GRNA], wlen_um[WLI::WHTA], UStep_um);
	int inc = PsP.Inc[int(Ch)], inc2 = 2 * inc, inc3 = 3 * inc, inc4 = 4 * inc;
	int wdw = 2 * (inc - 1) + 1; if (wdw < 3) wdw = 3;
	bool bPChg = Rcp.bPChg;

	short nSmo = 0;
	if (Rcp.bSmo) nSmo = Rcp.nSmo;
	if (Rcp.bSmoHvy) nSmo = 3 * Rcp.nSmo;

	int idx = 0;
	int sz10 = sz / 8; if (sz10 < inc2) sz10 = inc2;
	//float xmx, xmn;
	float PS1sin, PS2sin, wlen = Strip.wlen_um[Ch], wlf;
	SROI R(sz), R1;
	R.EnsureValid(inc, sz);

	//if (ICC.isRegionType == ICC.LINE) { // 07122023
	//	if (ICC.isOriental == ICC.HORIZONTAL) ICC.y2 = ICC.y1;
	//	else ICC.x2 = ICC.x1;
	//}


	const int arraySize = 5;
	const int a[arraySize] = { 1, 2, 3, 4, 5 };
	const int b[arraySize] = { 10, 20, 30, 40, 50 };
	int c[arraySize] = { 0 };

	// Call CUDA function
	cudaAdd(c, a, b, arraySize);

	switch (Rcp.Mthd) {
	case RCP::PS0:
		PsP.SetConst(Strip.wlen_um[WLI::REDA], Strip.wlen_um[WLI::GRNA], Strip.wlen_um[WLI::WHTA], Strip.UStep_um);
		inc = PsP.Inc[int(Ch)]; if (inc < 1) { ASSERT(0); return false; }
		inc2 = 2 * inc; inc3 = 3 * inc; inc4 = 4 * inc;
#pragma omp parallel for
		for (int y = 0; y < ht; y++) { // 05302023 - Mortuja
			//for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			int idx;
			WLI::SFrng F; SROI R1, R2;
			SStat* pSt = &F.Z.St[Ch];
			for (int x = 0; x < wd; x++) { // 05302023 - Mortuja
				//for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCHCV(F, x, y, R, Ch);
				if (!Rcp.bFindPChg) {
					if (bPChg) idx = pSt->imn; else idx = pSt->imx;
				}
				else {
					if ((pSt->fmax - pSt->fave) >= (pSt->fave - pSt->fmin)) {
						idx = pSt->imx; bPChg = false;
					}
					else { idx = pSt->imn; bPChg = true; }
				}
				//if (!R.InRange(idx)) { Im16um.SetPixel(x, y, BADDATA); continue; }
				if (!R.InRange(idx)) { CVIm16um.at<float>(y, x) = BADDATA; continue; }//20250916
				if (nSmo) {
					if (R1.SetA(idx - sz10, idx + sz10, inc, sz)) {
						F.Smooth(Ch, nSmo, 3, R1);
					}
				}
				R2.SetI(idx, inc3, sz);
				//if (!R2.EnsureValid(inc, sz)) { Im16um.SetPixel(x, y, BADDATA); continue; }
				float Rsl;
				if (F.PhasePV5(Ch, WLI::PHS1, PsP, R2)) {
					Rsl = F.PeakPhas(WLI::PHS1, idx - 2, idx + 2, bPChg, sz);
				}
				else Rsl = BADDATA;
				//Im16um.SetPixel(x, y, Rsl);
				CVIm16um.at<float>(y, x) = Rsl;//20250916
			}
		}
		break;
	case RCP::PSI:
		PsP.Set(Ch, Strip.wlen_um[Ch], Strip.UStep_um);
		wlf = Strip.wlen_um[Ch] / 2.f * PIE;
		inc = PsP.Inc[int(Ch)]; if (inc < 1) ASSERT(0);
		inc2 = 2 * inc; inc3 = 3 * inc; inc4 = 4 * inc;
		PS1sin = PsP.PSsin[Ch];
		idx = GetMaxPeakIdx(wd / 2, ht / 2, Ch);
		if (idx < (inc2) || idx >(sz - inc2)) idx = sz / 2;
		R.Set(idx - inc3, idx + inc3);
#pragma omp parallel for
		//for (int y = 0; y < ht; y++) {
		for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			WLI::SFrng F;
			//for (int x = 0; x < wd; x++) {
			for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (nSmo) { F.Smooth(Ch, nSmo, 3, R); }
				//Im16um.SetPixel(x, y, F.PeakPSI5(Ch, idx, inc, PS1sin, sz));
				CVIm16um.at<float>(y, x) = F.PeakPSI5(Ch, idx, inc, PS1sin, sz); //20250916
			}
		}
		/*if (Rcp.bUnwrap)*/ Im16um.Unwrap(IMGL::LR);
		Im16um.Mult(wlf);
		break;
	case RCP::TW1:
		PsP.SetConst(Strip.wlen_um[REDA], Strip.wlen_um[GRNA], Strip.wlen_um[WHTA], Strip.UStep_um);
		inc = PsP.Inc[int(WLI::GRNA)]; if (inc < 1) ASSERT(0);
		inc2 = 2 * inc;
		idx = ICC.nIdx;
		if ((idx < inc2) || (idx >= sz - inc2)) idx = sz / 2;
		PS1sin = PsP.PSsin[FRP::REDA]; PS2sin = PsP.PSsin[FRP::GRNA];
		R1.SetA(idx - sz10, idx + sz10, inc, sz);
#pragma omp parallel for
		//for (int y = 0; y < ht; y++) {
		for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			WLI::SFrng F; SROI R1; float phG = 0;
			//for (int x = 0; x < wd; x++) {
			for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (nSmo) {
					F.Smooth(WLI::REDA, nSmo, 3, R);
					F.Smooth(WLI::GRNA, nSmo, 3, R);
				}
				Im16um.SetPixel(x, y, F.TW1Z(F.PhaseI5(PsP, idx, phG), phG, PsP));
			}
		}
		break;
	case RCP::VIS:
	default:
		PsP.Set(Ch, Strip.wlen_um[Ch], Strip.UStep_um);
		inc = PsP.Inc[int(Ch)]; if (inc < 1) ASSERT(0);
		inc2 = 2 * inc; inc4 = int(2.5 * inc);
		//xmn = Strip.Imgs[0]->PzPos_um;
		//xmx = Strip.Imgs[sz - 1]->PzPos_um;
#pragma omp parallel for
		//for (int y = 0; y < ht; y++) {
		for (int y = ICC.y1; y <= ICC.y2; y++) { // Height // 05302023 - Mortuja
			WLI::SFrng F; SROI Rt, R1;
			SStat* pSt1 = &F.Z.St[Ch];
			SStat* pSt2 = &F.Z.St[WLI::VIS1];
			//for (int x = 0; x < wd; x++) {
			for (int x = ICC.x1; x <= ICC.x2; x++) { // Width // 05302023 - Mortuja
				CollectZCH(F, x, y, R, Ch);
				if (bPChg) R1.SetI(F.Z.St[Ch].imn, inc4, sz);
				else R1.SetI(F.Z.St[Ch].imx, inc4, sz);
				F.PhasePV5(Ch, WLI::PHS1, PsP, R1);
				//F.VisiV5(Ch, PsP, R1);
				F.Smooth(WLI::VIS1, 1, 7, R1);
				Rt.SetI(F.Z.St[WLI::VIS1].imx, inc2, sz);
				float v = F.PeakGrad(WLI::VIS1, Rt.i1, Rt.i2, sz);
				Im16um.SetPixel(x, y, v);
			}
		}
		break;
	}
	return true;
}

bool WLI::CStrip::GenHMapFom(int Idx, std::wstring& log) {
	// Height measurement base on visibility [3/14/2022 FSM]
	int wd, ht, bpp;
	if (!GetDim(wd, ht, bpp)) return false;
	if (!Im16um.Create(wd, ht)) return false;

	log = _T("Height measurement base on visibility\r\n");

	//////////////////////////////////////////////////////////////////////////
	// Initialization: calculate center wavelength, phase shift distance in
	// rad & nm
	int wd2 = wd / 2, ht2 = ht / 2;
	int sz = int(Imgs.size()); if (sz < 9) {
		log += _T("Error: image group size is < 9\r\n");
		return false;
	}
	int nQ = 1, cnt = 0, sampling = 24;
	float sum1 = 0, sum2 = 0; // , sum3 = 0;

	SFrng F({ WLI::ZAXS, WLI::REDA, WLI::GRNA, WLI::BLUA }, sz);
	SROI R(sz), Rx;
	std::vector<SSStat> SSt; SSt.resize(2);

	int iChr = int(WLI::REDA);
	int iChg = int(WLI::GRNA);

	int nChr = int(IMGL::REDC);
	int nChg = int(IMGL::GRNC);

	//////////////////////////////////////////////////////////////////////////
	// data reduction
	CollectZCH(F, wd2, ht2, R, WLI::GRNA);
	int pkpos;
	if (Rcp.bPChg) pkpos = F.Z.St[nChg].imn; else pkpos = F.Z.St[nChg].imx;
	int st = int(pkpos - 6 * SSt[nChg].wavelength_um);
	int ed = int(pkpos + 6 * SSt[nChg].wavelength_um);
	if (st < 0) st = 0; if (ed > sz) ed = sz;
	Rx.Set(st, ed);

	//////////////////////////////////////////////////////////////////////////
	// try to estimate phase shift and wavelength
	float DStep = F.DStep();
	for (int y = sampling; y < (ht - sampling); y += sampling) {
		for (int x = sampling; x < (wd - sampling); x += sampling) {
			CollectZRG(F, x, y, R);
			sum1 += F.Wavelength(WLI::REDA, nQ);
			sum2 += F.Wavelength(WLI::GRNA, nQ);
			cnt++;
		}
	}
	F.Stats(SSt[iChr], DStep, sum1 / float(cnt));
	F.Stats(SSt[iChg], DStep, sum2 / float(cnt));

	log += _T("Micron per step = ") + std::to_wstring(SSt[iChr].MicronPerStep_um * 1e3) + _T(", ");
	log += std::to_wstring(SSt[iChg].MicronPerStep_um * 1e3);
	log += _T(" nm per step\r\n");

	log += _T("Wavelength = ") + std::to_wstring(SSt[iChr].wavelength_um * 1e3) + _T(", ");
	log += std::to_wstring(SSt[iChg].wavelength_um * 1e3);
	log += _T(" nm.\r\n");
	log += _T("Phase Shift = ") + std::to_wstring(SSt[iChr].PShift_um * 1e3) + _T(" nm.\r\n\r\n");

	//////////////////////////////////////////////////////////////////////////
	std::vector<WLI::SInc> InC; InC.resize(2);
	InC[0].sdStep = SSt[0].sdStep;
	int inc = InC[0].inc = SSt[0].inc;
	InC[0].inc2 = 2 * inc;

	InC[1].sdStep = SSt[1].sdStep;
	int v2 = InC[1].inc = SSt[1].inc;
	InC[1].inc2 = 2 * v2;
	if (v2 > inc) inc = v2;

	float lmE = F.EqvWL(SSt[0].wavelength_um, SSt[1].wavelength_um);
	float lmG = SSt[1].wavelength_um;
	float lmG1 = lmE / PIE2;
	float lmG2 = PIE2 * SSt[1].wavelength_um / lmE;

	int idx = ICC.nIdx; if ((idx < 8) || (idx > sz - 8)) idx = sz / 2;
	R.idx = idx;
	R.i1 = idx - 2 * inc; R.i2 = idx + 2 * inc;
	if ((R.i1 < 2 * inc) || (R.i2 > sz - 2 * inc)) return false;

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		SFrng F({ WLI::ZAXS,WLI::REDA,WLI::GRNA,WLI::BLUA }, sz);
		for (int x = 0; x < wd; x++) {
			if (CollectZRG(F, x, y, R)) {
				Im16um.SetPixel(x, y, F.TWM5(InC, lmG1, lmG2, lmG, lmE, R));
				continue;
			}
			Im16um.SetPixel(x, y, BADDATA);
		}
	}
	return true;
}

bool WLI::CStrip::GenHMapEnv(std::wstring& log) {
	// Height measurement base on visibility [3/14/2022 FSM]
	log = _T("Height measurement base on visibility\r\n");

	int wd, ht, bpp;
	if (!GetDim(wd, ht, bpp)) return false;
	if (!Im16um.Create(wd, ht)) return false;

	//////////////////////////////////////////////////////////////////////////
	// Initialization: calculate center wavelength, phase shift distance in
	// rad & nm
	int wd2 = wd / 2, ht2 = ht / 2;
	int sz = int(Imgs.size()); if (sz < 9) {
		log += _T("Error: image group size is < 9\r\n");
		return false;
	}
	float sum = 0;
	int nQ = 1, cnt = 0, sampling = 64;
	SFrng F({ WLI::ZAXS,WLI::REDA,WLI::GRNA,WLI::BLUA }, sz);
	SROI R(sz);
	std::vector<SSStat>SSt; SSt.resize(2);
	//////////////////////////////////////////////////////////////////////////
	// try to estimate phase shift and wavelength

	WLI::FRP wCh = WLI::WHTA;
	int iChw = int(wCh);
	WLI::FRP nCh1 = WLI::REDA;
	int iCh1 = int(nCh1);

	for (int y = sampling; y < (ht - sampling); y += sampling) {
		for (int x = sampling; x < (wd - sampling); x += sampling) {
			CollectZCH(F, x, y, R, wCh);
			sum += F.Wavelength(nCh1, nQ);
			cnt++;
		}
	}
	F.Stats(SSt[iCh1], F.DStep(), sum / float(cnt));

	//////////////////////////////////////////////////////////////////////////
	log += _T("Micron per step = ") + std::to_wstring(SSt[iCh1].MicronPerStep_um * 1e3) + _T(" nm per step\r\n");
	log += _T("Wavelength = ") + std::to_wstring(SSt[iCh1].wavelength_um * 1e3) + _T(" nm.\r\n");
	log += _T("Phase Shift = ") + std::to_wstring(SSt[iCh1].PShift_um * 1e3) + _T(" nm.\r\n\r\n");
	//////////////////////////////////////////////////////////////////////////

	int inc = SSt[iCh1].inc;
	if (inc < 1) inc = 1; if (inc > 16) inc = 16;
	SInc InC;
	InC.inc = inc; InC.inc2 = 2 * inc; InC.sdStep = SSt[iCh1].sdStep;

#pragma omp parallel for
	for (int y = 0; y < ht; y++) {
		int st, ed, pkpos;
		SROI R1; SFrng F({ WLI::ZAXS, WLI::REDA,WLI::GRNA,WLI::BLUA }, sz);
		for (int x = 0; x < wd; x++) {
			if (!CollectZCH(F, x, y, R, wCh)) continue;
			if (Rcp.bPChg) pkpos = F.Z.St[iChw].imn; else pkpos = F.Z.St[iChw].imx;
			st = pkpos - 3 * inc; ed = pkpos + 3 * inc;
			if ((st >= (2 * inc)) && (ed <= (sz - 2 * inc))) {
				R1.i1 = st; R1.i2 = ed;
				F.Visi5(nCh1, InC, R1);
				F.Smooth5(WLI::VIS1, 1, R1);
				Im16um.SetPixel(x, y, F.XatY0(WLI::VIS1, F.Z.St[WLI::VIS1].imx, 2));
			}
			else Im16um.SetPixel(x, y, BADDATA);
		}
	}
	return true;
}

bool WLI::CStrip::Save16(std::wstring& outfile) {
	if (Im16um.IsNull()) return false;
	//Im16um.Level();
	//float mx, mn; Im16um.GetMaxMin2(mx, mn);
	Im16um.Save(outfile.c_str());
	std::wstring str = outfile.substr(0, outfile.rfind('.')) + _T(".BMP");
	Im16um.SaveBMP(str, 100);
	return true;
}

bool WLI::CStrip::Load(const std::wstring& StripName) {
	CFile theFile;
	if (theFile.Open(StripName.c_str(), CFile::modeRead)) {
		Clear();
		CArchive archive(&theFile, CArchive::load);
		Serialize(archive);
		archive.Close(); theFile.Close();
		return true;
	}
	return false;
}

bool WLI::CStrip::Save(const std::wstring& StripName) {
	CFile theFile;
	if (theFile.Open(StripName.c_str(), CFile::modeCreate | CFile::modeWrite)) {
		CArchive archive(&theFile, CArchive::store);
		Serialize(archive);
		archive.Close(); theFile.Close();
		return true;
	}
	return false;
}

bool WLI::CStrip::ExportBMP(const TCHAR* fname) {
	int sz = int(Strip.size()); if (sz < 1) return false;
	std::wstring namex;
	std::wstring name = DosUtil.RemoveExtension(fname);
	std::wstring name1 = DosUtil.ExtractFilename(fname);
	_wmkdir(name.c_str());
	name = name + L"\\" + name1;
	for (int i = 0; i < sz; i++) {
		namex = name + L"_" + std::to_wstring(i + 1) + L".BMP";
		Strip.Imgs[i]->Im.Save(namex.c_str());
	}
	namex = name + L".CSV";
	Dump(namex.c_str());
	return true;
}

bool WLI::CStrip::Dump(const TCHAR* fname) {
	int sz = Strip.size(); if (sz < 1) return false;
	FILE* fp = _wfopen(fname, _T("wb")); if (!fp) return false;
	fprintf(fp, "Strip #,Z pos\n");
	for (int i = 0; i < sz; i++) {
		WLI::SIms* p = Strip.GetIms(i);
		fprintf(fp, "%d,%.4f\n", i + 1, p->PzPos_um);
	}
	fclose(fp);
	return true;
}

bool WLI::CStrip::Dump(const TCHAR* fname, SFrng& F) {
	FILE* fp = _wfopen(fname, _T("wb")); if (!fp) return false;
	F.dump(fp);
	fclose(fp);
	return true;
}

bool WLI::CStrip::Dump(const TCHAR* fname, SFrng& F, WLI::SPSpar& PsP) {
	FILE* fp = _wfopen(fname, _T("wb")); if (!fp) return false;
	PsP.dump(fp); F.dump(fp);
	fprintf(fp, "\n");
	fclose(fp);
	return true;
}

bool WLI::CStrip::Dump(const TCHAR* fname, SFrng& F, std::vector<SSStat>& SSt) {
	FILE* fp = _wfopen(fname, _T("wb")); if (!fp) return false;
	int iChr = int(WLI::REDA);
	fprintf(fp, "Wavelength,%.1f,nm\ndStep,%.1f,nm\n",
		SSt[iChr].wavelength_um * 1e3, SSt[iChr].MicronPerStep_um * 1e3);
	int sz = int(SSt.size());
	for (int i = 0; i < sz; i++) {
		fprintf(fp, "Stats %d\n", i + 1);
		SSt[i].Dump(fp);
	}
	fprintf(fp, "\n");
	F.dump(fp);
	fclose(fp);
	return true;
}

WLI::CStrip::CStrip() {}

WLI::CStrip::~CStrip() {
	DeallocAll();
}

void WLI::CStrip::Clear() {
	int sz = int(Imgs.size());
	for (int i = 0; i < sz; i++) {
		delete Imgs[i];
	}
	Imgs.clear();
}

void WLI::CStrip::Serialize(CArchive& ar) {
	USHORT magic = 4;
	if (ar.IsStoring()) {
		ar << magic;
		ar << NSlice;
		size_t sz = Imgs.size();
		ar << sz;
		for (size_t i = 0; i < sz; i++) {
			Imgs[i]->Serialize(ar);
		}
		ImBG.Serialize(ar);
		ar << xPos;
		ar << yPos;
		ar << zPos;
		ar << UStep_um;
		ar << zrange_um;
		Im16um.Serialize(ar);
	}
	else {
		ar >> magic;
		if (magic > 1) { ar >> NSlice; }
		else NSlice = 1;
		size_t sz;
		ar >> sz;
		if (sz > 0) DeallocAll();
		for (size_t i = 0; i < sz; i++) {
			SIms* pIm = new SIms;
			pIm->Serialize(ar);
			Imgs.push_back(pIm);
		}
		if (magic > 2) {
			ImBG.Serialize(ar);
		}
		if (magic > 3) {
			ar >> xPos;
			ar >> yPos;
			ar >> zPos;
			ar >> UStep_um;
			ar >> zrange_um;
			Im16um.Serialize(ar);
		}
	}
}

bool WLI::CStrip::ExpAnalysis(const TCHAR* fname, int x, int y) {
	int sz = size(); if (sz < 1) return false;

	SROI R(sz);
	WLI::SFrng F;
	WLI::SPSpar PsP;
	WLI::FRP Ch = WLI::WHTA;

	switch (Rcp.Mthd) {
	case RCP::TW1:
		PsP.SetConst(wlen_um[WLI::REDA], wlen_um[WLI::GRNA], Strip.wlen_um[WHTA], UStep_um);
		break;
	default: PsP.Set(Ch, wlen_um[Ch], UStep_um); break;
	}

	const int inc = PsP.Inc[int(Ch)];
	float Rsl = BADDATA;
	int wdw = 3;
	CollectRGBW(F, x, y, R);
	if (Ch == WLI::WHTA) F.MakeWhite(R, false, 0);

	short nSmo = 0;
	if (Rcp.bSmo) Rcp.nSmo;
	if (Rcp.bSmoHvy) nSmo = 3 * Rcp.nSmo;

	if (nSmo) F.Smooth(WLI::REDA, nSmo, wdw, R);
	if (nSmo) F.Smooth(WLI::GRNA, nSmo, wdw, R);
	if (nSmo) F.Smooth(WLI::BLUA, nSmo, wdw, R);
	if (nSmo) F.Smooth(WLI::WHTA, nSmo, wdw, R);

	F.PeakTW1ex(sz / 2, PsP, R, sz);

	std::wstring fname1 = DosUtil.RemoveExtension(fname);
	fname1 += L"-a.CSV";
	return Dump(fname1.c_str(), F, PsP);
}