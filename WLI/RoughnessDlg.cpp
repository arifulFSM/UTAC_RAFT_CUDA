// RoughnessDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "RoughnessDlg.h"
#include <math.h>
#include "fft_lib.h"

// RoughnessDlg dialog

IMPLEMENT_DYNAMIC(RoughnessDlg, CResizableDialog)

RoughnessDlg::RoughnessDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DLG_ROUGHNESS, pParent) {}

RoughnessDlg::~RoughnessDlg() {}

void RoughnessDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NFFT, m_NFFT);
	DDX_Control(pDX, IDC_FFT_CUTOFF, m_FFTCutoff);
	DDX_Control(pDX, IDC_ORA, m_ORa);
	DDX_Control(pDX, IDC_BFRA, m_BfRa);
	DDX_Control(pDX, IDC_FFTRA, m_FFTRa);
	DDX_Control(pDX, IDC_ORRMS, m_ORrms);
	DDX_Control(pDX, IDC_BFRRMS, m_BfRrms);
	DDX_Control(pDX, IDC_FFTRRMS, m_FFTRrms);
	DDX_Control(pDX, IDC_ORMAX, m_ORmax);
	DDX_Control(pDX, IDC_BFRMAX, m_BfRmax);
	DDX_Control(pDX, IDC_FFTRMAX, m_FFTRmax);
}

BEGIN_MESSAGE_MAP(RoughnessDlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BTN_RECALC_ROUGHNESS, &RoughnessDlg::OnBnClickedBtnRecalcRoughness)
END_MESSAGE_MAP()

// RoughnessDlg message handlers

BOOL RoughnessDlg::OnInitDialog() {
	__super::OnInitDialog();

	m_nFFT = 16384;
	m_nFFTcutoff = 3;

	m_NFFT.SetWindowTextW(CString(std::to_string(m_nFFT).c_str()));
	m_FFTCutoff.SetWindowTextW(CString(std::to_string(m_nFFTcutoff).c_str()));

	//CalcRoughness();

	return TRUE;
}

void RoughnessDlg::CalcRoughness() {
	STATS Stats;

	CopyData();

	// Original Roughness Calc
	CalculateRoughnessStats(&Stats);
	m_fRrms1 = Stats.fStDev;
	m_fRa1 = Stats.fRa;
	m_fRmax1 = Stats.fMax - Stats.fMin;

	// Best Fit Roughness Calc
	SubtractBestFitPlane();
	CalculateRoughnessStats(&Stats);
	m_fRrms2 = Stats.fStDev;
	m_fRa2 = Stats.fRa;
	m_fRmax2 = Stats.fMax - Stats.fMin;

	// FFT
	CopyData();
	ApplyFFT();
	CalculateRoughnessStats(&Stats);
	m_fRrms3 = Stats.fStDev;
	m_fRa3 = Stats.fRa;
	m_fRmax3 = Stats.fMax - Stats.fMin;

	ShowData();
}

void RoughnessDlg::CopyData() {
	tmpHeightDataOvrCord.clear();
	if (ICC.isRegionType == ICC.LINE) {
		for (auto X : ICC.HeightDataOvrCord) tmpHeightDataOvrCord.push_back(X);
	}
	else {
		for (int i = 0; i < ICC.LineData.size(); i++) {
			for (int j = 0; j < ICC.LineData[0].size(); j++) {
				tmpHeightDataOvrCord.push_back({ {i, j}, ICC.LineData[i][j] });
			}
		}
	}
}

void RoughnessDlg::SubtractBestFitPlane() {
	int i = 0, j;
	double S1, Sx, Sy, Sz, Sxx, Sxy, Syy, Sxz, Syz;
	double det, det1, det2, det3, alpha, beta, gamma;
	/// subtract a best-fit plane
	S1 = Sx = Sy = Sz = Sxx = Sxy = Syy = Sxz = Syz = 0;
	int row = ICC.y2 - ICC.y1 + 1;
	int col = ICC.x2 - ICC.x1 + 1;
	for (int i = 0; i < row; i++) {
		for (j = 0; j < col; j++) {
			S1 += 1.0;
			Sx += j;
			Sy += i;
			Sz += tmpHeightDataOvrCord[i * col + j].second;
			Sxx += 1.0 * j * j;
			Syy += 1.0 * i * i;
			Sxy += 1.0 * j * i;
			Sxz += tmpHeightDataOvrCord[i * col + j].second * j;
			Syz += tmpHeightDataOvrCord[i * col + j].second * i;
		}
	}
	det = Sxx * (Syy * S1 - Sy * Sy) - Sxy * (Sxy * S1 - Sx * Sy) + Sx * (Sxy * Sy - Sx * Syy);
	det1 = Sxz * (Syy * S1 - Sy * Sy) - Sxy * (Syz * S1 - Sz * Sy) + Sx * (Syz * Sy - Sz * Syy);
	det2 = Sxx * (Syz * S1 - Sz * Sy) - Sxz * (Sxy * S1 - Sx * Sy) + Sx * (Sxy * Sz - Sx * Syz);
	det3 = Sxx * (Syy * Sz - Sy * Syz) - Sxy * (Sxy * Sz - Sx * Syz) + Sxz * (Sxy * Sy - Sx * Syy);

	if (det == 0) {
		alpha = beta = gamma = 0;
	}
	else {
		alpha = det1 / det;
		beta = det2 / det;
		gamma = det3 / det;
		for (i = 0; i < row; i++) {
			for (j = 0; j < col; j++)
				tmpHeightDataOvrCord[i * col + j].second -= (alpha * j + beta * i + gamma);
		}
	}
}

void RoughnessDlg::ApplyFFT() {
	fft_lib fft;
	double* pfSignal = new double[m_nFFT + 5];
	memset(pfSignal, 0, m_nFFT * sizeof(double));
	int i, j;
	int row = ICC.y2 - ICC.y1 + 1;
	int col = ICC.x2 - ICC.x1 + 1;
	for (i = 0; i < row; i++) {
		for (j = 0; j < col; j++)
			pfSignal[j] = tmpHeightDataOvrCord[i * col + j].second;
		SpreadArray(pfSignal, col, m_nFFT);

		fft.realft2(pfSignal, m_nFFT, 1);
		for (j = 0; j < m_nFFT; j++) {
			if (j <= m_nFFTcutoff)
				pfSignal[j] = 0;
		}
		fft.realft2(pfSignal, m_nFFT, -1);
		SpreadArray(pfSignal, m_nFFT, col);
		for (j = 0; j < col; j++)
			tmpHeightDataOvrCord[i * col + j].second = pfSignal[j];
	}
	for (i = 0; i < col; i++) {
		for (j = 0; j < row; j++)
			pfSignal[j] = tmpHeightDataOvrCord[j * col + i].second;
		SpreadArray(pfSignal, row, m_nFFT);
		fft.realft2(pfSignal, m_nFFT, 1);
		for (j = 0; j < m_nFFT; j++) {
			if (j <= m_nFFTcutoff)
				pfSignal[j] = 0;
		}
		fft.realft2(pfSignal, m_nFFT, -1);
		SpreadArray(pfSignal, m_nFFT, row);
		for (j = 0; j < row; j++)
			tmpHeightDataOvrCord[j * col + i].second = pfSignal[j];
	}
	if (pfSignal)
		delete[] pfSignal;
}

void RoughnessDlg::SpreadArray(double* p, int N1, int N2) {
	int i, j;
	double fXMin = 0;
	double fXMax = N1;
	double fXTemp = fXMin;
	double fXStep = (fXMax - fXMin) / (N2 - 1);

	double* p2 = new double[N2];

	p2[0] = p[0];
	p2[N2 - 1] = p[N1 - 1];

	j = 0;
	for (i = 1; i < N2 - 1; i++) {
		fXTemp += fXStep;
		while (!(fXTemp >= j && fXTemp <= j + 1)) {
			j++;
			if (j >= N1 - 1)
				break;
		}
		p2[i] = p[j] + (fXTemp - j) * (p[j + 1] - p[j]);
	}
	for (i = 0; i < N2; i++)
		p[i] = p2[i];
	if (p2)
		delete[] p2;
}

void RoughnessDlg::CalculateRoughnessStats(STATS* pStats) {
	int i, N = tmpHeightDataOvrCord.size();
	pStats->fMin = 1e20;
	pStats->fMax = -1e20;
	pStats->fAver = 0;
	pStats->fRa = 0;
	pStats->fStDev = 0;
	for (i = 0; i < N; i++) {
		pStats->fAver += tmpHeightDataOvrCord[i].second;
		if (tmpHeightDataOvrCord[i].second < pStats->fMin)
			pStats->fMin = tmpHeightDataOvrCord[i].second;
		if (tmpHeightDataOvrCord[i].second > pStats->fMax)
			pStats->fMax = tmpHeightDataOvrCord[i].second;
	}
	pStats->fAver /= N;
	for (i = 0; i < N; i++) {
		pStats->fStDev += (tmpHeightDataOvrCord[i].second - pStats->fAver) * (tmpHeightDataOvrCord[i].second - pStats->fAver);
		pStats->fRa += fabs(tmpHeightDataOvrCord[i].second - pStats->fAver);
	}
	pStats->fStDev = sqrt(pStats->fStDev / N);
	pStats->fRa /= N;
}

void RoughnessDlg::OnBnClickedBtnRecalcRoughness() {
	// TODO: Add your control notification handler code here
	CalcRoughness();
}

void RoughnessDlg::ShowData() {
	m_ORa.SetWindowTextW(CString(std::to_string(m_fRa1).c_str()));
	m_ORrms.SetWindowTextW(CString(std::to_string(m_fRrms1).c_str()));
	m_ORmax.SetWindowTextW(CString(std::to_string(m_fRmax1).c_str()));
	m_BfRa.SetWindowTextW(CString(std::to_string(m_fRa2).c_str()));
	m_BfRrms.SetWindowTextW(CString(std::to_string(m_fRrms2).c_str()));
	m_BfRmax.SetWindowTextW(CString(std::to_string(m_fRmax2).c_str()));
	m_FFTRa.SetWindowTextW(CString(std::to_string(m_fRa3).c_str()));
	m_FFTRrms.SetWindowTextW(CString(std::to_string(m_fRrms3).c_str()));
	m_FFTRmax.SetWindowTextW(CString(std::to_string(m_fRmax3).c_str()));
}