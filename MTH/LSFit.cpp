#include "pch.h"

#include "Define.h"
#include "LSFit.h"

void CLSFit::Calc() {
	double denom = N * sumx2 - sumx * sumx;
	if (denom == 0) {
		// singular matrix. can't solve the problem.
		M = 0; C = 0; R = 0;
		return;
	}

	M = (N * sumxy - sumx * sumy) / denom;
	C = (sumy * sumx2 - sumx * sumxy) / denom;
	R = (sumxy - sumx * sumy / N) /    /* compute correlation coeff */
		sqrt((sumx2 - (sumx * sumx) / N) *
			(sumy2 - (sumy * sumy) / N));
}

CLSFit::CLSFit() {}

CLSFit::~CLSFit() {}

void CLSFit::Add(float x, float y) {
	D.push_back(SPxy(x, y));
	sumx += x;
	sumx2 += (x * x);
	sumxy += (x * y);
	sumy += y;
	sumy2 += (y * y);
	N++;
}

void CLSFit::Add(int i, float x, float y) {
	D.push_back(SPxy(i, x, y));
	sumx += x;
	sumx2 += (x * x);
	sumxy += (x * y);
	sumy += y;
	sumy2 += (y * y);
	N++;
}

void CLSFit::Dump(const char* str) {
	FILE* fp = fopen(str, "wb"); if (!fp) return;
	int sz = int(D.size());
	for (int i = 0; i < sz; i++) {
		fprintf(fp, "%f,%f\n", D[i].x, D[i].y);
	}
	fclose(fp);
}

void CLSFit::DumpPar(const char* str) {
	Calc();
	FILE* fp = fopen(str, "wb"); if (!fp) return;
	fprintf(fp, "M = ,%.6f,C = ,%.6f,R = ,%.6f\n\n", M, C, R);
	for (int i = 0; i < int(D.size()); i++) {
		fprintf(fp, "%f,%f\n", D[i].x, D[i].y);
	}
	fclose(fp);
}

void CLSFit::Reset() {
	D.clear();
	N = 0;
	sumx = 0; sumx2 = 0; sumxy = 0; sumy = 0; sumy2 = 0;
}

bool CLSFit::Smooth(short wdw) {
	int sz = int(D.size()); if (sz < (2 * wdw + 1)) return false;
	float* p, * q, * buf = new float[sz];
	q = buf;
	SPxy* Pxy = &D[0];
	// copy [10/3/2021 yuenl]
	for (int i = 0; i < sz; i++, q++, Pxy++) {
		*q = Pxy->y;
	}
	// smooth [10/3/2021 yuenl]
	q = buf;
	Pxy = &D[0];
	for (int i = 0; i < sz; i++, q++, Pxy++) {
		int st = i - wdw, ed = i + wdw;
		if (st < 0) st = 0; if (ed >= sz) ed = sz - 1;
		p = buf + st;
		double sum = 0;
		for (int j = st; j <= ed; j++, p++) {
			sum += *p;
		}
		Pxy->y = float(sum / (ed - st + 1));
	}
	if (buf) delete[]buf;
	return true;
}

bool CLSFit::Smooth(short wdw, short n) {
	if (wdw > 7) wdw = 7;
	for (int j = 0; j < n; j++) {
		if (!Smooth(wdw)) return false;
	}
	return true;
}

float CLSFit::Grad(int i1, int i2) {
	return D[i2].y - D[i1].y;
}

float CLSFit::Root() {
	CLSFit Lsq;
	int sz = int(D.size()) - 1; if (sz < 0) return 0;
	for (int i = 0; i < sz - 1; i++) {
		int ii = i + 1;
		float v = (D[ii].y - D[i].y) / (D[ii].x - D[i].x);
		Lsq.Add(D[ii].x, v);
	}
	Lsq.DumpPar("C:\\TEMP\\F2.CSV");
	return float(Lsq.GetX(0));
}

float CLSFit::ZeroCross(bool bPChg) {
	// return interpolated x Pz position [10/9/2021 yuenl]
	int sz = int(D.size()) - 1; if (sz < 2) return BADDATA;
	sz--;
	int idx = -1;
	CLSFit Ls;
	std::vector<SPxy> F; F.resize(sz);
	SPxy* pF = &F[0];
	SPxy* A = &D[0], * B = &D[1];
	for (int i = 0; i < sz; i++, pF++, A++, B++) {
		*pF = *B; pF->y = (B->y - A->y) / (B->x - A->x);
	}
	pF = &F[0];
	if (!bPChg) {
		for (int j = 0; j < sz; j++, pF++) {
			if (pF->y <= 0) continue;
			for (int i = j; i < sz; i++, pF++) {
				if (pF->y > 0) continue;
				idx = i; break;
			}
			if (idx > 0) {
				A = &F[idx - 1];
				Ls.Add(A->x, A->y); A++; Ls.Add(A->x, A->y);
				return float(Ls.GetX(0));
			}
			break;
		}
	}
	else {
		for (int j = 0; j < sz; j++, pF++) {
			if (pF->y >= 0) continue;
			for (int i = j; i < sz; i++, pF++) {
				if (pF->y < 0) continue;
				idx = i; break;
			}
			if (idx > 0) {
				A = &F[idx - 1];
				Ls.Add(A->x, A->y); A++; Ls.Add(A->x, A->y);
				return float(Ls.GetX(0));
			}
			break;
		}
	}
	// Find zero crossing [10/9/2021 yuenl]
	return BADDATA;
}

SPxy* CLSFit::GetD(int n) {
	if ((n >= 0) && n < D.size()) return &D[n];
	return nullptr;
}

SPxy* CLSFit::GetDLast() {
	int n = int(D.size() - 1);
	if (n >= 0) return &D[n];
	return nullptr;
}

double CLSFit::GetX(float y) {
	// get x @ y=0 [7/3/2021 FSM]
	Calc();
	if (M) return (y - C) / M; return 0;
}

float CLSFit::Get(float x) {
	return float(M * x + C);
}

double CLSFit::GetM() {
	Calc();
	return M;
}

void CLSFit::Get(float& m, float& c) {
	Calc();
	m = float(M); c = float(C);
}

void CLSFit::Get(double& m, double& c) {
	Calc();
	m = M; c = C;
}

void CLSFit::Get(double& m, double& c, double& r) {
	Calc();
	m = M; c = C; r = R;
}