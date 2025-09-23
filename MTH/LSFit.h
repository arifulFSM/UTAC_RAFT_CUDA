#pragma once

#include <vector>

#include "Pxy.h"

// Linear least square fit [9/1/2021 FSM]
class CLSFit {
	// incremental linear least square fit [12/10/2020 FSM]
	double sumx = 0, sumx2 = 0, sumxy = 0, sumy = 0, sumy2 = 0;

	inline float Interpolate(float x0, float x, float x1, float y0, float y1) {
		return ((x - x0) / (x1 - x0)) * (y1 - y0) + y0;
	}

public:
	void Calc();

public:
	int N = 0;
	double R = 0; // correlation coeff [12/10/2020 FSM]
	double M = 0, C = 0; // y = Mx + C [12/10/2020 FSM]

	std::vector<SPxy> D;

public:
	CLSFit();
	virtual ~CLSFit();

public:
	double GetM();
	double GetX(float y);
	float Get(float x);
	SPxy* GetD(int n);
	SPxy* GetDLast();
	void Add(float x, float y);
	void Add(int i, float x, float y);
	void Dump(const char* str);
	void DumpPar(const char* str);
	void Get(double& m, double& c);
	void Get(double& m, double& c, double& r);
	void Get(float& m, float& c);
	void Reset();
	float Root();
	float ZeroCross(bool bPChg);
	bool Smooth(short wdw);
	bool Smooth(short wdw, short n);
	float Grad(int i1, int i2);
};
