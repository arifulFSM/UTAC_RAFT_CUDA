#include "pch.h"
#include "LSF3D.h"

CLSF3D::CLSF3D() {
	Init();
}

CLSF3D::~CLSF3D() {}

void CLSF3D::Init() {
	m_dfSxx = m_dfSyy = m_dfSxy = 0;
	m_dfSx = m_dfSy = 0;
	m_dfSo = 0;

	m_dfSxz = m_dfSyz = 0;
	m_dfSz = 0;
}

void CLSF3D::Input(double dfX, double dfY, double dfZ) {
	m_dfSxx += dfX * dfX;
	m_dfSxy += dfX * dfY;
	m_dfSyy += dfY * dfY;
	m_dfSx += dfX;
	m_dfSy += dfY;
	m_dfSo += 1;

	m_dfSxz += dfX * dfZ;
	m_dfSyz += dfY * dfZ;
	m_dfSz += dfZ;
}

// Superman : very straight-forward method of Gauss Jordan Elimination
// Later we need to use the alternative method, LU
BOOL CLSF3D::GetResult(double& dfA, double& dfB, double& dfC) {
	// convention: [row][col]
	double Matrix[3][3] = { { m_dfSxx, m_dfSxy, m_dfSx}, {m_dfSxy, m_dfSyy, m_dfSy}, {m_dfSx, m_dfSy, m_dfSo} };
	double Solution[3] = { m_dfSxz, m_dfSyz, m_dfSz };

	for (int i = 0; i < 3; i++) {
		double dfDivider = Matrix[i][i];
		if (dfDivider == 0.) return FALSE;	// cannot proceed without pivoting

		Matrix[i][i] = 1;
		for (int c = i + 1; c < 3; c++) {
			Matrix[i][c] /= dfDivider;
		}
		Solution[i] /= dfDivider;
		for (int r = 0; r < 3; r++) {
			if (r == i) continue;
			double dfMultiplier = Matrix[r][i];
			if (dfMultiplier != 0) {
				Matrix[r][i] = 0;
				for (int d = i + 1; d < 3; d++) {
					Matrix[r][d] -= Matrix[i][d] * dfMultiplier;
				}
				Solution[r] -= Solution[i] * dfMultiplier;
			}
		}
	}
	dfA = Solution[0];
	dfB = Solution[1];
	dfC = Solution[2];
	return TRUE;
}