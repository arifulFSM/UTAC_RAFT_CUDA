#include "pch.h"

#include <iostream>

#include "MTH/Ang.h"
#include "Frng.h"
#include "MTH/LSFit.h"

#define SIGNF(x) ((x) >= 0)

float WLI::SFrng::FakeIntensity(float z, float z0, float dz, float lc, float lm) {
	float v = (z - (z0 + dz));
	return expf(-((v * v) / (lc))) * cosf(PIE2 * (z - z0) / lm);
}

float WLI::SFrng::Interpolate(float x0, float x, float x1, float y0, float y1) {
	return ((x - x0) / (x1 - x0)) * (y1 - y0) + y0;
}

void WLI::SFrng::Stats(SSStat& SSt, float DStep, float wavelen) {
	short nQ = 2;
	SSt.MicronPerStep_um = DStep;
	SSt.wavelength_um = wavelen;
	SSt.PShift_um = SSt.wavelength_um / 4.f; // 90 deg phase shift
	SSt.inc = int(SSt.wavelength_um / SSt.MicronPerStep_um / 4.f);
	SSt.psd = PIE2 * SSt.MicronPerStep_um / SSt.wavelength_um;
	SSt.sdStep = sin(SSt.inc * SSt.psd);
}

void WLI::SFrng::Visi5(WLI::FRP Ch, SInc& InC, const SROI& R) {
	//! no sanity check [4/5/2022 yuenl]
	int st = R.i1, ed = R.i2;
	for (int i = st; i < ed; i++) {
		_Visi5(i, InC, Ch);
	}
}

bool WLI::SFrng::_Visi5(int ix, SInc& InC, WLI::FRP Ch) {
	//? no sanity check
	int sz = Z.size(Ch);
	int inc = InC.inc, inc2 = 2 * inc;
	float* pZ = Z.Get(Ch, ix, sz);
	float num = float(InC.sdStep * 2 * (*(pZ - inc) - *(pZ + inc)));
	float den = float(2 * (*pZ) - *(pZ + inc2) - *(pZ - inc2));
	*Z.Get(FRP::VIS1, ix, sz) = sqrtf(num * num + den * den);
	return true;
}

float WLI::SFrng::_Phase5(int ix, SInc& InC, WLI::FRP Ch1, WLI::FRP Ch2) {
	//? no sanity check
	int sz = Z.size(Ch1);
	float* pZ = Z.Get(Ch1, ix, sz);
	int inc = InC.inc, inc2 = InC.inc2;
	float num = float(InC.sdStep * 2 * (*(pZ - inc) - (*(pZ + inc))));
	float den = float(2 * (*pZ - *(pZ + inc2) - (*(pZ - inc2))));
	return *Z.Get(Ch2, ix, sz) = atan2(num, den);
}

void WLI::SFrng::Reset() {
	bZeroed = false;
}

float WLI::SFrng::DStep() {
	CLSFit Fit;
	int sz = int(Z.size());
	float* p = Z.Get(FRP::ZAXS, 0, sz);
	for (int i = 0; i < sz; i++, p++) {
		if (*p > 0) Fit.Add(float(i), *p);
	}
	Fit.Calc();
	return float(Fit.M);
}

float WLI::SFrng::EqvWL(float l1, float l2) {
	return l1 * l2 / abs(l1 - l2);
}

float WLI::SFrng::Wavelength(WLI::FRP Ch, short nQ) {
	//! no sanity check
	// assume non zero mean fringe
	int sz = Z.size(Ch), sz2 = sz - 2;
	float nlamda = (2 * (nQ - 1) + 1) / 2.f;
	int ist = BADDATA, ied = BADDATA;
	float ave = 0, st = BADDATA, ed = BADDATA;

	SStat& S = Z.St[int(Ch)];
	ave = S.fave;

	// left side
	short n = 0;
	float* pA = Z.Get(Ch, S.imx, sz);
	float* pZ = Z.Get(FRP::ZAXS, S.imx, sz);
	bool sign = SIGNF(*pA - ave);
	for (int i = S.imx; i >= 2; i--, pA--, pZ--) {
		if (sign == SIGNF(*pA - ave)) continue;
		sign = !sign; n++;
		if (n < nQ) continue;
		ist = i;
		st = Interpolate(*pA, ave, *(pA + 1), *pZ, *(pZ + 1));
		break;
	}
	if (st == BADDATA) return 0;

	// right side
	n = 0;
	pA = Z.Get(Ch, S.imx, sz);
	pZ = Z.Get(FRP::ZAXS, S.imx, sz);
	sign = SIGNF(*pA - ave);
	for (int i = S.imx; i < sz2; i++, pA++, pZ++) {
		if (sign == SIGNF(*pA - ave)) continue;
		sign = !sign; n++;
		if (n < nQ) continue;
		ied = i;
		ed = Interpolate(*(pA - 1), ave, *pA, *(pZ - 1), *pZ);
		break;
	}
	if (ed == BADDATA) return 0;
	return (ed - st) / nlamda;
}

/*
	Ch -> channel name
	n -> number of iterations for smoothing
	wdw -> window size
	R -> number of image
*/
bool WLI::SFrng::Smooth(WLI::FRP Ch, int n, int wdw, const SROI& R) {
	if (!n) return false;
	wdw /= 2;
	int sz = Z.size();
	int st = R.i1, ed = R.i2;
	if (st < wdw) st = wdw; if (ed > (sz - wdw)) ed = sz - wdw;

	memset(Z.Get(FRP::TMP1, 0, sz), 0, sz * sizeof(float));
	for (int j = 0; j < n; j++) {
		int a, b, cnt;
		float sum;
		float* tb, * ta = Z.Get(FRP::TMP1, st, sz);
		float* pa = Z.Get(Ch, st - wdw, sz);
		float* pb = Z.Get(Ch, st, sz); tb = ta;
		// Smooth the data through stack of image in specific window size
		for (int i = st; i < ed; i++, pa++, tb++) {
			a = i - wdw; b = i + wdw; 
			float* p = pa; sum = 0; cnt = 0;
			for (int k = a; k <= b; k++, p++) {
				if (*p == BADDATA) continue;
				sum += *p; cnt++;
			}
			if (cnt) *tb = sum / float(cnt); else *tb = BADDATA;
		}
		memcpy(pb, ta, (ed - st) * sizeof(float));
	}
	// calculate max min or average for smoothed value
	if (Ch < WLI::WHTA + 1) MaxMin(Ch, R, sz, false);
	else MaxMin(Ch, R, sz, true);
	return true;
}

void WLI::SFrng::Smooth5(FRP Ch, int n, const SROI& R) {
	// limited sanity check
	int sz = Z.size();
	int st = R.i1, ed = R.i2;
	if (st < 2) st = 2; if (ed > (sz - 2)) ed = sz - 2;
	float* p, * t;
	for (int j = 0; j < n; j++) {
		p = Z.Get(Ch, st, sz);
		t = Z.Get(FRP::TMP1, st, sz);
		for (int i = st; i < ed; i++, p++, t++) {
			*t = (*(p - 2) + *(p - 1) + *p + *(p + 1) + *(p + 2)) / 5.f;
		}
		memcpy(Z.Get(Ch, st, sz), Z.Get(FRP::TMP1, st, sz), (ed - st) * sizeof(float));
	}
	if (Ch < WLI::WHTA + 1) MaxMin(Ch, R, sz, false);
	else MaxMin(Ch, R, sz, true);
}

void WLI::SFrng::MakeWhite(const SROI& R, bool bBg, float ave) {
	// no sanity check
	int sz = Z.size();
	int st = R.i1, ed = R.i2;
	if (!bBg) ave = 0;
	float* pZ1 = Z.Get(FRP::REDA, st, sz);
	float* pZ2 = Z.Get(FRP::GRNA, st, sz);
	float* pZ3 = Z.Get(FRP::BLUA, st, sz);
	float* pZ4 = Z.Get(FRP::WHTA, st, sz);
	for (int i = st; i < ed; i++, pZ1++, pZ2++, pZ3++, pZ4++) {
		*pZ4 = (*pZ1 + *pZ2 + *pZ3) / 3.f;
		if (!bBg) ave += *pZ4;
	}
	if (!bBg) ave /= (ed - st);
	MaxMin(FRP::WHTA, R, sz, false);
	if (bBg) Z.St[FRP::WHTA].fave = ave;
}

void WLI::SFrng::FakData(float len, int div, float z0, float dz, int gain, int offset) {
	float lmr = 0.6328f / 2.f, lmg = 0.532f / 2.f, lmb = 0.4416f / 2.f, lc = 0.40f;
	float shift = lmg / float(div);
	int sz = int(len / shift + 0.5f); // 8 points per wavelength

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////
	int sz2 = sz / 2;
	int szz = 2 * sz2 + 1;
	resize({ WLI::ZAXS, WLI::REDA, WLI::GRNA,WLI::BLUA }, szz);

	int n = 0;
	float* pZ1 = Z.Get(FRP::REDA, 0, szz);
	float* pZ2 = Z.Get(FRP::GRNA, 0, szz);
	float* pZ3 = Z.Get(FRP::BLUA, 0, szz);
	float* pZ4 = Z.Get(FRP::WHTA, 0, szz);
	float* pX = Z.Get(FRP::ZAXS, 0, szz);
	for (int i = -sz2; i <= sz2; i++, pX++, pZ1++, pZ2++, pZ3++, n++) {
		float u, v = i * shift;
		*pX = v;
		u = gain * FakeIntensity(v, z0, dz, lc, lmr) + offset;
		*pZ1 = u;
		u = gain * FakeIntensity(v, z0, 0, lc, lmg) + offset;
		*pZ2 = u;
		u = gain * FakeIntensity(v, z0, dz, lc, lmb) + offset;
		*pZ3 = u;
		*pZ4 = (*pZ1 + *pZ2 * 2 + *pZ3) / 4.f;
	}
	SROI R(sz);
	MaxMin(FRP::REDA, R, sz, false);
	MaxMin(FRP::GRNA, R, sz, false);
	MaxMin(FRP::BLUA, R, sz, false);
	MaxMin(FRP::WHTA, R, sz, false);
}

float WLI::SFrng::XatY0(WLI::FRP Ch, int imx, int inc) {
	// no sanity check
	if (imx == BADDATA) return BADDATA;

	int sz = Z.size();
	int st = imx - inc, ed = imx + inc;
	CLSFit Fit;
	float dy;
	float* pZ = Z.Get(Ch, st, sz);
	float* pX = Z.Get(FRP::ZAXS, st, sz);
	float* pT = Z.Get(FRP::TMP1, st, sz);
	for (int i = st; i <= ed; i++, pT++, pX++, pZ++) {
		dy = *(pZ + 1) - *(pZ - 1);
		Fit.Add(*pX, dy);
		*pT = dy;
	}
	Fit.Calc();
	return float(-Fit.C / Fit.M);
}

int WLI::SFrng::BestMax(IMGL::eCH nCh) {
	float a = Z.St[int(nCh)].fmax - Z.St[int(nCh)].fave;
	float b = Z.St[int(nCh)].fave - Z.St[int(nCh)].fmin;
	if (a >= b) return Z.St[int(nCh)].imx;
	return Z.St[int(nCh)].imn;
}

float WLI::SFrng::TWM5(std::vector<SInc>& InC, float lmG1, float lmG2, float lmG, float lmE, const SROI& R) {
	int idx = R.idx;
	float pR = _Phase5(idx, InC[0], FRP::REDA, FRP::PHS1);
	float pG = _Phase5(idx, InC[1], FRP::GRNA, FRP::PHS2);
	float pE = pG - pR;
	if (pE >= PIE) pE -= PIE2; else if (pE <= -PIE) pE += PIE2;

	float sf = PIE2 * lmG2;
	float opdE = pE * lmE / PIE2;
	float opdG = pG + sf * int(pE / PIE2 * lmG1);

	if (abs(opdG - opdE) > lmG / 2) {
		if (opdG > opdE) {
			opdG = pG + sf * (int(pE / PIE2 * lmG1) - 1);
		}
		else if (opdG < opdE) {
			opdG = pG + sf * (int(pE / PIE2 * lmG1) + 1);
		}
	}

	int sz = Z.size();
	*Z.Get(WLI::RSLT, idx, sz) = opdG;
	return opdG;
}

float WLI::SFrng::PeakGrad(WLI::FRP Ch, int st, int ed, int sz) {
	if (st < 0) st = 0; if (ed >= sz - 1) ed = sz - 1;
	st++; ed--;
	float* v = Z.Get(FRP::VIS1, st, sz);
	float* z = Z.Get(FRP::ZAXS, st, sz);
	CLSFit Fit;
	for (int i = st; i <= ed; i++, v++, z++) {
		Fit.Add(*z, *(v + 1) - *(v - 1));
	}
	return float(Fit.GetX(0));
}

float WLI::SFrng::PeakPhas(WLI::FRP Ch, int st, int ed, bool bPChg, int sz) {
	//if (st < 2) return BADDATA;
	//if (ed >= (sz - 2)) return BADDATA;
	float* v = Z.Get(FRP::PHS1, st, sz);
	float* z = Z.Get(FRP::ZAXS, st, sz);
	CLSFit Fit;
	if (bPChg) {
		for (int i = st; i <= ed; i++, v++, z++) {
			if (*v > 0) Fit.Add(*z, *v - PIE2);
			else Fit.Add(*z, *v);
		}
		float rsl = -float(Fit.GetX(-PIE));
		if (rsl > 200.f) { /*ASSERT(0);*/ return BADDATA; }
		return rsl;
	}
	else {
		for (int i = st; i <= ed; i++, v++, z++) {
			Fit.Add(*z, *v);
		}
		float rsl = -float(Fit.GetX(0));
		if (rsl > 200.f) { /*ASSERT(0);*/ return BADDATA; }
		return rsl;
	}
	return BADDATA;
}

float WLI::SFrng::PhaseI5(WLI::SPSpar& PsP, int idx, float& PhG) {
	// no sanity check
	int sz = Z.size();
	int inc = PsP.Inc[int(FRP::REDA)];
	float* p1 = Z.Get(FRP::PHS1, idx, sz);
	float* v3 = Z.Get(FRP::REDA, idx, sz);
	float* v1 = v3 - 2 * inc, * v2 = v3 - inc;
	float* v4 = v3 + inc, * v5 = v3 + 2 * inc;
	*p1 = atan2(PsP.PSsin[FRP::REDA] * (*v2 - *v4), (*v3) * 2 - (*v5) - (*v1));

	float* p2 = Z.Get(FRP::PHS2, idx, sz);
	v3 = Z.Get(FRP::GRNA, idx, sz);
	inc = PsP.Inc[int(FRP::GRNA)];
	v1 = v3 - 2 * inc, v2 = v3 - inc;
	v4 = v3 + inc, v5 = v3 + 2 * inc;
	PhG = *p2 = atan2(PsP.PSsin[FRP::GRNA] * (*v2 - *v4), (*v3) * 2 - (*v5) - (*v1));

	float* p3 = Z.Get(FRP::PHSE, idx, sz);
	*p3 = *p2 - *p1;
	if (*p3 >= PIE) *p3 -= PIE2; else if (*p3 <= -PIE) *p3 += PIE2;

	return *p3;
}

bool WLI::SFrng::PhasePV5(WLI::FRP Ch, WLI::FRP Ph, WLI::SPSpar& PsP, const SROI& Ra) {
	int sz = Z.size();
	short inc = PsP.Inc[int(Ch)], inc2 = 2 * inc;
	SROI R = Ra;
	R.EnsureValid(inc, sz);
	int st = R.i1, ed = R.i2;

	float* p, * v, sn = PsP.Get(PsP, Ch);
	FRP ChP = FRP::PHS1, ChV = FRP::VIS1;
	if (Ph == FRP::PHS2) { ChP = FRP::PHS2; ChV = FRP::VIS2; }

	p = Z.Get(ChP, st, sz); v = Z.Get(ChV, st, sz);

	float N, D;
	float* p3 = Z.Get(Ch, st, sz);
	float* p1 = p3 - inc2, * p2 = p3 - inc;
	float* p4 = p3 + inc, * p5 = p3 + inc2;
	for (int i = st; i < ed; i++, p1++, p2++, p3++, p4++, p5++, p++, v++) {
		N = sn * (*p2 - *p4); D = *p3 * 2 - *p5 - *p1;
		*p = atan2(N, D);
		*v = sqrtf(N * N + D * D);
	}

	MaxMin(ChP, R, sz, true); MaxMin(ChV, R, sz, true);

	return true;
}

bool WLI::SFrng::VisiV5(WLI::FRP Ch, WLI::SPSpar& PsP, const SROI& R) {
	int sz = Z.size();
	int st = R.i1, ed = R.i2;

	short inc = PsP.Inc[int(Ch)], inc2 = 2 * inc;
	if (st < inc2) st = inc2;
	if (ed > (sz - inc2)) ed = sz - inc2;
	if ((ed - st) < 4 * inc) return false;

	float N, D;
	float* v = Z.Get(FRP::VIS1, st, sz);
	float* p3 = Z.Get(Ch, st, sz);
	float* p1 = p3 - inc2, * p2 = p3 - inc;
	float* p4 = p3 + inc, * p5 = p3 + inc2;
	float sn = 2 * sin(PsP.PSrd[FRP::GRNA]);
	for (int i = st; i < ed; i++, p1++, p2++, p3++, p4++, p5++, /*p++,*/ v++) {
		N = sn * (*p2 - *p4); D = *p3 * 2 - *p5 - *p1;
		//*p = atan2(N, D);
		*v = sqrt(N * N + D * D);
	}
	MaxMin(FRP::VIS1, R, sz, true);
	return true;
}

float WLI::SFrng::PeakPSI5(WLI::FRP Ch, int idx, int inc, float PSsin, int sz) {
	float* p = Z.Get(PHS1, idx, sz);
	float* p3 = Z.Get(Ch, idx, sz);
	float N = PSsin * (*(p3 - inc) - *(p3 + inc));
	float D = *p3 * 2 - *(p3 + 2 * inc) - *(p3 - 2 * inc);
	return *p = atan2(N, D);
}

bool WLI::SFrng::PeakTW1ex(int idx, WLI::SPSpar& PsP, const SROI& R, int sz) {
	int inc = PsP.Inc[int(FRP::GRNA)], inc2 = 2 * inc;
	int st = R.i1, ed = R.i2;
	if (st < inc2) st = inc2; if (ed > (sz - inc2)) ed = sz - inc2;

	float N, D;
	float* v1, * v2, * v3, * v4, * v5;
	float* ph1, * ph2, * phe, * vs1, * vs2, * ord, * rsl, * Zg, * Ze;
	//////////////////////////////////////////////////////////////////////////
	ph1 = Z.Get(FRP::PHS1, st, sz);
	vs1 = Z.Get(FRP::VIS1, st, sz);
	v3 = Z.Get(FRP::REDA, st, sz);
	v1 = v3 - inc2, v2 = v3 - inc;
	v4 = v3 + inc, v5 = v3 + inc2;
	for (int i = st; i < ed; i++,
		v1++, v2++, v3++, v4++, v5++, ph1++, vs1++) {
		N = PsP.PSsin[FRP::REDA] * (*v2 - *v4); D = (*v3) * 2 - (*v5) - (*v1);
		*ph1 = atan2(N, D); *vs1 = sqrtf(N * N + D * D);
	}
	MaxMin(WLI::PHS1, R, sz, true);
	MaxMin(WLI::VIS1, R, sz, true);
	//////////////////////////////////////////////////////////////////////////
	ph2 = Z.Get(FRP::PHS2, st, sz);
	vs2 = Z.Get(FRP::VIS2, st, sz);
	v3 = Z.Get(FRP::GRNA, st, sz);
	v1 = v3 - inc2, v2 = v3 - inc;
	v4 = v3 + inc, v5 = v3 + inc2;
	for (int i = st; i < ed; i++, v1++, v2++, v3++, v4++, v5++, ph2++, vs2++) {
		N = PsP.PSsin[FRP::GRNA] * (*v2 - *v4); D = (*v3) * 2 - (*v5) - (*v1);
		*ph2 = atan2(N, D); *vs2 = sqrtf(N * N + D * D);
	}
	MaxMin(WLI::PHS2, R, sz, true);
	MaxMin(WLI::VIS2, R, sz, true);
	//////////////////////////////////////////////////////////////////////////
	ph1 = Z.Get(FRP::PHS1, st, sz);
	ph2 = Z.Get(FRP::PHS2, st, sz);
	phe = Z.Get(FRP::PHSE, st, sz);
	ord = Z.Get(FRP::ORDR, st, sz);
	rsl = Z.Get(FRP::RSLT, st, sz);
	Zg = Z.Get(FRP::TMP1, st, sz);
	Ze = Z.Get(FRP::TMP2, st, sz);
	float sf = PsP.sf1 * PsP.sf2;
	float sfg = PsP.WL2_um / PIE4;
	float sfe = PsP.WLE_um / PIE4;
	for (int i = st; i < ed; i++, ph1++, ph2++, phe++, ord++, rsl++, Zg++, Ze++) {
		*phe = (*ph2) - (*ph1);
		if (*phe >= PIE) *phe -= PIE2; else if (*phe <= -PIE) *phe += PIE2;
		*ord = float(int(((*ph2) - sf * (*phe)) / PIE2) + 0.5f);
		*rsl = ((*ph2) - PIE2 * (*ord)) * sfg;
		//*rsl = ((*ph2) - PIE2 * (*ord)) / sfg;
		*Zg = (*ph2) * sfg;
		*Ze = (*phe) * sfe;
	}
	Smooth(WLI::PHSE, 3, 3, R);
	MaxMin(WLI::PHSE, R, sz, true);
	MaxMin(WLI::ORDR, R, sz, true);
	MaxMin(WLI::RSLT, R, sz, true);
	MaxMin(WLI::TMP1, R, sz, true);
	MaxMin(WLI::TMP2, R, sz, true);
	//////////////////////////////////////////////////////////////////////////
	return *Z.Get(FRP::RSLT, idx, sz);
}

float WLI::SFrng::TW1Z(float phEU, float phG, WLI::SPSpar& PsP) {
	return (phG - PIE2 * int((phG - PsP.sf1 * PsP.sf2 * phEU) / PIE2)) / PsP.sf1;
}

float WLI::SFrng::TW1Zex(int sz, float phEU, float phG, float& Ord, WLI::SPSpar& PsP) {
	Ord = float(int((phG - PsP.sf1 * PsP.sf2 * phEU) / PIE2));
	return (phG - PIE2 * int(Ord)) / PsP.sf1;
}

void WLI::SFrng::dump(FILE* fp) {
	fprintf(fp, "Ave1,%.4f,Max1,%.4f,Min1,%.4f\n",
		Z.St[int(IMGL::REDC)].fave, Z.St[int(IMGL::REDC)].fmax, Z.St[int(IMGL::REDC)].fmin);
	fprintf(fp, "Ave2,%.4f,Max2,%.4f,Min2,%.4f\n\n",
		Z.St[int(IMGL::GRNC)].fave, Z.St[int(IMGL::GRNC)].fmax, Z.St[int(IMGL::GRNC)].fmin);

	int sz = int(Z.Get(FRP::ZAXS).size()); if (sz < 1) return;

	std::vector<int>Col;
	for (int i = 0; i < FRP::LASTF - 1; i++) {
		if (Z.Get(FRP(i)).size() > 0) Col.push_back(i);
	}
	int szCol = int(Col.size()); if (szCol < 1) return;

	fprintf(fp, "Index,Position");
	for (int i = 0; i < szCol; i++) {
		fprintf(fp, ",%s", Z.FrpHdr[Col[i]].c_str());
	}
	fprintf(fp, "\n");

	for (int i = 0; i < sz; i++) {
		fprintf(fp, "%d,%.4f", i, *Z.Get(FRP::ZAXS, i, sz));
		for (int j = 0; j < szCol; j++) {
			fprintf(fp, ",%.4f", *Z.Get(FRP(Col[j]), i, sz));
		}
		fprintf(fp, "\n");
	}
}

void WLI::SFrng::dump(FILE* fp, WLI::FRP idx, const SROI& R) {
	int sz = Z.size(); if (sz < 1) return;

	SZp zp;
	fprintf(fp, "Index, Position,");
	fprintf(fp, "%s\n", zp.FrpHdr[int(idx)].c_str());
	fprintf(fp, "\n");

	std::vector<float*>pZ;
	for (int i = 0; i < int(IMGL::LST); i++) {
		pZ.push_back(Z.Get(FRP(i), idx, sz));
	}
	float** pF = &pZ[0];
	for (int i = R.i1; i < R.i2; i++, pF++) {
		fprintf(fp, "%d,%.4f", i, **(pF + int(FRP::ZAXS)));
		fprintf(fp, ",%.4f\n", **pF);
	}
}

void WLI::SFrng::dump(const char* fname) {
	FILE* fp = fopen(fname, "wb"); if (!fp) return;
	dump(fp);
	fclose(fp);
}

void WLI::SFrng::dump(const char* fname, WLI::FRP idx, const SROI& R) {
	FILE* fp = fopen(fname, "wb"); if (!fp) return;
	dump(fp, idx, R);
	fclose(fp);
}

void WLI::SFrng::resize(WLI::FRP Ch, int n) {
	Z.resize(Ch, n); Reset();
}

void WLI::SFrng::MaxMin(WLI::FRP Ch, const SROI& R, int sz, bool bAve) {
	int st = R.i1, ed = R.i2;
	float* p = Z.Get(Ch, st, sz);
	while (*p == BADDATA) { st++, p++; continue; }
	int n = 0, imax = st, imin = st;
	float v, fmax = *p, fmin = *p; double ave = 0;
	for (int i = st; i < ed; i++, p++) {
		if (*p == BADDATA) continue;
		v = *p;
		if (v > fmax) { fmax = v; imax = i; }
		if (v < fmin) { fmin = v; imin = i; }
		if (bAve) { ave += v; n++; }
	}
	SStat& Stt = Z.St[Ch];
	Stt.fmax = fmax; Stt.fmin = fmin;
	Stt.imx = imax; Stt.imn = imin;
	if (bAve) Stt.fave = float(ave / float(n));
}

WLI::SFrng::SFrng() {}

WLI::SFrng::SFrng(int sz) {
	for (int i = 0; i<int(WLI::LASTF); i++) {
		Z.resize(WLI::FRP(i), sz);
	}
}

WLI::SFrng::SFrng(std::vector<WLI::FRP> args, int sz) {
	for (auto elem : args) {
		Z.resize(elem, sz);
	}
}

void WLI::SFrng::resize(int sz) {
	for (int i = 0; i<int(WLI::LASTF); i++) {
		Z.resize(WLI::FRP(i), sz);
	}
}

void WLI::SFrng::resize(std::vector<WLI::FRP> args, int sz) {
	for (auto elem : args) {
		Z.resize(elem, sz);
	}
}