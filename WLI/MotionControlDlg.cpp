#include "pch.h"
#include "WLI.h"
#include "PSI/ICC.h"
#include "MoSetup.h"
#include "XYStageDlg.h"
#include "FocusLevelDlg.h"
#include "TiltControlDlg.h"
#include "afxdialogex.h"
#include "wdefine.h"
#include "MotionControlDlg.h"

IMPLEMENT_DYNAMIC(CMotionControlDlg, CResizableDialog)

CMotionControlDlg::CMotionControlDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DIALOG10, pParent) {}

CMotionControlDlg::~CMotionControlDlg() {}

void CMotionControlDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, cTab);
	DDX_Control(pDX, IDC_PREVIEW, cPicWnd[0]);
	DDX_Control(pDX, IDC_PREVIEW2, cPicWnd[1]);
}

BEGIN_MESSAGE_MAP(CMotionControlDlg, CResizableDialog)
	ON_WM_DESTROY()
	//ON_COMMAND(ID_STARTCAM1, &CMotionControlDlg::OnStartcam1)
	ON_COMMAND(ID_STOPCAM1, &CMotionControlDlg::OnStopcam1)
	ON_COMMAND(ID_STARTCAM2, &CMotionControlDlg::OnStartcam2)
	ON_COMMAND(ID_STOPCAM2, &CMotionControlDlg::OnStopcam2)
	ON_COMMAND(ID_GRABCAM1, &CMotionControlDlg::OnGrabcam1)
	ON_COMMAND(ID_GRABCAM2, &CMotionControlDlg::OnGrabcam2)
	ON_COMMAND(ID_PROPERTIES_CAMERA1, &CMotionControlDlg::OnPropertiesCamera1)
	ON_COMMAND(ID_PROPERTIES_CAMERA2, &CMotionControlDlg::OnPropertiesCamera2)
	ON_COMMAND(ID_LED, &CMotionControlDlg::OnLed)
	ON_COMMAND(ID_LASER, &CMotionControlDlg::OnLaser)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CMotionControlDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMotionControlDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMotionControlDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON6, &CMotionControlDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON4, &CMotionControlDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON8, &CMotionControlDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON5, &CMotionControlDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CMotionControlDlg::OnBnClickedButton7)
END_MESSAGE_MAP()

BOOL CMotionControlDlg::OnInitDialog() {
	AddAnchor(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);

	CResizableDialog::OnInitDialog();
	short nTab = 0;
	pMoSetup = new CMoSetup;
	if (pMoSetup) {
		pMoSetup->pMoCtrl = this;
		pMoSetup->Create(IDD_DIALOG5, &cTab);
		cTab.AddTab(pMoSetup, CString("  Motor  ").GetBuffer(), nTab++);
	}

	pXY = new CXYStageDlg;
	if (pXY) {
		pXY->Create(IDD_DIALOG11, &cTab);
		cTab.AddTab(pXY, CString("  XY Stage  ").GetBuffer(), nTab++);
	}
	pTilt = new CTiltControlDlg;
	if (pTilt) {
		pTilt->Create(IDD_DIALOG9, &cTab);
		cTab.AddTab(pTilt, CString("  Tilt Stage  ").GetBuffer(), nTab++);
	}
	pFL = new CFocusLevelDlg;
	if (pFL) {
		pFL->Create(IDD_DIALOG12, &cTab);
		cTab.AddTab(pFL, CString(" Focus n Level ").GetBuffer(), nTab++);
	}

	ArrangeLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMotionControlDlg::OnDestroy() {
	CResizableDialog::OnDestroy();

	if (pFL) delete pFL;
	if (pXY) delete pXY;
	if (pTilt) delete pTilt;
	if (pMoSetup) delete pMoSetup;
}

void CMotionControlDlg::RestartLiveStreaming(CAM::ECAM eID) {
	if (eID == CAM::INVD) return;
	int ID = int(eID);
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	CAM::SCtx Ctx;
	Ctx.hWnd = cPicWnd[ID].GetSafeHwnd();
	cPicWnd[ID].GetClientRect(Ctx.rc);
	if (pCam->StartStream(Ctx, pCam->SCaM.ID)) {
		bStream[ID] = true;
	}
	else bStream[ID] = false;
}

void CMotionControlDlg::StopLiveStreaming(CAM::ECAM eID) {
	if (eID == CAM::INVD) return;
	int ID = int(eID);
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	Ctx.hWnd = cPicWnd[ID].GetSafeHwnd();
	cPicWnd[ID].GetClientRect(Ctx.rc);
	pCam->StopStream(Ctx, pCam->SCaM.ID);
	bStream[ID] = false;
}

bool CMotionControlDlg::GrabAnImage(IMGL::CIM& Im, CAM::ECAM eID, short subSampling) {
	CAM::SCamProp prop;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID);
	if (eID == CAM::SECCAM) {
		if (subSampling > 4) subSampling = 4;
	}
	int ID = int(eID);
	if (bStream[ID]) StopLiveStreaming(eID);
	if (!pCam->InitCapture(pCam->SCaM.ID, subSampling)) {
		if (bStream[ID]) RestartLiveStreaming(eID);
		return false;
	}
	if (!pCam->GetCamProp(prop)) {
		if (bStream[ID]) RestartLiveStreaming(eID);
		return false;
	}
	if (!pCam->StartFastGrab(prop, pCam->GetSubSample())) {
		if (bStream[ID]) RestartLiveStreaming(eID);
		return false;
	}
	if (!Im.IsNull()) Im.Destroy();
	if (!pCam->FastGrab(Im, prop)) {
		if (bStream[ID]) RestartLiveStreaming(eID);
		return false;
	}
	pCam->StopFastGrab();
	// restore image format
	pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
	if (bStream[ID]) RestartLiveStreaming(eID);
	return true;
}

//void CMotionControlDlg::OnStartcam1() {
//	if (!bStream[0]) { RestartLiveStreaming(CAM::PRICAM); bStream[0] = true; }
//}

void CMotionControlDlg::OnStopcam1() {
	if (bStream[0]) { StopLiveStreaming(CAM::PRICAM); bStream[0] = false; }
}

void CMotionControlDlg::OnStartcam2() {
	if (!bStream[1]) { RestartLiveStreaming(CAM::SECCAM); bStream[1] = true; }
}

void CMotionControlDlg::OnStopcam2() {
	if (bStream[1]) { StopLiveStreaming(CAM::SECCAM); bStream[1] = false; }
}

void CMotionControlDlg::OnGrabcam1() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, CAM::PRICAM, 8)) {
		Im.SavePicTime();
	}
}

void CMotionControlDlg::OnGrabcam2() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, CAM::SECCAM, 8)) {
		Im.SavePicTime();
	}
}

void CMotionControlDlg::OnPropertiesCamera1() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->LucamPropPage();
}

void CMotionControlDlg::OnPropertiesCamera2() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::SECCAM); if (!pCam) return;
	pCam->LucamPropPage();
}

void CMotionControlDlg::OnLed() {
	if (ICC.bLed) ICC.bLed = false;
	else ICC.bLed = true;
	MOT::CMotorCtrl* MO = Dev.MC.get();
	MO->stage.LED(ICC.bLed);
}

void CMotionControlDlg::OnLaser() {
	if (ICC.bLaser) ICC.bLaser = false;
	else ICC.bLaser = true;
	MOT::CMotorCtrl* MO = Dev.MC.get();
	MO->stage.Laser(ICC.bLaser);
}

void CMotionControlDlg::OnClose() {
	if (bStream[0]) { StopLiveStreaming(CAM::PRICAM); bStream[0] = false; }
	if (bStream[1]) { StopLiveStreaming(CAM::SECCAM); bStream[1] = false; }
	if (hParent) {
		::PostMessage(hParent, UM_CLOSE, 0, 0);
		hParent = 0;
	}
	CResizableDialog::OnClose();
}

void CMotionControlDlg::OnBnClickedButton1() {
	if (!bStream[0]) { RestartLiveStreaming(CAM::PRICAM); bStream[0] = true; }
}

void CMotionControlDlg::OnBnClickedButton2() {
	if (bStream[0]) { StopLiveStreaming(CAM::PRICAM); bStream[0] = false; }
}

void CMotionControlDlg::OnBnClickedButton3() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, CAM::PRICAM, 8)) {
		Im.SavePicTime();
	}
}

void CMotionControlDlg::OnBnClickedButton6() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->LucamPropPage();
}

void CMotionControlDlg::OnBnClickedButton4() {
	if (!bStream[1]) { RestartLiveStreaming(CAM::SECCAM); bStream[1] = true; }
}

void CMotionControlDlg::OnBnClickedButton8() {
	if (bStream[1]) { StopLiveStreaming(CAM::SECCAM); bStream[1] = false; }
}

void CMotionControlDlg::OnBnClickedButton5() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, CAM::SECCAM, 4)) {
		Im.SavePicTime();
	}
}

void CMotionControlDlg::OnBnClickedButton7() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::SECCAM); if (!pCam) return;
	pCam->LucamPropPage();
}

void CMotionControlDlg::Reset() {
	if (pMoSetup) {
		pMoSetup->CalibReset();
	}
}

float CMotionControlDlg::GetTiltZ() {
	MTH::SPoint3 N;
	if (pMoSetup) {
		return pMoSetup->GetTZPos(N);
	}
}

void CMotionControlDlg::FringAdjustAF(float calibPos, float tiltZ, float Range, float Step) {
	if (pMoSetup) {
		pMoSetup->AF.calibZPos = calibPos;
		pMoSetup->AF.tiltZ = tiltZ;
		pMoSetup->AutoFocus(Range, Step);
	}
}

bool CMotionControlDlg::FringAdjustAT() {
	if (pMoSetup) {
		return pMoSetup->FringeStripeAnalysis2();
	}
}

void CMotionControlDlg::LiftZMot() {
	if (pMoSetup) {
		pMoSetup->LiftZMot();
	}
}

void CMotionControlDlg::UpdatePos() {
	if (pMoSetup) {
		pMoSetup->UpdatePositions();
	}
}

void CMotionControlDlg::CenteringFringe(int Range, double StepSize) {
	if (pMoSetup) {
		pMoSetup->getFringeCenter(Range, StepSize);
	}
}

void CMotionControlDlg::ATWithGeo() {
	if (pMoSetup) {
		pMoSetup->ATWithGeo();
	}
}