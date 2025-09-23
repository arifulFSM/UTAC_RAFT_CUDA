#pragma once

#include <fstream>

#include "In.h"

struct SROI {
public:
	// Region of interest [4/7/2021 FSM]
	int idx = 0; // index of reference position
	int i1 = 0, i2 = 0; // index of x1, x2 [4/7/2021 FSM]
	//float x1_um = 0, x2_um = 0; // 0 indicate not active or use [4/7/2021 FSM]

public:
	SROI();
	SROI(int sz);
	SROI(int lo, int hi);
	SROI(int idx, int wd, int sz);
	int Center();
	int Size();
	int Validate(int sz, int inc);
	void Clear();
	void Inflate(int n);
	void Set(int lo, int hi);
	void Set(int sz);
	bool SetA(int x1, int x2, int inc, int sz);
	bool SetI(int idx, int wd, int sz);
	void SetB(int idx, int wd);
	bool InRange(int idx);
	bool InRange(int idx, int inc);
	bool EnsureValid(int inc, int sz, int rg = 4);
};
