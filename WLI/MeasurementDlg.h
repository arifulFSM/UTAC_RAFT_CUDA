#pragma once
#include "afxdialogex.h"
#include "SRC/ResizableDialog.h"
#include "RecipeRAFT.h"
#include "ResultRAFT.h"
#include <vector>
#include <fstream>
#include <fft_lib.h>
#include <MeasProgressDlg.h>
#include "WaferMap.h"
#include "wdefine.h"
#include "Cfilters.h"
#include <opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>

// MeasurementDlg dialog
using namespace cv;

typedef struct _RStats {
	double fAver;
	double fStDev;
	double fMin;
	double fMax;
	double fRa;
}RSTATS;

//20250916
struct Histogram {
	const int nbins = 256;
	std::vector<int> His;
	float mx = 0, mn = 0, sf = 0;
};

class MeasurementDlg : public CResizableDialog {
	DECLARE_DYNAMIC(MeasurementDlg)
public:
	BOOL bMeasured;
	void StartCalculation(int pt, float x, float y);

public:
	CRecipeRAFT* pRcp;
	CResultRAFT* pResult;

	MeasurementDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~MeasurementDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEASUREMENT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_cResults;
	afx_msg void OnBnClickedMeLoadrcp();
	afx_msg void OnBnClickedMeasure();
	void Make24HStretchCV(cv::Mat& ImCV);//20250916
	void LevelCV(cv::Mat& ImCV);//20250916
	bool Histo256(const cv::Mat& image);//20250916
	void DataAcquisitionSimu();
	void DataAcquisitionSimuCV();//20250916
	void DataAcquisition();
	std::vector<float>HeightData;
	std::vector<float>HeightDataCV;//20250916
	void getHeightDataCV(int idx = -1);//20250916
	void getHeightData(int idx = -1);
	void CalculateRoughnessStats(RSTATS* pStats);
	void SpreadArray(double* p, int N1, int N2);
	void ApplyFFT();
	void calcRoughness();
	int m_nFFT = 16384;
	int m_nFFTcutoff = 3;
	float m_fRrms1 = 0.0;
	float m_fRa1 = 0.0;
	float m_fRmax1 = 0.0;
	int wd, ht;
	CWaferMap m_cWaferMap;
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	void RecipeToLocal();
	MeasProgressDlg* progress = nullptr;
	CStatic cLiveVid;
	void camRun();
	afx_msg void OnBnClickedMotSetupMd();
	afx_msg void OnBnClickedCamPropMd();
	CStatic m_ProgressMsg;
	void ShowMessage(CString str);
	HWND hWndParent = 0;
	Cfilters filter;
	CProgressCtrl m_MeasurementProgress;
	CStatic m_ProgressCount;
public:
	Histogram His5;// 20250916
};
