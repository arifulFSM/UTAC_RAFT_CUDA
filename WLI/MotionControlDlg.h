#pragma once
#include "afxdialogex.h"

#include "PicWnd.h"
#include "CAM/CamPar.h"
#include "SRC/XTabCtrl.h"
#include "SRC/ResizableDialog.h"

class CMoSetup;
class CXYStageDlg;
class CFocusLevelDlg;
class CTiltControlDlg;

class CMotionControlDlg : public CResizableDialog {
	DECLARE_DYNAMIC(CMotionControlDlg)

		HWND hParent = 0;
	CXTabCtrl cTab;
public:
	CMoSetup* pMoSetup = nullptr;
	CTiltControlDlg* pTilt = nullptr;
	CXYStageDlg* pXY = nullptr;
	CFocusLevelDlg* pFL = nullptr;

	void StopLiveStreaming(CAM::ECAM eID);
	void RestartLiveStreaming(CAM::ECAM eID);
	bool GrabAnImage(IMGL::CIM& Im, CAM::ECAM eID, short subSampling);

public:
	bool bStream[2] = { false };
	CPicWnd cPicWnd[2];

public:
	void SetHParent(HWND hParent) {
		this->hParent = hParent;
	}
	CMotionControlDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMotionControlDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG10 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnStart();
	afx_msg void OnStopcam1();
	afx_msg void OnStartcam2();
	afx_msg void OnStopcam2();
	afx_msg void OnGrabcam1();
	afx_msg void OnGrabcam2();
	afx_msg void OnPropertiesCamera1();
	afx_msg void OnPropertiesCamera2();
	afx_msg void OnLed();
	afx_msg void OnLaser();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	float GetTiltZ();
	bool FringAdjustAT();
	void FringAdjustAF(float calibPos, float titlZ, float Range, float Step);
	void Reset();
	void LiftZMot();
	void UpdatePos();
	void CenteringFringe(int Range, double StepSize);
	void ATWithGeo();
};
