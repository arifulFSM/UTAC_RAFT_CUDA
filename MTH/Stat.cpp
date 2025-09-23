#include "pch.h"

#include <cmath>
#include <stdio.h>

#include "Stat.h"

SStat::SStat() {
	Clear();
}

SStat::~SStat() {}

void SStat::Dump(FILE* fp) {
	if (!fp) return;
	fprintf(fp, "Statistics:\n");
	fprintf(fp, "Average,%.3f\n", Ave);
	fprintf(fp, "Std.Dev,%.4f\n", Stdev);
	fprintf(fp, "TTV,%.3f\n", dY);
	fprintf(fp, "Maximum,%.3f\n", Max);
	fprintf(fp, "Minimum,%.3f\n\n", Min);
}

void SStat::Calculate() {
	if (N == 0) { Ave = 0; Stdev = 0; Max = Min = dY = 0; return; }
	double v = N * sum2 - sum * sum;
	Ave = float(sum / double(N));
	if ((N > 1) && (v >= 0)) Stdev = float(sqrt(float(v / (N * (N - 1)))));
	else Stdev = 0;
}

void SStat::CalcStat(float* pBuf, int N) {
	if (!pBuf || N < 1) return;
	Clear();
	this->N = N;
	Max = Min = *pBuf;
	for (int i = 0; i < N; i++, pBuf++) {
		float v = *pBuf;
		sum += v; sum2 += (v * v);
		if (v < Min) Min = v;
		if (v > Max) Max = v;
	}
	dY = Max - Min;
	Calculate();
}

void SStat::Clear() {
	N = 0;
	sum = sum2 = 0;
	Stdev = Ave = 0;
	Min = 0; Max = 0; dY = 0;
}

void SStat::Add(float v) {
	if (N == 0) Max = Min = v;
	sum += v; sum2 += (v * v);
	if (v < Min) Min = v;
	if (v > Max) Max = v;
	dY = Max - Min;
	N++;
}

void SSStat::Dump(FILE* fp) {
	fprintf(fp, "wavelength,%.4f,um\n", wavelength_um);
	fprintf(fp, "Micron per step,%.4f,um\n", MicronPerStep_um);
	fprintf(fp, "Phase shift,%.4f,um\n", PShift_um);
}

void SSStat::Step_rad(short inc, float MicronPerStep_um) {
	psd = PIE2 * MicronPerStep_um / wavelength_um;
	sdStep = sin(inc * psd);
}

void SStat::Serialize(CArchive& ar) {
	short magic;

	if (ar.IsStoring()) {
		magic = 0;
		ar << magic;
		ar << Stdev;
		ar << Ave;
		ar << Min;
		ar << Max;
		ar << dY;
	}
	else {
		magic = 0;
		ar >> magic;
		ar >> Stdev;
		ar >> Ave;
		ar >> Min;
		ar >> Max;
		ar >> dY;
	}
}
