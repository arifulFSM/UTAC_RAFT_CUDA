#pragma once

#include "Dev.h"
#include "PicWnd.h"
#include "LOG/LogSys.h"
#include "afxdialogex.h"
#include "Spot.h"
#include "AutoFocus.h"

#define M_PI 3.14159

struct RES {
	int vPeak, hPeak, vPeakDis, hPeakDis;
	RES() {
		vPeak = -1;
		vPeakDis = -1;
		hPeak = -1;
		hPeakDis = -1;
	}
};

struct SDLoc {
	int idx = 0;
	float dist = 0;
	SDLoc(float d, int n) {
		dist = d; idx = n;
	}
};

struct SLvl {
	CAM::CCamera* pCam;
	CAM::SCamProp prop;
	IMGL::CIM BG;

	float oldpos, score1, score2, bgscore;

	void Next() { score1 = score2; }
	float DScore() { return score2 - score1; }
};

class CMotionControlDlg;

class CMoSetupB {
protected:
	static bool bCheck;
	const float incZ = 2.5f;

protected:
	virtual void UpdatePositions() {}
	void MoveR(MOT::MAXIS nAx, float inc, DWORD tout, bool bCheck);

public:
	static bool bBlock;

public:
	CMoSetupB() {}
	virtual ~CMoSetupB() {}
};

class CMoSetup : public CDialogEx, CMoSetupB {
	DECLARE_DYNAMIC(CMoSetup)
		bool bFrnSrchStop = false;
	//float Xpos = 0, Ypos = 0, Rpos = 0, Apos = 0;
	float PrevPpos = 50.f;
	const float Mx_um = PZHOMEPOS * 2, Mn_um = 0;
	const float incPz_um = 0.01f;
	const float tltTUV = 0.25f;

	static bool bFineStage, bFineTilt;
	static bool bShowSearch;
	static float zN;
	CButton cUp, cDn, cLs;
	CStatic cMsg3;
	static MTH::SPoint3 P1, P2;
	MOT::CMotorCtrl* MO = nullptr; // pointer only
	CMotionControlDlg* pMoCtrl = nullptr; // pointer only, allow access to camera helpers

	static bool sorterDLoc(SDLoc const& lhs, SDLoc const& rhs) {
		return lhs.dist < rhs.dist;
	}
	static bool sorterCGdist(IMGL::SCgI const& lhs, IMGL::SCgI const& rhs) {
		return lhs.Dst < rhs.Dst;
	}
	static bool sorterCGcount(IMGL::SCgI const& lhs, IMGL::SCgI const& rhs) {
		return lhs.Count > rhs.Count;
	}

	void UpdatePositions();
	void ShowCG(IMGL::SCgI& Cg);
	void ShowScore(float score);
	void ShowScore(float score, float thres);
	void ShowScoreA(CString& msg, float score1, float score2);

	void TakeBG(SLvl& Lvl, IMGL::CIM& Im);
	void SubBG(SLvl& Lvl, IMGL::CIM& Im, IMGL::CIM& ID) {
		ID.SubAbs(Im, Lvl.BG);
	}
	void Snap1(SLvl& Lvl, IMGL::CIM& Im, IMGL::CIM& ID);
	void Snap2(SLvl& Lvl, IMGL::CIM& Im, IMGL::CIM& ID);
	void Leveling(float mv);
	bool Leveling(IMGL::CIM& Im, std::vector<IMGL::SCgI>& CG, IMGL::SNxt& nxt, CAM::ECAM eID);

	void ShowImage(IMGL::CIM& Im, CAM::ECAM eID);
	//void Identify(IMGL::CIM& Im, IMGL::CIM2& Im2, int mx, int lmt, std::vector<IMGL::SCgI>& CG);
	bool Replace(USHORT from, IMGL::CIM& Im, IMGL::CIM2& Im2, IMGL::SCgI& CG, COLORREF cr);
	void S1LevelSearch();
	void S2LevelSearch();
	void FringeSearch(bool bUp);

	void EnableUpDn(bool bEn);
	float GetTZPos(MTH::SPoint3& N);
	float CalcTZPos(float tDist, float uDist, float vDist);
	void TiltX(float tlt, DWORD tout, bool bCheck);
	void TiltY(float tlt, DWORD tout, bool bCheck);
	void TiltXY(float tltX, float tltY, DWORD tout, bool bCheck);
	void TiltUpDown(float inc, DWORD tout, bool bCheck);

	void Log(const std::wstring& msg, LOG::ELOGLEVEL lvl = LOG::ELOGLEVEL::NORMAL) {}

	bool GrabAnImage(IMGL::CIM& Im, CAM::ECAM eID, short subSampling);

public:
	CAutoFocus AF;
	CMoSetup(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMoSetup();
	void Yld(DWORD nTime);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG5 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	afx_msg void OnProperties();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedButton26();
	afx_msg void OnBnClickedButton27();
	afx_msg void OnBnClickedButton28();
	afx_msg void OnBnClickedButton46();
	afx_msg void OnBnClickedButton47();
	afx_msg void OnBnClickedButton54();
	afx_msg void OnBnClickedButton55();
	afx_msg void OnBnClickedButton56();
	afx_msg void OnBnClickedButton57();
	afx_msg void OnBnClickedButton58();
	afx_msg void OnBnClickedButton59();
	afx_msg void OnBnClickedButton60();
	afx_msg void OnBnClickedButton61();
	afx_msg void OnBnClickedButton66();
	afx_msg void OnBnClickedButton67();
	afx_msg void OnBnClickedButton69();
	afx_msg void OnBnClickedButton70();
	afx_msg void OnBnClickedButton72();
	afx_msg void OnBnClickedButton73();
	afx_msg void OnBnClickedButton74();
	afx_msg void OnBnClickedButton75();
	afx_msg void OnBnClickedButton76();
	afx_msg void OnBnClickedButton77();
	afx_msg void OnBnClickedButton79();
	afx_msg void OnBnClickedButton82();
	afx_msg void OnBnClickedButton83();
	afx_msg void OnBnClickedButton84();
	afx_msg void OnBnClickedButton87();
	afx_msg void OnBnClickedButton89();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton93();
	afx_msg void OnBnClickedButton94();
	afx_msg void OnBnClickedButton96();
	afx_msg void OnBnClickedButton97();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck10();
	afx_msg void OnBnClickedCheck11();
	afx_msg void OnBnClickedCheck15();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton92();
	afx_msg void OnBnClickedButton95();
	afx_msg void OnBnClickedButton98();
	afx_msg void OnBnClickedButton15();

	// Auto Tilt
	afx_msg void OnBnClickedAutoTilt1(); // 11212023
	bool AutoTilt1();
	afx_msg void FringeStripeAnalysis1();
	afx_msg bool FringeStripeAnalysis2();
	void computePhaseMap(const double* I0, const double* I1, const double* I2, const double* I3, int size, double* phaseMap);
	std::pair<int, int> countFringePeaks(const double* phaseMap, int size, double threshold);
	RES FringePeakCnt();
	bool IsFringeExist();
	bool VerticalFringeDirectionAdjustmentWithPS();
	bool HorizontalFringeDirectionAdjustmentWithPS();
	afx_msg void CollectIntensity(IMGL::CIM& ImR, std::vector<double>& IntensityValue, bool direction);
	afx_msg void OnBnClickedBtnShutter(); // // 12052023 - START
	bool HorizontalFringeDirectionAdjustment();
	bool VerticalFringeDirectionAdjustment();
	afx_msg void OnBnClickedAutoTilt2();
	void getFringeCenter(int totRange, double stepSize);
	bool PiezoMoveAcc(double targetPos);
	afx_msg void OnBnClickedAutoTilt3();
	CEdit m_StepSize;
	afx_msg void OnBnClickedAutoFocus();
	void AutoFocus(int range, double stepSize, BOOL isGeometry = TRUE);
	CEdit m_AFRange;
	bool isAtCancel = false;
	afx_msg void OnBnClickedBtnAtCncl();
	bool statusFlag1 = 0, statusFlag2 = 0;
	afx_msg void OnBnClickedCalibReset();
	bool isCamOpen = true;
	void getFringeDirection();
	enum FRNGDIR { LTR, RTL, TTD, DTT, INVD };
	FRNGDIR fringDirX = FRNGDIR::INVD, fringDirY = FRNGDIR::INVD;
	double pixDisX = -1, PiDisX = -1, pixDisY = -1, PiDisY = -1;
	void LiftZMot();
	void CalibReset();
	void ATWithGeo();
};
