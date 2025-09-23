#pragma once

#include "afxdialogex.h"
#include "SRC/ResizableDialog.h"
#include "PSI/ICC.h"

// RoughnessDlg dialog
typedef struct _Stats {
	double fAver;
	double fStDev;
	double fMin;
	double fMax;
	double fRa;
}STATS;

class RoughnessDlg : public CResizableDialog {
	DECLARE_DYNAMIC(RoughnessDlg)

public:
	RoughnessDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~RoughnessDlg();
	BOOL OnInitDialog();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_ROUGHNESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nFFT;
	int m_nFFTcutoff;
	float m_fRrms1;
	float m_fRa1;
	float m_fRmax1;
	float m_fRrms2;
	float m_fRa2;
	float m_fRmax2;
	float m_fRrms3;
	float m_fRa3;
	float m_fRmax3;
	CEdit m_NFFT;
	CEdit m_FFTCutoff;
	CEdit m_ORa;
	CEdit m_BfRa;
	CEdit m_FFTRa;
	CEdit m_ORrms;
	CEdit m_BfRrms;
	CEdit m_FFTRrms;
	CEdit m_ORmax;
	CEdit m_BfRmax;
	CEdit m_FFTRmax;
	afx_msg void OnBnClickedBtnRecalcRoughness();
	void CalculateRoughnessStats(STATS* pStats);
	void CalcRoughness();
	void ShowData();
	std::vector<std::pair<std::pair<int, int>, double>>tmpHeightDataOvrCord;
	void SubtractBestFitPlane();
	void SpreadArray(double* p, int N1, int N2);
	void ApplyFFT();
	void CopyData();
};
