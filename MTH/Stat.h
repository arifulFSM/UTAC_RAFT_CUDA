#pragma once

#include "MTH/Ang.h"

struct SSStat {
	//? Obsoleted, to be removed
	float MicronPerStep_um = 0;
	float psd = 0, sdStep = 0;
	float PShift_um = 0; // phase shift in um, quarter wavelength, pie / 2
	float wavelength_um = 0; // wavelength
	int inc = 0;

public:
	void Dump(FILE* fp);
	void Step_rad(short inc, float MicronPerStep_um);
};

struct SStat {
public:
	SStat();
	virtual ~SStat();

	void Dump(FILE* fp);

public:
	//////////////////////////////////////////////////////////////////////////
	unsigned long long N = 0;
	double sum = 0, sum2 = 0;
	//////////////////////////////////////////////////////////////////////////
	float Stdev = 0;
	float Ave = 0;
	float Min = 0;
	float Max = 0;
	float dY = 0; // Max - Min [8/6/2021 FSM]
	//////////////////////////////////////////////////////////////////////////
public:
	void Add(float v);
	void Clear();
	void Calculate();
	void CalcStat(float* pBuf, int N);
public:
	float MidU() { return float(Max - Ave) * 0.5f; }
	float MidD() { return float(Ave - Min) * 0.5f; }
	void Serialize(CArchive& ar);
};
