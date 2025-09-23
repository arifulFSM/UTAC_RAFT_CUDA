#include "pch.h"

#include "Roi.h"

void SROI::Clear() {
	i1 = i2 = 0;
}

void SROI::Inflate(int n) { i1 += n; i2 -= n; }

void SROI::Set(int sz) {
	i1 = 0;  i2 = sz;
}

void SROI::Set(int lo, int hi) {
	i1 = lo; i2 = hi;
}

bool SROI::SetI(int idx, int wd, int sz) {
	this->idx = idx;
	i1 = idx - wd; if (i1 < 0) i1 = 0;
	i2 = idx + wd; if (i2 > sz) i2 = sz;
	return true;
}

SROI::SROI() {}

SROI::SROI(int sz) : i1(0), i2(sz) {}

SROI::SROI(int lo, int hi) : i1(lo), i2(hi) {}

SROI::SROI(int idx, int wd, int sz) {
	i1 = idx - wd; if (i1 < wd) i1 = wd;
	i2 = idx + wd; if (i2 > sz - wd) i2 = sz - wd;
}

bool SROI::SetA(int x1, int x2, int inc, int sz) {
	i1 = x1; i2 = x2;
	if (i1 < inc) i1 = inc;
	if (i2 > (sz - inc)) i2 = sz - inc;
	if ((i2 - i1) < 2 * inc) return false;
	return true;
}

void SROI::SetB(int idx, int wd) {
	i1 = idx - wd; i2 = idx + wd;
}

bool SROI::InRange(int idx) {
	if ((idx >= i1) && (idx < i2)) return true;
	return false;
}

bool SROI::InRange(int idx, int inc) {
	if (((idx - inc) >= i1) && ((idx + inc) < i2)) return true;
	return false;
}

bool SROI::EnsureValid(int inc, int sz, int rg/* = 4*/) {
	short inc2 = 2 * inc;
	if (i1 < inc2) i1 = inc2; if (i2 > (sz - inc2)) i2 = sz - inc2;
	if ((i2 - i1) < rg * inc) return false;
	return true;
}

int SROI::Size() { return i2 - i1; }

int SROI::Center() { return (i1 + i2) / 2; }

int SROI::Validate(int sz, int inc) {
	if (i1 < inc) i1 = inc;
	if (i2 > sz - inc) i2 = sz - inc;
	return i2 - i1;
}