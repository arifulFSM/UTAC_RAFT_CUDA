#include "pch.h"
#include <thread>

#include "WLI.h"
#include "WLI/Glbs.h"
#include "SRC/DOSUtil.h"
#include "PSI/ICC.h"
#include "MoSetup.h"
#include "MotionControlDlg.h"
#include <algorithm>
#include <chrono>
#include "afxdialogex.h"
#include "MotPropDlg.h"
#include "AutoTilt.h"
#include "AutoFocus.h"
#include "fstream"

using namespace MOT;

IMPLEMENT_DYNAMIC(CMoSetup, CDialogEx)

float CMoSetup::zN = 100.f;
bool CMoSetupB::bCheck = true;
bool CMoSetupB::bBlock = false;
bool CMoSetup::bShowSearch = true;
bool CMoSetup::bFineStage = false;
bool CMoSetup::bFineTilt = false;
MTH::SPoint3 CMoSetup::P1, CMoSetup::P2;

float CMoSetup::GetTZPos(MTH::SPoint3& N) {
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	MTH::SGeo& G = MO->tilt.Geo;
	MTH::SPoint3 Pt = G.T, Pu = G.U, Pv = G.V, Pxy;
	Pt.z = M[int(MAXIS::T)].now;
	Pu.z = M[int(MAXIS::U)].now;
	Pv.z = M[int(MAXIS::V)].now;
	Pxy.x = M[int(MAXIS::X)].now;
	Pxy.y = M[int(MAXIS::Y)].now;
	G.Pln(Pt, Pu, Pv);
	return G.Pz(Pxy);
}

float CMoSetup::CalcTZPos(float tDist, float uDist, float vDist) {
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	MTH::SGeo& G = MO->tilt.Geo;
	MTH::SPoint3 Pt = G.T, Pu = G.U, Pv = G.V, Pxy;
	Pt.z = M[int(MAXIS::T)].now + tDist;
	Pu.z = M[int(MAXIS::U)].now + uDist;
	Pv.z = M[int(MAXIS::V)].now + vDist;
	Pxy.x = M[int(MAXIS::X)].now;
	Pxy.y = M[int(MAXIS::Y)].now;
	G.Pln(Pt, Pu, Pv);
	return G.Pz(Pxy);
}

void CMoSetup::UpdatePositions() {
	CString str;
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	//str.Format(_T("X:%.3f mm"), M[int(MAXIS::X)].now);
	//GetDlgItem(IDC_EDIT1)->SetWindowTextW(str);
	//str.Format(_T("Y:%.3f mm"), M[int(MAXIS::Y)].now);
	//GetDlgItem(IDC_EDIT9)->SetWindowTextW(str);
	str.Format(_T("Z:%.3f mm"), M[int(MAXIS::Z)].now);
	GetDlgItem(IDC_EDIT7)->SetWindowTextW(str);
	str.Format(_T("%.3f, %.3f, %.1f au"), P2.x - P1.x, P2.y - P1.y, P2.z - P1.z);
	GetDlgItem(IDC_EDIT11)->SetWindowTextW(str);

	str.Format(_T("P: %.3f um"), MO->Piezo.GetPos_um());
	GetDlgItem(IDC_EDIT18)->SetWindowTextW(str);
}

void CMoSetup::TiltX(float tlt, DWORD tout, bool bCheck) {
	if (bBlock) return; bBlock = true;
	CWaitCursor wc;
	CTilt& tilt = MO->tilt;
	SMotPar* M = &MO->stage.Mpar[0];
	P1.Zero(); P2.z = zN; P2.x += tlt;
	MTH::SPoint3 N = MTH::SPoint3::Normalize(P2 - P1);
	tilt.SetN(N);
	MTH::SPoint3 P3;
	P3.x = M[int(MOT::MAXIS::X)].now;
	P3.y = M[int(MOT::MAXIS::Y)].now;
	P3.z = GetTZPos(N);
	tilt.DoTilts(P3, N, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::TiltY(float tlt, DWORD tout, bool bCheck) {
	if (bBlock) return; bBlock = true;
	CWaitCursor wc;
	CTilt& tilt = MO->tilt;
	SMotPar* M = &MO->stage.Mpar[0];
	P1.Zero(); P2.z = zN; P2.y += tlt;
	MTH::SPoint3 N = MTH::SPoint3::Normalize(P2 - P1);
	tilt.SetN(N);
	MTH::SPoint3 P3;
	P3.x = M[int(MOT::MAXIS::X)].now;
	P3.y = M[int(MOT::MAXIS::Y)].now;
	P3.z = GetTZPos(N);
	tilt.DoTilts(P3, N, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::TiltXY(float tltX, float tltY, DWORD tout, bool bCheck) {
	if (bBlock) return; bBlock = true;
	CWaitCursor wc;
	CTilt& tilt = MO->tilt;
	SMotPar* M = &MO->stage.Mpar[0];
	P1.Zero(); P2.z = zN; P2.x += tltX; P2.y += tltY;
	MTH::SPoint3 N = MTH::SPoint3::Normalize(P2 - P1);
	tilt.SetN(N);
	MTH::SPoint3 P3;
	P3.x = M[int(MOT::MAXIS::X)].now;
	P3.y = M[int(MOT::MAXIS::Y)].now;
	P3.z = GetTZPos(N);
	tilt.DoTilts(P3, N, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::TiltUpDown(float inc, DWORD tout, bool bCheck) {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->tilt.UpDownTUV(inc, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CMoSetupB::MoveR(MAXIS nAx, float inc, DWORD tout, bool bCheck) {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	Dev.MC->MoveR(nAx, inc, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

CMoSetup::CMoSetup(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG5, pParent) {}

CMoSetup::~CMoSetup() {}

void CMoSetup::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_EDIT19, Xpos);
	//DDX_Text(pDX, IDC_EDIT22, Ypos);
	//DDX_Text(pDX, IDC_EDIT23, Rpos);
	//DDX_Text(pDX, IDC_EDIT24, Apos);
	DDX_Control(pDX, IDC_BUTTON82, cUp);
	DDX_Control(pDX, IDC_BUTTON83, cDn);
	DDX_Control(pDX, IDC_BUTTON96, cLs);
	DDX_Control(pDX, IDC_MSG3, cMsg3);
	DDX_Control(pDX, IDC_Step_Size, m_StepSize);
	DDX_Control(pDX, IDC_AF_RANGE, m_AFRange);
}

BEGIN_MESSAGE_MAP(CMoSetup, CDialogEx)
	ON_COMMAND(IDC_BUTTON14, &CMoSetup::OnUpdate)
	ON_COMMAND(IDC_BUTTON13, &CMoSetup::OnProperties)
	ON_BN_CLICKED(IDC_BUTTON10, &CMoSetup::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CMoSetup::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CMoSetup::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON25, &CMoSetup::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CMoSetup::OnBnClickedButton26)
	ON_BN_CLICKED(IDC_BUTTON27, &CMoSetup::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON28, &CMoSetup::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON46, &CMoSetup::OnBnClickedButton46)
	ON_BN_CLICKED(IDC_BUTTON47, &CMoSetup::OnBnClickedButton47)
	ON_BN_CLICKED(IDC_BUTTON54, &CMoSetup::OnBnClickedButton54)
	ON_BN_CLICKED(IDC_BUTTON55, &CMoSetup::OnBnClickedButton55)
	ON_BN_CLICKED(IDC_BUTTON56, &CMoSetup::OnBnClickedButton56)
	ON_BN_CLICKED(IDC_BUTTON57, &CMoSetup::OnBnClickedButton57)
	ON_BN_CLICKED(IDC_BUTTON58, &CMoSetup::OnBnClickedButton58)
	ON_BN_CLICKED(IDC_BUTTON59, &CMoSetup::OnBnClickedButton59)
	ON_BN_CLICKED(IDC_BUTTON60, &CMoSetup::OnBnClickedButton60)
	ON_BN_CLICKED(IDC_BUTTON61, &CMoSetup::OnBnClickedButton61)
	ON_BN_CLICKED(IDC_BUTTON66, &CMoSetup::OnBnClickedButton66)
	ON_BN_CLICKED(IDC_BUTTON67, &CMoSetup::OnBnClickedButton67)
	ON_BN_CLICKED(IDC_BUTTON69, &CMoSetup::OnBnClickedButton69)
	ON_BN_CLICKED(IDC_BUTTON70, &CMoSetup::OnBnClickedButton70)
	ON_BN_CLICKED(IDC_BUTTON72, &CMoSetup::OnBnClickedButton72)
	ON_BN_CLICKED(IDC_BUTTON73, &CMoSetup::OnBnClickedButton73)
	ON_BN_CLICKED(IDC_BUTTON74, &CMoSetup::OnBnClickedButton74)
	ON_BN_CLICKED(IDC_BUTTON75, &CMoSetup::OnBnClickedButton75)
	ON_BN_CLICKED(IDC_BUTTON76, &CMoSetup::OnBnClickedButton76)
	ON_BN_CLICKED(IDC_BUTTON77, &CMoSetup::OnBnClickedButton77)
	ON_BN_CLICKED(IDC_BUTTON79, &CMoSetup::OnBnClickedButton79)
	ON_BN_CLICKED(IDC_BUTTON82, &CMoSetup::OnBnClickedButton82)
	ON_BN_CLICKED(IDC_BUTTON83, &CMoSetup::OnBnClickedButton83)
	ON_BN_CLICKED(IDC_BUTTON84, &CMoSetup::OnBnClickedButton84)
	ON_BN_CLICKED(IDC_BUTTON87, &CMoSetup::OnBnClickedButton87)
	ON_BN_CLICKED(IDC_BUTTON89, &CMoSetup::OnBnClickedButton89)
	ON_BN_CLICKED(IDC_BUTTON9, &CMoSetup::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON93, &CMoSetup::OnBnClickedButton93)
	ON_BN_CLICKED(IDC_BUTTON94, &CMoSetup::OnBnClickedButton94)
	ON_BN_CLICKED(IDC_BUTTON96, &CMoSetup::OnBnClickedButton96)
	ON_BN_CLICKED(IDC_BUTTON97, &CMoSetup::OnBnClickedButton97)
	ON_BN_CLICKED(IDC_CHECK1, &CMoSetup::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK10, &CMoSetup::OnBnClickedCheck10)
	ON_BN_CLICKED(IDC_CHECK11, &CMoSetup::OnBnClickedCheck11)
	ON_BN_CLICKED(IDC_CHECK15, &CMoSetup::OnBnClickedCheck15)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON92, &CMoSetup::OnBnClickedButton92)
	ON_BN_CLICKED(IDC_BUTTON95, &CMoSetup::OnBnClickedButton95)
	ON_BN_CLICKED(IDC_BUTTON98, &CMoSetup::OnBnClickedButton98)
	ON_BN_CLICKED(IDC_BUTTON15, &CMoSetup::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_AUTO_TILT1, &CMoSetup::OnBnClickedAutoTilt1) // 11212023
	ON_BN_CLICKED(IDC_AUTO_TILT2, &CMoSetup::OnBnClickedAutoTilt2) // 11212023
	ON_BN_CLICKED(IDC_AUTO_TILT3, &CMoSetup::OnBnClickedAutoTilt3)
	ON_BN_CLICKED(IDC_BTN_SHUTTER, &CMoSetup::OnBnClickedBtnShutter)
	ON_BN_CLICKED(IDC_Auto_Focus, &CMoSetup::OnBnClickedAutoFocus)
	ON_BN_CLICKED(IDC_BTN_AT_CNCL, &CMoSetup::OnBnClickedBtnAtCncl)
	ON_BN_CLICKED(IDC_CALIB_RESET, &CMoSetup::OnBnClickedCalibReset)
END_MESSAGE_MAP()

void CMoSetup::OnBnClickedButton9() {
	// z up 10um
	if (bFineStage) MoveR(MAXIS::Z, -incZ / 5.f, 20000, bCheck);
	else MoveR(MAXIS::Z, -incZ, 20000, bCheck);
}

void CMoSetup::OnBnClickedButton10() {
	// z up 1um
	if (bFineStage) MoveR(MAXIS::Z, -incZ / 15.f, 20000, bCheck);
	else MoveR(MAXIS::Z, -incZ / 10.f, 20000, bCheck);
}

void CMoSetup::OnBnClickedButton11() {
	// z down 1 um
	if (bFineStage) MoveR(MAXIS::Z, incZ / 15.f, 20000, bCheck);
	else MoveR(MAXIS::Z, incZ / 10.f, 20000, bCheck);
}

void CMoSetup::OnBnClickedButton12() {
	// z down 10 um
	if (bFineStage) MoveR(MAXIS::Z, incZ / 5.f, 20000, bCheck);
	else MoveR(MAXIS::Z, incZ, 20000, bCheck);
}

void CMoSetup::OnBnClickedButton25() {
	// z home
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->stage.HomeZ();
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::OnBnClickedButton26() {
	// z reset
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->stage.ResetZ();
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::OnBnClickedButton27() {
	// z max
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->MoveA(MAXIS::Z, CMotor::Mpar[int(MAXIS::Z)].max, 25000);
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::OnBnClickedButton28() {
	// z min
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->MoveA(MAXIS::Z, CMotor::Mpar[int(MAXIS::Z)].min, 25000);
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::OnBnClickedButton46() {
	if (bFineStage) MoveR(MAXIS::Z, -incZ / 275.f, 20000, bCheck);
	else MoveR(MAXIS::Z, -incZ / 200.f, 20000, bCheck);
}

void CMoSetup::OnBnClickedButton47() {
	if (bFineStage) MoveR(MAXIS::Z, incZ / 275.f, 20000, bCheck);
	else MoveR(MAXIS::Z, incZ / 200.f, 20000, bCheck);
}

BOOL CMoSetup::OnInitDialog() {
	CDialogEx::OnInitDialog();

	MO = Dev.MC.get();
	P2.z = zN;
	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(bCheck);
	((CButton*)GetDlgItem(IDC_CHECK10))->SetCheck(bFineTilt);
	((CButton*)GetDlgItem(IDC_CHECK11))->SetCheck(bFineStage);

	ICC.bLed = true; ICC.bLaser = false;
	MO->stage.LED(ICC.bLed); MO->stage.Laser(ICC.bLaser);
	MO->stage.SHUTTER(ICC.bShutter); // 12052023
	((CButton*)GetDlgItem(IDC_CHECK15))->SetCheck(bShowSearch);

	OnUpdate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMoSetup::OnUpdate() {
	MO->tilt.UpdatePositions();
	MO->stage.UpdatePositions();
	UpdatePositions();
}

void CMoSetup::OnProperties() {
	CMotPropDlg dlg(Dev.MC.get());
	dlg.DoModal();
}

void CMoSetup::OnBnClickedButton55() {
	if (bFineTilt) TiltY(-tltTUV / 125.f, 15000, bCheck);
	else TiltY(-tltTUV / 10.f, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton60() {
	if (bFineTilt) TiltY(tltTUV / 125.f, 15000, bCheck);
	else TiltY(tltTUV / 10.f, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton57() {
	if (bFineTilt) TiltX(tltTUV / 125.f, 15000, bCheck);
	else TiltX(tltTUV / 10.f, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton58() {
	if (bFineTilt) TiltX(-tltTUV / 125.f, 15000, bCheck);
	else TiltX(-tltTUV / 10.f, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton54() {
	if (bFineTilt) TiltY(-tltTUV / 25.f, 15000, bCheck);
	else TiltY(-tltTUV, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton61() {
	if (bFineTilt) TiltY(tltTUV / 25.f, 15000, bCheck);
	else TiltY(tltTUV, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton56() {
	if (bFineTilt) TiltX(tltTUV / 25.f, 15000, bCheck);
	else TiltX(tltTUV, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton59() {
	if (bFineTilt) TiltX(-tltTUV / 25.f, 15000, bCheck);
	else TiltX(-tltTUV, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton66() {
	if (bBlock) return; bBlock = true;
	CWaitCursor wc;
	float r = 0.50f;
	float PIE2 = 2 * 6.28318f;
	float inc = PIE2 / 17.f;
	CTilt& tilt = MO->tilt;
	SMotPar* M = &MO->stage.Mpar[0];
	P1.Zero(); P2.z = zN;
	for (float t = 0; t <= PIE2 + inc; t += inc) {
		P2.x = float(r * cos(t)); P2.y = float(r * sin(t));
		MTH::SPoint3 N = MTH::SPoint3::Normalize(P2 - P1);
		tilt.SetN(N);
		MTH::SPoint3 P3;
		P3.x = M[int(MOT::MAXIS::X)].now;
		P3.y = M[int(MOT::MAXIS::Y)].now;
		P3.z = GetTZPos(N);
		tilt.DoTilts(P3, N, 15000, bCheck);
	}
	bBlock = false;
}

void CMoSetup::OnBnClickedCheck1() {
	bCheck = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
}

void CMoSetup::OnBnClickedButton67() {
	if (bBlock) return; bBlock = true;
	CTilt& tilt = MO->tilt;
	SMotPar* M = &MO->stage.Mpar[0];
	P1.Zero(); P2.Zero(); P2.z = zN;
	MTH::SPoint3 N = MTH::SPoint3::Normalize(P2 - P1);
	tilt.SetN(N);
	MTH::SPoint3 P3;
	P3.x = M[int(MOT::MAXIS::X)].now;
	P3.y = M[int(MOT::MAXIS::Y)].now;
	P3.z = GetTZPos(N);
	tilt.DoTilts(P3, N, 15000, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CMoSetup::OnBnClickedButton69() {
	TiltXY(0, 0, 15000, bCheck);
}

void CMoSetup::OnBnClickedButton70() {
	MO->tilt.RecvBuf();
	MO->stage.RecvBuf();
}

void CMoSetup::OnBnClickedCheck11() {
	if (bFineStage) bFineStage = false;
	else bFineStage = true;
}

void CMoSetup::OnBnClickedCheck10() {
	if (bFineTilt) bFineTilt = false;
	else bFineTilt = true;
}

void CMoSetup::OnBnClickedButton73() {
	CPiezoB* pPz = MO->Piezo.Get();
	float pos = pPz->GetPos_um() - incPz_um;
	if (pos < Mn_um) pos = Mn_um;
	pPz->Goto(pos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton74() {
	CPiezoB* pPz = MO->Piezo.Get();
	float pos = pPz->GetPos_um() + incPz_um;
	if (pos > Mx_um) pos = Mx_um;
	pPz->Goto(pos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton72() {
	CPiezoB* pPz = MO->Piezo.Get();
	PrevPpos = pPz->GetPos_um();
	pPz->Goto(PZHOMEPOS, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton75() {
	CPiezoB* pPz = MO->Piezo.Get();
	PrevPpos = pPz->GetPos_um();
	pPz->Goto(PrevPpos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton76() {
	CPiezoB* pPz = MO->Piezo.Get();
	float pos = pPz->GetPos_um() - 25 * incPz_um;
	if (pos < Mn_um) pos = Mn_um;
	pPz->Goto(pos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton77() {
	CPiezoB* pPz = MO->Piezo.Get();
	float pos = pPz->GetPos_um() + 25 * incPz_um;
	if (pos > Mx_um) pos = Mx_um;
	pPz->Goto(pos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton79() {
	CPiezoB* pPz = MO->Piezo.Get();
	PrevPpos = pPz->GetPos_um();
	pPz->Goto(0, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton83() {
	// Down
	FringeSearch(false);
}

void CMoSetup::EnableUpDn(bool bEn) {
	cUp.EnableWindow(bEn);
	cDn.EnableWindow(bEn);
	cLs.EnableWindow(bEn);
}

void CMoSetup::OnBnClickedButton82() {
	// Up
	FringeSearch(true);
}

void CMoSetup::FringeSearch(bool bUp) {
	if (!pMoCtrl) { ASSERT(0); return; }
	float score, threshold = 145.f;
	CPiezo& Pz = Dev.GetPiezo();

	float nowPz = Pz.GetPos_um();
	Pz.Goto(0, true);

	EnableUpDn(false);
	bool bStream = pMoCtrl->bStream[0];
	pMoCtrl->StopLiveStreaming(CAM::PRICAM);

	// Setup image format for subsampling = 8
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM);
	if (!pCam->InitCapture(pCam->SCaM.ID, 8)) {
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true); return;
	}

	// Setup fast capture
	IMGL::CIM BG, IM, ID;
	CAM::SCamProp prop;
	if (!pCam->GetCamProp(prop)) {
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true); return;
	}
	if (!pCam->StartFastGrab(prop, pCam->GetSubSample())) {
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true); return;
	}

	if (!pCam->FastGrab(BG, prop)) {
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true); return;
	}
	pMoCtrl->cPicWnd[0].SetImg2(IM); // Copy operation [10/25/2020 yuenl]
	pMoCtrl->cPicWnd[0].Redraw(FALSE);

	Pz.Goto(nowPz, true);
	Yld(750);

	// Setup jog parameter
	MO->stage.SetupZJog(0.03f, bUp);

	// fast grab, save image as BG
	if (!pCam->FastGrab(IM, prop)) {
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true); return;
	}
	ID.SubAbs(IM, BG);

	threshold = ID.IScore() * 1.15f;

	// start jog
	MO->stage.StartZJog();
	// loop
	CString str;
	int loopCnt = 1000;
	while (loopCnt--) {
		if (bFrnSrchStop) break;
		score = ID.IScore(); ShowScore(score, threshold);
		if (score > threshold) break;
		// fast grab IM
		if (!pCam->FastGrab(IM, prop)) break;
		ID.SubAbs(IM, BG);
		if (bShowSearch) {
			pMoCtrl->cPicWnd[0].SetImg2(IM); pMoCtrl->cPicWnd[0].Redraw(FALSE);
		}
		Yld(1); //! Important: allow message pump to run
	} // end loop
	bFrnSrchStop = false;
	// end jog
	if (!MO->stage.StopZJog()) { ASSERT(0); }

	// end fast grab
	pCam->StopFastGrab();

	// restore image format
	pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);

	if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);

	EnableUpDn(true);

	UpdatePositions();
}

void CMoSetup::OnBnClickedButton84() {
	bFrnSrchStop = true;
}

void CMoSetup::OnClose() {
	//if (bStream) { StopLiveStreaming(nowCam); bStream = false; }
	CDialogEx::OnClose();
}

bool CMoSetup::GrabAnImage(IMGL::CIM& Im, CAM::ECAM eID, short subSampling) {
	int ID = int(eID);
	bool bStream = pMoCtrl->bStream[ID];
	if (bStream) pMoCtrl->StopLiveStreaming(eID);

	CAM::SCamProp prop;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID);
	try {
		if (!pCam->InitCapture(pCam->SCaM.ID, subSampling)) {
			throw 101;
		}
		if (!pCam->GetCamProp(prop)) {
			throw 101;
		}
		if (!pCam->StartFastGrab(prop, pCam->GetSubSample())) {
			throw 101;
		}
		if (!Im.IsNull()) Im.Destroy();
		if (!pCam->FastGrab(Im, prop)) {
			throw 102;
		}
	}
	catch (int exCode) {
		if (exCode == 102) {
			pCam->StopFastGrab();
		}
		pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
		if (bStream) pMoCtrl->RestartLiveStreaming(eID);
		EnableUpDn(true);
		return false;
	}
	pCam->StopFastGrab();
	// restore image format
	pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
	if (bStream) pMoCtrl->RestartLiveStreaming(eID);
	EnableUpDn(true);
	return true;
}

void CMoSetup::OnBnClickedButton87() {
	float score, oldpos;
	IMGL::CIM BG, IM, ID;
	CPiezoB* pPz = MO->Piezo.Get();
	oldpos = pPz->GetPos_um();
	pPz->Goto(0, true);
	if (!GrabAnImage(BG, CAM::PRICAM, 8)) return;
	pPz->Goto(oldpos, true);
	if (!GrabAnImage(IM, CAM::PRICAM, 8)) return;

	if (BG.IsNull()) { score = IM.IScore(); }
	else { ID.SubAbs(IM, BG); score = ID.IScore(); }
	ShowScore(score);
}

void CMoSetup::OnBnClickedButton93() {
	CPiezoB* pPz = MO->Piezo.Get();
	float pos = pPz->GetPos_um() - 50 * incPz_um;
	if (pos < Mn_um) pos = Mn_um;
	pPz->Goto(pos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedButton94() {
	CPiezoB* pPz = MO->Piezo.Get();
	float pos = pPz->GetPos_um() + 50 * incPz_um;
	if (pos > Mx_um) pos = Mx_um;
	pPz->Goto(pos, true);
	UpdatePositions();
}

void CMoSetup::OnBnClickedCheck15() {
	if (bShowSearch) bShowSearch = false;
	else bShowSearch = true;
}

void CMoSetup::OnBnClickedButton89() {}

void CMoSetup::OnBnClickedButton96() {
	// S1 level search
	S1LevelSearch();
}

void CMoSetup::OnBnClickedButton97() {
	// S2 level search
	S2LevelSearch();
}

void CMoSetup::S1LevelSearch() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::SECCAM); if (!pCam) return;

	MO->stage.Laser(true); MO->stage.LED(false);

	EnableUpDn(false);
	bool bStreamOn = false;
	if (pMoCtrl) {
		bStreamOn = pMoCtrl->bStream[int(CAM::SECCAM)];
		pMoCtrl->StopLiveStreaming(CAM::SECCAM);
	}
	else Yld(500);

	IMGL::CIM Im, ImT;
	CAM::SCamProp prop;
	try {
		if (!pCam->GetCamProp(prop)) throw 101;
		if (!pCam->StartFastGrab(prop, pCam->GetSubSample())) throw 101;

		if (ImT.IsNull()) {
			CString fname = DosUtil.GetProgramDir().c_str(); fname += _T("IMG\\THUMB.BMP");
			if (ImT.Load(fname) == E_FAIL) throw 102;
		}

		SSpot Spot;
		std::vector<IMGL::SCgI> CG1, CG2;
		SSpotCtx Ctx;
		Ctx.Wdw = 5; Ctx.Sz = 17; Ctx.Thres = 127; Ctx.Obj = 255;
		Ctx.lmt = 8; Ctx.clmt = 2;
		if (!pCam->FastGrab(Im, prop)) throw 102; //ShowImage(Im, CAM::SECCAM);
		if (!pMoCtrl) { ASSERT(0); return; }
		if (Spot.FindSpot(Im, ImT, pMoCtrl->cPicWnd[0])) {
			Spot.DecodeBlob(Im, Ctx, CG1, pMoCtrl->cPicWnd[CAM::SECCAM]);
		}
		TiltX(tltTUV, 15000, bCheck);
		if (!pCam->FastGrab(Im, prop)) throw 102; //ShowImage(Im, CAM::SECCAM);
		if (Spot.FindSpot(Im, ImT, pMoCtrl->cPicWnd[0])) {
			Spot.DecodeBlob(Im, Ctx, CG2, pMoCtrl->cPicWnd[CAM::SECCAM]);
		}
		// determine anchor point
		if ((CG1.size() < 2) || (CG2.size() < 2)) throw 102;
		std::vector<SDLoc> Loc;
		Loc.push_back(SDLoc(CG2[0].Distance(CG1[0]), 0));
		Loc.push_back(SDLoc(CG2[0].Distance(CG1[1]), 1));
		Loc.push_back(SDLoc(CG2[1].Distance(CG1[0]), 2));
		Loc.push_back(SDLoc(CG2[1].Distance(CG1[1]), 3));
		std::sort(Loc.begin(), Loc.end(), &CMoSetup::sorterDLoc);
		// find distance, convert to tilt space
		float dist = 0, sfx = 0.0225f, sfy = -0.0220f;
		IMGL::SCgI Cg;
		switch (Loc[0].idx) {
		case 0:
		case 1: CG2[1].Distance(CG2[0]); Cg = CG2[1];
			break;
		case 2:
		case 3: CG2[0].Distance(CG2[1]); Cg = CG2[0];
			break;
		default: ASSERT(0); break;
		}
		pCam->StopFastGrab();
		if (bStreamOn && pMoCtrl) pMoCtrl->RestartLiveStreaming(CAM::SECCAM);

		// execute tilt
		TiltX(sfx * Cg.dx, 15000, bCheck);
		TiltY(sfy * Cg.dy, 15000, bCheck);
	}
	catch (int exCode) {
		if (exCode == 101) {
			//RestartLiveStreaming(nowCam);
		}
		if (exCode == 102) {
			pCam->StopFastGrab();
			//RestartLiveStreaming(nowCam);
		}
		if (bStreamOn && pMoCtrl) pMoCtrl->RestartLiveStreaming(CAM::SECCAM);
	}
	EnableUpDn(true);
	MO->stage.Laser(ICC.bLaser); MO->stage.LED(ICC.bLed);
}

void CMoSetup::ShowCG(IMGL::SCgI& Cg) {
	float an = Cg.A;
	if (an < 0) an += 3.14159265f;
	float ang = an * 180.f / 3.14159265f;
	CString str;
	str.Format(L"A: %.3f rad %.3f deg  M: %.3f  C: %.3f\ncxy: %d, %d  N= %d",
		an, ang, Cg.M, Cg.C, Cg.cx, Cg.cy, Cg.Count);
	cMsg3.SetWindowTextW(str);
}

void CMoSetup::ShowScore(float score) {
	CString str; str.Format(L"Score: %.3f", score);
	cMsg3.SetWindowTextW(str);
}

void CMoSetup::ShowScore(float score, float thres) {
	CString str; str.Format(L"Score: %.3f\nThreshold: %.3f", score, thres);
	cMsg3.SetWindowTextW(str);
}

void CMoSetup::ShowScoreA(CString& msg, float score1, float score2) {
	CString str; str.Format(L"%s: %.3f, %.3f\nDiff: %.3f", msg, score1, score2, score2 - score1);
	cMsg3.SetWindowTextW(str);
}

void CMoSetup::TakeBG(SLvl& Lvl, IMGL::CIM& Im) {
	IMGL::CIM ID;
	CPiezoB* pPz = MO->Piezo.Get();
	CAM::CCamera* pCam = Lvl.pCam;
	Lvl.oldpos = pPz->GetPos_um();
	pPz->Goto(0, true);
	if (!pCam->FastGrab(Lvl.BG, Lvl.prop)) { throw 102; }
	if (!pCam->FastGrab(Im, Lvl.prop)) { throw 102; }
	CString str = L"BGround: ";
	ID.SubAbs(Im, Lvl.BG); Lvl.bgscore = ID.IScore();
	ShowScoreA(str, Lvl.bgscore, Lvl.bgscore);
	pPz->Goto(Lvl.oldpos, true);
}

void CMoSetup::Snap1(SLvl& Lvl, IMGL::CIM& Im, IMGL::CIM& ID) {
	if (!Lvl.pCam->FastGrab(Im, Lvl.prop)) { throw 102; }
	ShowImage(Im, CAM::PRICAM);
	ID.SubAbs(Im, Lvl.BG); Lvl.score1 = ID.IScore();
	ShowScore(Lvl.score1, 0);
}

void CMoSetup::Snap2(SLvl& Lvl, IMGL::CIM& Im, IMGL::CIM& ID) {
	if (!Lvl.pCam->FastGrab(Im, Lvl.prop)) { throw 102; }
	ShowImage(Im, CAM::PRICAM);
	ID.SubAbs(Im, Lvl.BG); Lvl.score2 = ID.IScore();
	CString str = L"Intensity: ";
	ShowScoreA(str, Lvl.score1, Lvl.score2);
}

void CMoSetup::Leveling(float mv) {
	bool bStream = pMoCtrl->bStream[int(CAM::PRICAM)];
	if (bStream) pMoCtrl->StopLiveStreaming(CAM::PRICAM);

	SLvl Lvl;
	IMGL::CIM Im, ID;
	CPiezoB* pPz = MO->Piezo.Get();
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM);
	Lvl.pCam = pCam;

	try {
		if (!pCam->InitCapture(pCam->SCaM.ID, 8)) { throw 101; }
		if (!pCam->GetCamProp(Lvl.prop)) { throw 101; }
		if (!pCam->StartFastGrab(Lvl.prop, pCam->GetSubSample())) { throw 101; }

		int cnt = 0;

		TakeBG(Lvl, Im);

		for (int i = 0; i < 2; i++) {
			Snap1(Lvl, Im, ID);
			TiltX(mv, 15000, bCheck);
			Snap2(Lvl, Im, ID);
			if (Lvl.DScore() < -0.02f) {
				mv *= -1;
				TiltX(mv / 2.5f, 15000, bCheck);
				Snap2(Lvl, Im, ID);
			}
			Lvl.Next();
			cnt = 0;
			while (cnt < 32) {
				cnt++;
				TiltX(mv, 15000, bCheck);
				Snap2(Lvl, Im, ID);
				if (Lvl.DScore() < 0) {
					TiltX(-mv / 2.f, 15000, bCheck);
					Snap2(Lvl, Im, ID);
					Lvl.Next();
					break;
				}
				Lvl.Next();
			}
			//cnt = 0;
			//while (cnt < 32) {
			//	cnt++;
			//	TiltY(mv, 15000, bCheck);
			//	Snap2(Lvl);
			//	if (Lvl.DScore() < 0) {
			//		TiltY(-mv / 2.f, 15000, bCheck);
			//		break;
			//	}
			//	Lvl.Next();
			//}
			mv /= 4.f;
		}
	}
	catch (int exCode) {
		if (exCode == 102) {
			pCam->StopFastGrab();
		}
		pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true);
		return;
	}
	pCam->StopFastGrab();
	// restore image format
	pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
	if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
	EnableUpDn(true);
}

bool CMoSetup::Leveling(IMGL::CIM& Im, std::vector<IMGL::SCgI>& CG, IMGL::SNxt& nxt, CAM::ECAM eID) {
	CG.clear();
	BYTE Obj = 255;
	IMGL::CIM2 Im1;
	int wd, ht, bpp; Im.GetDim(wd, ht, bpp);
	float thresh = Im.AverW() * Glbs.thresFac;
	Im.BThres(BYTE(thresh), 0, 255); ShowImage(Im, eID);
	Im.Thining(5, 8, Obj); //ShowImage(Im, eID);
	if (!nxt.Segment(Im, Im1, Obj)) return false;
	nxt.Identify(Im, Im1, 50, 3, CG, IMGL::SNxt::BLOB); ShowImage(Im, eID);
	int sz = int(CG.size());
	if (sz < 1) return false;
	IMGL::SCgI Cxy; Cxy.cx = wd / 2; Cxy.cy = ht / 2;
	for (int i = 0; i < sz; i++) {
		CG[i].Distance(Cxy);
	}
	if (sz > 1) {
		std::sort(CG.begin(), CG.end(),
			[](IMGL::SCgI& lhs, IMGL::SCgI& rhs) {
				return lhs.Dst < rhs.Dst;
			}
		);
	}
	ShowCG(CG[0]);
	return true;
}

void CMoSetup::ShowImage(IMGL::CIM& Im, CAM::ECAM eID) {
	if (!pMoCtrl) return;
	int ID = int(eID);
	pMoCtrl->cPicWnd[ID].SetImg2(Im);
	pMoCtrl->cPicWnd[ID].Redraw(TRUE); Yld(100);
}

void CMoSetup::S2LevelSearch() {
	if (!pMoCtrl) return;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	MO->stage.Laser(false); MO->stage.LED(true);

	bool bStreamOn = pMoCtrl->bStream[0];
	EnableUpDn(false);
	pMoCtrl->StopLiveStreaming(CAM::PRICAM);

	IMGL::CIM Im, ID;
	IMGL::SNxt nxt;
	CAM::SCamProp prop;
	std::vector<IMGL::SCgI> CG;
	try {
		if (!pCam->GetCamProp(prop)) throw 101;
		if (!pCam->StartFastGrab(prop, pCam->GetSubSample())) throw 101;

		CPiezoB* pPz = MO->Piezo.Get();
		//SLvl Lvl;
		//Lvl.pCam = pCam;
		//TakeBG(Lvl, Im); ShowImage(Im, CAM::SECCAM);

		IMGL::SCgI C1, C2;
		bool bPosv = false;
		float mv = tltTUV / 30.f;
		if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
		//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
		IMGL::CIM& Ir = Im;
		Leveling(Ir, CG, nxt, CAM::SECCAM);
		if (CG.size() > 0) C1 = CG[0]; else throw 102;
		TiltY(mv, 15000, bCheck);
		if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
		//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
		Ir = Im;
		Leveling(Ir, CG, nxt, CAM::SECCAM);
		if (CG.size() > 0) C2 = CG[0]; else throw 102;
		if (C1.Ang() <= 90.f) {
			if (C2.Ang() < C1.Ang()) {
				mv *= -1; bPosv = true;
				TiltY(mv, 15000, bCheck);
			}
		}
		else {
			if (C2.Ang() > C1.Ang()) {
				mv *= -1; bPosv = true;
				TiltY(mv, 15000, bCheck);
			}
		}
		// find Y tilt and fringe width
		int cnt = 0;
		while (cnt < 16) {
			cnt++;
			TiltY(mv, 15000, bCheck);
			if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
			//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
			Ir = Im;
			if (!Leveling(Ir, CG, nxt, CAM::SECCAM)) break;
			if (CG.size() < 1) throw 102;
			if (bPosv) {
				if (CG[0].Ang() < 90.f) break;
			}
			else {
				if (CG[0].Ang() > 90.f) break;
			}
		}
		// find Y tilt reverse fine
		mv = -mv / 12.f; if (bPosv) bPosv = false; else bPosv = true;
		cnt = 0;
		while (cnt < 16) {
			cnt++;
			TiltY(mv, 15000, bCheck);
			if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
			//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
			Ir = Im;
			if (!Leveling(Ir, CG, nxt, CAM::SECCAM)) break;
			if (CG.size() < 1) throw 102;
			if (bPosv) {
				if (CG[0].Ang() < 90.f) break;
			}
			else {
				if (CG[0].Ang() > 90.f) break;
			}
		}

		// begin find X tilt, base on finge width
		mv = tltTUV / 25.f; bPosv = false;
		if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
		//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
		Ir = Im;
		Leveling(Ir, CG, nxt, CAM::SECCAM);
		if (CG.size() > 0) C1 = CG[0]; else throw 102;
		TiltX(mv, 15000, bCheck);
		if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
		//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
		Ir = Im;
		Leveling(Ir, CG, nxt, CAM::SECCAM);
		if (CG.size() > 0) C2 = CG[0]; else throw 102;
		int Cnt1 = C1.Count, Cnt2 = C2.Count;
		if (Cnt2 < Cnt1) {
			mv *= -1; bPosv = false;
			TiltX(mv, 15000, bCheck);
		}
		Cnt1 = Cnt2;
		cnt = 0;
		while (cnt < 16) {
			cnt++;
			TiltX(mv, 15000, bCheck);
			if (!pCam->FastGrab(Im, prop)) throw 102; ShowImage(Im, CAM::PRICAM);
			//SubBG(Lvl, Im, ID); ShowImage(ID, CAM::SECCAM);
			Ir = Im;
			if (!Leveling(Ir, CG, nxt, CAM::SECCAM)) break;
			if (CG.size() < 1) throw 102;
			Cnt2 = CG[0].Count;
			if (bPosv) {
				if ((Cnt2 - Cnt1) > 0) {
					TiltX(-mv, 15000, bCheck); break;
				}
			}
			else {
				if ((Cnt2 - Cnt1) < 0) {
					TiltX(-mv, 15000, bCheck); break;
				}
			}
			Cnt1 = Cnt2;
		}

		pCam->StopFastGrab();
		if (bStreamOn) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
	}
	catch (int exCode) {
		if (exCode == 102) {
			pCam->StopFastGrab();
		}
		if (bStreamOn) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
	}
	EnableUpDn(true);
	MO->stage.Laser(ICC.bLaser); MO->stage.LED(ICC.bLed);
}

void CMoSetup::Yld(DWORD nTime) {
	ULONGLONG time1;
	time1 = GetTickCount64();
	while ((GetTickCount64() - time1) < nTime) {
		MSG msg;
		while (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

bool CMoSetup::Replace(USHORT from, IMGL::CIM& Im, IMGL::CIM2& Im2, IMGL::SCgI& CG, COLORREF cr) {
	int wd, ht, bpp; Im.GetDim(wd, ht, bpp); if (bpp != 24) return false;
	IMGL::SLoc stL, edL;

	if (fabsf(CG.M) > 1) {
		stL.y = ht / 4; stL.x = int((stL.y - CG.C) / CG.M);
		edL.y = 3 * stL.y; edL.x = int((edL.y - CG.C) / CG.M);
	}
	else {
		stL.x = wd / 4; stL.y = int(CG.M * stL.x + CG.C);
		edL.x = 3 * stL.x; edL.y = int(CG.M * edL.x + CG.C);
	}

	IMGL::CRGB C(cr);
	CG.Count = Im.ReplacePixel(Im2, from, cr);
	if (CG.Count) CG.SetColor(cr);
	if (CG.Count > 1500) {
		Im.DrawLine(stL.x, stL.y, edL.x, edL.y, C.cR, C.cG, C.cB);
		Im.DrawCross(CG.cx, CG.cy, int(0.05f * wd), C.cR, C.cG, C.cB);
	}
	else {
		CG.Count = 0; return false;
	}
	return true;
}

void CMoSetup::OnBnClickedButton92() {
	bool bStream = pMoCtrl->bStream[int(CAM::PRICAM)];
	if (bStream) pMoCtrl->StopLiveStreaming(CAM::PRICAM);

	IMGL::CIM Im, ID;
	CPiezoB* pPz = MO->Piezo.Get();
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM);
	SLvl Lvl; Lvl.pCam = pCam;

	try {
		if (!pCam->InitCapture(pCam->SCaM.ID, 8)) { throw 101; }
		if (!pCam->GetCamProp(Lvl.prop)) { throw 101; }
		if (!pCam->StartFastGrab(Lvl.prop, pCam->GetSubSample())) { throw 101; }

		int cnt = 0;
		float mv = 30.f;
		float prev, pos;

		TakeBG(Lvl, Im);

		for (int i = 0; i < 2; i++) {
			Snap1(Lvl, Im, ID);
			pos = pPz->GetPos_um() + mv * incPz_um; pPz->Goto(pos, true);
			Snap2(Lvl, Im, ID);
			if ((Lvl.DScore()) < -0.05f) {
				//pPz->Goto(Lvl.oldpos, true);
				mv *= -1;
			}
			Lvl.Next();
			cnt = 0;
			while (cnt < 64) {
				cnt++;
				prev = pos; pos = pPz->GetPos_um() + mv * incPz_um; pPz->Goto(pos, true);
				Snap2(Lvl, Im, ID);
				if (Lvl.DScore() < 0) {
					pPz->Goto(prev, true);
					break;
				}
				Lvl.Next();
			}
			mv /= 5.0f;
		}
	}

	catch (int exCode) {
		if (exCode == 102) {
			pCam->StopFastGrab();
		}
		pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
		if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
		EnableUpDn(true);
		return;
	}
	pCam->StopFastGrab();
	// restore image format
	pCam->InitCapture(pCam->SCaM.ID, pCam->subSampling);
	if (bStream) pMoCtrl->RestartLiveStreaming(CAM::PRICAM);
	EnableUpDn(true);
}

void CMoSetup::OnBnClickedButton95() {
	// expand
	float mv = -tltTUV / 12.5f;
	Leveling(mv);
}

void CMoSetup::OnBnClickedButton98() {
	float mv = -tltTUV / 125.f;
	Leveling(mv);
}

void CMoSetup::OnBnClickedButton15() {
	IMGL::CIM Im;
	IMGL::SNxt nxt;
	std::vector<IMGL::SCgI> CG;
	if (GrabAnImage(Im, CAM::PRICAM, 8)) {
		//ShowImage(Im, CAM::SECCAM);
		Leveling(Im, CG, nxt, CAM::SECCAM);
		ShowImage(Im, CAM::SECCAM);
	}
}

void CMoSetup::getFringeDirection() {
	IMGL::CIM Im;
	while (!Dev.Cam.Grab(Im, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));

	int wd, ht, bpp;
	Im.GetDim(wd, ht, bpp);
	COLORREF colour;
	double whiteChn;
	double befXMoveH = -1000, aftXMoveH = -1000, befYMoveV = -1000, aftYMoveV = -1000;
	double befXMoveHPos = -1, aftXMoveHPos = -1, befYMoveVPos = -1, aftYMoveVPos = -1;

	for (int i = 0; i < wd; i++) { // Horizontal
		colour = Im.GetPixel(i, ht / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		if (whiteChn > befXMoveH) {
			befXMoveH = whiteChn;
			befXMoveHPos = i;
		}
	}

	for (int i = 0; i < ht; i++) { // Vertical
		colour = Im.GetPixel(wd / 2, i);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		if (whiteChn > befYMoveV) {
			befYMoveV = whiteChn;
			befYMoveVPos = i;
		}
	}

	Im.Detach();

	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	float targetPos = Piezo.GetPos_um() + (250.0 / 1000.0);
	PiezoMoveAcc(targetPos);
	UpdatePositions();

	while (!Dev.Cam.Grab(Im, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));

	for (int i = 0; i < wd; i++) { // Horizontal
		colour = Im.GetPixel(i, ht / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		if (whiteChn > aftXMoveH) {
			aftXMoveH = whiteChn;
			aftXMoveHPos = i;
		}
	}

	for (int i = 0; i < ht; i++) { // Vertical
		colour = Im.GetPixel(wd / 2, i);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		if (whiteChn > aftYMoveV) {
			aftYMoveV = whiteChn;
			aftYMoveVPos = i;
		}
	}

	Im.Detach();
}

void CMoSetup::OnBnClickedAutoTilt1() {
	// TODO: Add your control notification handler code here
	getFringeDirection();
	//AutoTilt1();
}

bool CMoSetup::AutoTilt1() {
	//bool orgbStreamStat;
	//int nTimes = 1;
	//while (nTimes++ <= 1) {
	//	bool orgbLedStat = ICC.bLed;
	//	bool orgbLaserStat = ICC.bLaser;
	//	/*if (ICC.bLed) {
	//		ICC.bLed = false;
	//		MO->stage.LED(ICC.bLed);
	//	}
	//	if (!ICC.bLaser) {
	//		ICC.bLaser = true;
	//		MO->stage.Laser(ICC.bLaser);
	//	}*/

	//	MO->stage.LED(false);
	//	Sleep(25);
	//	MO->stage.Laser(true);
	//	Sleep(500);

	//	if (!ICC.bShutter) {
	//		ICC.bShutter = true; // Shutter Close
	//		MO->stage.SHUTTER(ICC.bShutter);
	//	}

	//	IMGL::CIM Im;
	//	CAM::ECAM eID = CAM::SECCAM;
	//	if (Dev.Cam.Grab(Im, eID, Dev.Cam.pCm[eID]->subSampling)) {
	//		Im.Save(L"C:\\WLIN\\IMG\\orgImg.BMP");
	//	}
	//	else {
	//		AfxMessageBox(L"Image Capture Failed!!!");
	//	}
	//	/*if (orgbLedStat) {
	//		ICC.bLed = true;
	//		MO->stage.LED(ICC.bLed);
	//	}
	//	if (!orgbLaserStat) {
	//		ICC.bLaser = false;
	//		MO->stage.Laser(ICC.bLaser);
	//	}*/

	//	MO->stage.LED(true);
	//	MO->stage.Laser(false);

	//	int height, width, bpp;
	//	Im.GetDim(width, height, bpp);
	//	std::vector<std::vector<int>>pixelValues;
	//	int iVal;
	//	for (int y = 0; y < height; y++) {
	//		std::vector<int>rows;
	//		for (int x = 0; x < width; x++) {
	//			COLORREF cr = Im.GetPixel(x, y);
	//			((int)GetRValue(cr) < 25) ? iVal = 5 : iVal = (int)GetRValue(cr); // Original Working
	//			//((int)GetRValue(cr) < 15) ? iVal = 5 : iVal = (int)GetRValue(cr);
	//			rows.push_back(iVal);
	//		}
	//		pixelValues.push_back(rows);
	//	}

	//	for (int y = 0; y < height; y++) {
	//		if (AT.orgPixelValues.size() && AT.orgPixelValues[y].size()) AT.orgPixelValues[y].clear();
	//	}
	//	AT.orgPixelValues.clear();
	//	AT.orgPixelValues = pixelValues;

	//	ofstream myF1("C:\\WLIN\\IMG\\orgPixels.csv");
	//	for (int x = 0; x < pixelValues.size(); x++) {
	//		for (int y = 0; y < pixelValues[0].size(); y++) {
	//			myF1 << pixelValues[x][y] << ",";
	//		}
	//		myF1 << endl;
	//	}
	//	myF1.close();

	//	for (int i = 0; i < AT.largest_components.size(); i++) {
	//		AT.largest_components[i].clear();
	//		AT.tmp_cmps[i].clear();
	//	}
	//	AT.largest_components.clear();
	//	AT.tmp_cmps.clear();

	//	AT.searchingReg(pixelValues, AT.THRESH::FIRST);
	//	AT.searchingReg(pixelValues, AT.THRESH::SECOND);
	//	AT.largest_components;

	//	//if (LasMatLib.largest_components.size() < 2) continue;

	//	// Create BITMAP Image

	//	IMGL::CIM Im1;
	//	Im1.Create(width, height, 24);
	//	COLORREF rgbBlack = 0x00000000;
	//	COLORREF rgbWhite = 0x00FFFFFF;
	//	for (int y = 0; y < height; y++) {
	//		for (int x = 0; x < width; x++) {
	//			Im1.SetPixel(x, y, rgbBlack);
	//		}
	//	}

	//	// Create an output image with the two largest components

	//	for (const auto& component : AT.tmp_cmps) {
	//		for (const auto& pixel : component) {
	//			Im1.SetPixel(pixel.y, pixel.x, rgbWhite);
	//		}
	//	}

	//	if (AT.stats.y1 == -1 || AT.stats.y2 == -1 || AT.stats.x1 == -1 || AT.stats.x2 == -1) {
	//		//AfxMessageBox(L"Laser Matching Failed!!!", MB_ICONINFORMATION);
	//		return false;
	//	}

	//	// Marked COM Pixel
	//	Im1.SetPixel(AT.stats.y1, AT.stats.x1, 255, 0, 0);
	//	if (AT.largest_components.size() > 1) {
	//		Im1.SetPixel(AT.stats.y2, AT.stats.x2, 0, 255, 0);
	//	}

	//	// Save Modified Image
	//	Im1.Save(L"C:\\WLIN\\IMG\\modImg.BMP");

	//	// Calculate Distance
	//	float pixelSize = 6.9;
	//	if (AT.largest_components.size() > 1) {
	//		float X = -(((AT.stats.y1 - AT.stats.y2) * pixelSize) / 1000);
	//		float Y = -(((AT.stats.x1 - AT.stats.x2) * pixelSize) / 1000);
	//		TiltXY(X, Y, 15000, bCheck);
	//	}
	//}

	//if (ICC.bShutter) {
	//	ICC.bShutter = false; // Shutter Open
	//	MO->stage.SHUTTER(ICC.bShutter);
	//}

	//OnBnClickedAutoTilt3(); // PS Algorithm

	return true;
}

void CMoSetup::getFringeCenter(int _totRange, double _stepSize) {
	PiezoMoveAcc(50); // Piezo Pos: 40um
	IMGL::CIM img;
	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	CAM::ECAM eID = CAM::PRICAM;
	int ht, wd, bpp;
	double mxIntensity = -100000, centerFringePos = -1;
	double targetPos = max(Piezo.GetPos_um() - (_totRange / 2), Mn_um + (_totRange / 2));
	double stepSize = _stepSize; // nm
	int totStep = _totRange; // um
	int st = -((totStep / stepSize) / 2);
	int ed = (totStep / stepSize) / 2;
	Piezo.Goto(targetPos, false);

	double fstQMxH = -100000, ThrdQMxH = -100000, fstQPiPosH = 0, ThrdQPiPosH = 0;
	int fstImgPosH = -1, scndImgPosH = -1, ThrdImgPosH = -1;
	double fstQMxV = -100000, ThrdQMxV = -100000, fstQPiPosV = 0, ThrdQPiPosV = 0;
	int fstImgPosV = -1, scndImgPosV = -1, ThrdImgPosV = -1;

	fringDirX = FRNGDIR::INVD, fringDirY = FRNGDIR::INVD;
	pixDisX = -1, PiDisX = -1, pixDisY = -1, PiDisY = -1;

	Yld(15);

	int window = 3;

	Dev.Cam.SetTriggerMode(CAM::PRICAM, true);

	std::ofstream ff("fringeCenteringIntensity.csv");
	for (int i = st; i <= ed; i++) {
		if (Piezo.GetPos_um() > (Mx_um - (_totRange / 2))) {
			break;
		}
		//Yld(15);
		Dev.Cam.ExecuteTrigger(CAM::PRICAM);
		while (!Dev.Cam.GetBitmapImage(img, CAM::PRICAM));
		//if (Dev.Cam.Grab(img, eID, Dev.Cam.pCm[eID]->subSampling)) {
			// Vertical And Horizontal Both 2nd quarter
		img.GetDim(wd, ht, bpp);
		double whiteChn = 0.0, whiteChn1stQrtrH = 0.0, whiteChn3rdQrtrH = 0.0, whiteChn1stQrtrV = 0.0, whiteChn3rdQrtrV = 0.0;
		COLORREF colour;
		for (int j = -window; j <= window; j++) {
			colour = img.GetPixel(wd / 2 + j, ht / 2);
			whiteChn += (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
			/*if (whiteChn > mxIntensity) {
				centerFringePos = Piezo.GetPos_um();
				mxIntensity = whiteChn;
				scndImgPosH = i;
			}*/
			// Horizontal 1st And 3rd Quarter
			colour = img.GetPixel(wd / 4 + j, ht / 2);
			whiteChn1stQrtrH += (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
			/*if (whiteChn1stQrtrH > fstQMxH) {
				fstQMxH = whiteChn1stQrtrH;
				fstImgPosH = i;
				fstQPiPosH = Piezo.GetPos_um();
			}*/
			colour = img.GetPixel(3 * wd / 4 + j, ht / 2);
			whiteChn3rdQrtrH += (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
			/*if (whiteChn3rdQrtrH > ThrdQMxH) {
				ThrdQMxH = whiteChn3rdQrtrH;
				ThrdImgPosH = i;
				ThrdQPiPosH = Piezo.GetPos_um();
			}*/
			// Vertical 1st And 3rd Quarter
			if (j <= -2 && j >= -2) {
				colour = img.GetPixel(wd / 2, ht / 4 + j);
				whiteChn1stQrtrV += (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
				/*if (whiteChn1stQrtrV > fstQMxV) {
					fstQMxV = whiteChn1stQrtrV;
					fstImgPosV = i;
					fstQPiPosV = Piezo.GetPos_um();
				}*/
				colour = img.GetPixel(wd / 2, 3 * ht / 4 + j);
				whiteChn3rdQrtrV += (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
			}/*if (whiteChn3rdQrtrV > ThrdQMxV) {
				ThrdQMxV = whiteChn3rdQrtrV;
				ThrdImgPosV = i;
				ThrdQPiPosV = Piezo.GetPos_um();
			}*/
			//ff << whiteChn1stQrtrH << "," << whiteChn << "," << whiteChn3rdQrtrH << std::endl;
		}

		if (whiteChn > mxIntensity) {
			centerFringePos = Piezo.GetPos_um();
			mxIntensity = whiteChn;
			scndImgPosH = i;
		}

		if (whiteChn1stQrtrH > fstQMxH) {
			fstQMxH = whiteChn1stQrtrH;
			fstImgPosH = i;
			fstQPiPosH = Piezo.GetPos_um();
		}

		if (whiteChn3rdQrtrH > ThrdQMxH) {
			ThrdQMxH = whiteChn3rdQrtrH;
			ThrdImgPosH = i;
			ThrdQPiPosH = Piezo.GetPos_um();
		}

		if (whiteChn1stQrtrV > fstQMxV) {
			fstQMxV = whiteChn1stQrtrV;
			fstImgPosV = i;
			fstQPiPosV = Piezo.GetPos_um();
		}

		if (whiteChn3rdQrtrV > ThrdQMxV) {
			ThrdQMxV = whiteChn3rdQrtrV;
			ThrdImgPosV = i;
			ThrdQPiPosV = Piezo.GetPos_um();
		}

		Piezo.Goto(Piezo.GetPos_um() + stepSize, false);
		Piezo.Yld(2);
		UpdatePositions();
		img.Detach();
	}
	Dev.Cam.SetTriggerMode(CAM::PRICAM, false);
	ff.close();
	Piezo.Goto(centerFringePos, false);
	UpdatePositions();
	if (fstImgPosH < ThrdImgPosH) fringDirX = FRNGDIR::LTR;
	else if (fstImgPosH > ThrdImgPosH) fringDirX = FRNGDIR::RTL;
	if (fstImgPosV < ThrdImgPosV) fringDirY = FRNGDIR::TTD;
	else if (fstImgPosV > ThrdImgPosV) fringDirY = FRNGDIR::DTT;

	pixDisX = (((wd / 4) * 1.5) / wd) * 1000; // Horizontal - um
	pixDisY = (((ht / 4) * 1.2) / ht) * 1000; // Vertical - um
	PiDisX = (abs(centerFringePos - fstQPiPosH) + abs(centerFringePos - ThrdQPiPosH)) / 2.0;
	PiDisY = (abs(centerFringePos - fstQPiPosV) + abs(centerFringePos - ThrdQPiPosV)) / 2.0;
	//TRACE("1/n");
	//PiDisY = abs(centerFringePos - fstQPiPosV);
	//double angle = atan(PiDisY / pixDisY);

	//MoveR(MOT::MAXIS::T, incTUV / 100.f, 20000, bCheck);
}

void CMoSetup::OnBnClickedAutoTilt2() {
	// TODO: Add your control notification handler code here
	//getFringeCenter();
	//FringeStripeAnalysis1(); // Genarel Algorithm
	AfxMessageBox(L"Tilt-2 Is Done", MB_ICONINFORMATION);
}

void CMoSetup::OnBnClickedAutoTilt3() {
	// TODO: Add your control notification handler code here
	//getFringeCenter();
	FringeStripeAnalysis2(); // PS Algorithm
	AfxMessageBox(L"Tilt-2 Is Done", MB_ICONINFORMATION);
}

bool CMoSetup::IsFringeExist() {
	IMGL::CIM Im;
	Sleep(15);
	while (!Dev.Cam.Grab(Im, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));
	Im.Save(L"C:\\WLIN\\IMG\\fringExist.bmp");
	int height, width, bpp;
	Im.GetDim(width, height, bpp);

	COLORREF colour;
	int whiteChn;
	int mx = -100000, mn = 100000, mx1 = -100000, mn1 = 100000;

	// Vertical
	for (int ro = 0; ro < height - 1; ro++) {
		colour = Im.GetPixel(width / 2, ro);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		mx = max(mx, whiteChn);
		mn = min(mn, whiteChn);
	}
	if (mx - mn > 25) {
		Im.Detach();
		return true;
	}

	// Horizontal

	for (int co = 0; co < width - 1; co++) {
		colour = Im.GetPixel(co, height / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		mx1 = max(mx1, whiteChn);
		mn1 = min(mn1, whiteChn);
	}
	if (mx1 - mn1 > 25) {
		Im.Detach();
		return true;
	}
	Im.Detach();
	return (max(mx, mx1) > 215);
}

// PS Algorithm START ---------

bool CMoSetup::FringeStripeAnalysis2() {
	RES res;
	res = FringePeakCnt();
	if (res.hPeak > 8 && res.vPeak > 8) {
		ATWithGeo();
		return true;
	}
	else {
		return false;
	}

	//isAtCancel = false;
	//RES res;
	//int idx = 1, nTime = 10;
	//statusFlag1 = 0, statusFlag2 = 0;
	//getFringeCenter(20, 0.1);
	//if (fringDirX == FRNGDIR::INVD && fringDirY == FRNGDIR::INVD) return;
	//while (idx++ < nTime) {
	//	if (isAtCancel) break;
	//	RES res;
	//	if (!IsFringeExist()) {
	//		AutoFocus(150, 5, FALSE);
	//		getFringeCenter(20, 0.1);
	//	}
	//	res = FringePeakCnt();
	//	if (res.vPeak > 3 && res.hPeak > 3) {
	//		if (res.vPeak > res.hPeak) {
	//			VerticalFringeDirectionAdjustmentWithPS();
	//			//HorizontalFringeDirectionAdjustmentWithPS();
	//		}
	//		else {
	//			HorizontalFringeDirectionAdjustmentWithPS();
	//			//VerticalFringeDirectionAdjustmentWithPS();
	//		}
	//	}
	//	else if (res.vPeak > 3)	VerticalFringeDirectionAdjustmentWithPS();
	//	else if (res.hPeak > 3) HorizontalFringeDirectionAdjustmentWithPS();
	//	else break;
	//}
}

bool CMoSetup::HorizontalFringeDirectionAdjustmentWithPS() {
	// Horizontal Adjustment

	int range = 50;
	float step = 0.020; // micron
	int mn1 = 100000, mx1 = -100000;
	int i;

	//if (statusFlag1 == 0) {
	if (fringDirX == FRNGDIR::LTR) {
		for (i = 1; i <= range; i++) {
			if (isAtCancel) break;
			if (!IsFringeExist()) {
				AutoFocus(150, 5, FALSE);
				getFringeCenter(10, 0.1);
			}
			int HPeakCnt = FringePeakCnt().hPeak;
			int HPeakDis = FringePeakCnt().hPeakDis;
			if (HPeakCnt < 3) step = 0.002; // micron
			else if (HPeakCnt < 6) step = 0.005; // micron
			else if (HPeakCnt < 9) step = 0.015; // micron
			else if (HPeakCnt > 8) step = 0.030; // micron
			if (HPeakCnt < 9) {
				mn1 = min(mn1, HPeakCnt);
				if (HPeakCnt < 4) {
					break;
				}
				if (HPeakCnt < 7 && abs(mn1 - HPeakCnt) > 2) {
					statusFlag1 = 1;
					break;
				}
				TiltX(step, 15000, bCheck);
			}
			/*else if (HPeakDis > 0) {
				mx1 = max(mx1, HPeakDis);
				if (mx1 - HPeakDis > 10) {
					TiltX(-step, 15000, bCheck);
					statusFlag1 = 1;
					break;
				}
				TiltX(step, 15000, bCheck);
			}*/
		}
	}
	//if (statusFlag1 == 1) {
	else if (fringDirX == FRNGDIR::RTL) {
		mn1 = 100000, mx1 = -100000;
		for (i = 1; i <= range; i++) {
			if (isAtCancel) break;
			if (!IsFringeExist()) {
				AutoFocus(150, 5, FALSE);
				getFringeCenter(10, 0.1);
			}
			int HPeakCnt = FringePeakCnt().hPeak;
			int HPeakDis = FringePeakCnt().hPeakDis;
			if (HPeakCnt < 3) step = 0.002; // micron
			else if (HPeakCnt < 6) step = 0.005; // micron
			else if (HPeakCnt < 9) step = 0.015; // micron
			else if (HPeakCnt > 8) step = 0.030; // micron
			mn1 = min(mn1, HPeakCnt);
			if (HPeakCnt < 5) {
				break;
			}
			if (HPeakCnt < 7 && abs(mn1 - HPeakCnt) > 1) {
				break;
			}
			TiltX(-step, 15000, bCheck);
		}
	}

	return statusFlag1;
}

bool CMoSetup::VerticalFringeDirectionAdjustmentWithPS() {
	// Vertical Adjustment

	int range = 50;
	float step = 0.020; // micron
	int mn2 = 100000, mx2 = -100000;
	int i;

	//if (statusFlag2 == 0) {
	if (fringDirY == FRNGDIR::DTT) {
		for (i = 1; i <= range; i++) {
			if (isAtCancel) break;
			if (!IsFringeExist()) {
				AutoFocus(150, 5, FALSE);
				getFringeCenter(10, 0.1);
			}
			int VPeakCnt = FringePeakCnt().vPeak;
			int VPeakDis = FringePeakCnt().vPeakDis;
			if (VPeakCnt < 3) step = 0.002; // micron
			else if (VPeakCnt < 6) step = 0.005; // micron
			else if (VPeakCnt < 9) step = 0.045; // micron
			else if (VPeakCnt > 8) step = 0.075; // micron
			if (VPeakCnt < 9) {
				mn2 = min(mn2, VPeakCnt);
				if (VPeakCnt < 4) {
					break;
				}
				if (VPeakCnt < 7 && abs(mn2 - VPeakCnt) > 2) {
					statusFlag2 = 1;
					break;
				}
				TiltY(step, 15000, bCheck);
			}
			/*else if (VPeakDis > 0) {
				mx2 = max(mx2, VPeakDis);
				if (mx2 - VPeakDis > 10) {
					TiltY(step, 15000, bCheck);
					statusFlag2 = 1;
					break;
				}
				TiltY(step, 15000, bCheck);
			}*/
		}
	}
	//if (statusFlag2 == 1) {
	else if (fringDirY == FRNGDIR::TTD) {
		mn2 = 100000, mx2 = -100000;;
		for (i = 1; i <= range; i++) {
			if (isAtCancel) break;
			if (!IsFringeExist()) {
				AutoFocus(150, 5, FALSE);
				getFringeCenter(10, 0.1);
			}
			int VPeakCnt = FringePeakCnt().vPeak;
			int VPeakDis = FringePeakCnt().vPeakDis;
			if (VPeakCnt < 3) step = 0.002; // micron
			else if (VPeakCnt < 6) step = 0.005; // micron
			else if (VPeakCnt < 9) step = 0.045; // micron
			else if (VPeakCnt > 8) step = 0.075; // micron
			mn2 = min(mn2, VPeakCnt);
			if (VPeakCnt < 4) {
				break;
			}
			if (VPeakCnt < 7 && abs(mn2 - VPeakCnt) > 1) {
				break;
			}
			TiltY(-step, 15000, bCheck);
		}
	}
	return statusFlag2;
}

// Function to compute the phase map from four fringe patterns
void CMoSetup::computePhaseMap(const double* I0, const double* I1, const double* I2, const double* I3, int size, double* phaseMap) {
	ofstream pVal("C:\\WLIN\\IMG\\orgPVal.csv");
	ofstream NpVal("C:\\WLIN\\IMG\\norPVal.csv");
	ofstream FDer("C:\\WLIN\\IMG\\firstDer.csv");
	for (int i = 0; i < size; ++i) {
		// Compute the phase map using the four-step algorithm
		double I_ac = (I0[i] - I2[i]) / 2.0;
		double I_bd = (I1[i] - I3[i]) / 2.0;

		phaseMap[i] = std::atan2(I_bd, I_ac);
		pVal << phaseMap[i] << endl;

		// Normalize phase values to the range [0, 2*pi]
		phaseMap[i] = std::fmod(phaseMap[i] + 2 * M_PI, 2 * M_PI);
		NpVal << phaseMap[i] << endl;
	}

	for (int i = 0; i < size - 1; i++) FDer << (phaseMap[i] - phaseMap[i + 1]) << endl;

	pVal.close();
	NpVal.close();
	FDer.close();
}

// Function to count fringe peaks in the phase map
std::pair<int, int> CMoSetup::countFringePeaks(const double* phaseMap, int size, double threshold) {
	int peakCount = 0, peakToPeakDis = -1;
	vector<int>peakDis;
	// Count peaks by checking where the phase changes rapidly
	int dis = 1;
	for (int i = 0; i < size - 1; i++) {
		double phaseDiff = fabs(phaseMap[i] - phaseMap[i + 1]);
		if (phaseDiff >= threshold && (dis == 1 || (i - dis) >= 35)) {
			peakCount++;
			if (dis != 1) {
				peakDis.push_back(abs(i - dis));
			}
			dis = i;
		}
	}

	int sum = 0;
	if (peakDis.size() > 8) {
		for (int i = 4; i < peakDis.size() - 4; i++) {
			sum += peakDis[i - 1];
		}
		peakToPeakDis = sum / (peakDis.size() - 8);
	}

	return { peakCount, peakToPeakDis };
}

bool CMoSetup::PiezoMoveAcc(double targetPos) {
	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	int idx = 1;
	while (idx++ <= 3) {
		if (!Piezo.Goto(targetPos, false)) return false;
		if (abs(targetPos - Piezo.GetPos_um()) <= 0.005) break;
		Sleep(2);
	}
	return true;
}

RES CMoSetup::FringePeakCnt() {
	RES res;
	IMGL::CIM image1, image2, image3, image4;
	CAM::ECAM eID = CAM::PRICAM;
	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	float now = 0.070; // 70 nm

	int currentNanoPos = (int)Piezo.GetPos_um(); // micron unit

	if (currentNanoPos > (Mx_um - 10) || currentNanoPos < (Mn_um + 10)) {
		AfxMessageBox(L"Piezo Will Hit Limit!!!", MB_ICONINFORMATION);
		return res;
	}

	Yld(2);
	if (!PiezoMoveAcc(Piezo.GetPos_um() - 0.105)) {
		AfxMessageBox(L"Nano Motion Failed", MB_ICONERROR);
		return res;
	}

	Yld(2);
	while (!Dev.Cam.Grab(image1, eID, Dev.Cam.pCm[eID]->subSampling)) {
		Yld(2);
	}

	Yld(2);
	if (!PiezoMoveAcc(Piezo.GetPos_um() + now)) {
		AfxMessageBox(L"Nano Motion Failed", MB_ICONERROR);
		return res;
	}

	Yld(2);
	while (!Dev.Cam.Grab(image2, eID, Dev.Cam.pCm[eID]->subSampling)) {
		Yld(2);
	}

	Yld(2);
	if (!PiezoMoveAcc(Piezo.GetPos_um() + now)) {
		AfxMessageBox(L"Nano Motion Failed", MB_ICONERROR);
		return res;
	}

	Yld(2);
	while (!Dev.Cam.Grab(image3, eID, Dev.Cam.pCm[eID]->subSampling)) {
		Yld(2);
	}

	Yld(2);
	if (!PiezoMoveAcc(Piezo.GetPos_um() + now)) {
		AfxMessageBox(L"Nano Motion Failed", MB_ICONERROR);
		return res;
	}

	Yld(2);
	while (!Dev.Cam.Grab(image4, eID, Dev.Cam.pCm[eID]->subSampling)) {
		Yld(2);
	}

	Yld(2);
	if (!PiezoMoveAcc(Piezo.GetPos_um() - 0.105)) {
		AfxMessageBox(L"Nano Motion Failed", MB_ICONERROR);
		return res;
	}

	UpdatePositions();

	image1.Save(L"C:\\WLIN\\IMG\\Phase_0.BMP");
	image2.Save(L"C:\\WLIN\\IMG\\Phase_90.BMP");
	image3.Save(L"C:\\WLIN\\IMG\\Phase_180.BMP");
	image4.Save(L"C:\\WLIN\\IMG\\Phase_270.BMP");

	// Vertical Peak Count

	int height, width, bpp;
	image1.GetDim(width, height, bpp);

	double* I0 = new double[height];
	double* I1 = new double[height];
	double* I2 = new double[height];
	double* I3 = new double[height];
	double* phaseMap = new double[height];

	COLORREF colour;
	double whiteChn;

	for (int ro = 0; ro < height; ro++) {
		colour = image1.GetPixel(width / 2, ro);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I0[ro] = whiteChn;
		colour = image2.GetPixel(width / 2, ro);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I1[ro] = whiteChn;
		colour = image3.GetPixel(width / 2, ro);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I2[ro] = whiteChn;
		colour = image4.GetPixel(width / 2, ro);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I3[ro] = whiteChn;
	}

	// Compute the phase map
	computePhaseMap(I0, I1, I2, I3, height, phaseMap);

	// Set the threshold for fringe peak detection
	double threshold = 5.9;

	// Count fringe peaks in the phase map
	std::pair<int, int> peakCount = countFringePeaks(phaseMap, height, threshold);

	res.vPeak = peakCount.first;
	res.vPeakDis = peakCount.second;

	delete[] I0; delete[] I1; delete[] I2; delete[] I3; delete[] phaseMap;

	// Horizontal Peak Count

	I0 = new double[width];
	I1 = new double[width];
	I2 = new double[width];
	I3 = new double[width];
	phaseMap = new double[width];

	for (int co = 0; co < width; co++) {
		colour = image1.GetPixel(co, height / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I0[co] = whiteChn;
		colour = image2.GetPixel(co, height / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I1[co] = whiteChn;
		colour = image3.GetPixel(co, height / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I2[co] = whiteChn;
		colour = image4.GetPixel(co, height / 2);
		whiteChn = (0.2989 * (int)GetRValue(colour)) + (0.5870 * (int)GetGValue(colour)) + (0.1140 * (int)GetBValue(colour));
		I3[co] = whiteChn;
	}

	// Compute the phase map
	computePhaseMap(I0, I1, I2, I3, height, phaseMap);

	// Count fringe peaks in the phase map
	peakCount = countFringePeaks(phaseMap, width, threshold);
	res.hPeak = peakCount.first;
	res.hPeakDis = peakCount.second;

	delete[] I0; delete[] I1; delete[] I2; delete[] I3; delete[] phaseMap;

	return res;
}

// PS Algorithm END ---------

void CMoSetup::FringeStripeAnalysis1() {
	IMGL::CIM ImA;
	CAM::ECAM eID = CAM::PRICAM;
	int HPeakCnt, VPeakCnt;
	int idx = 1;
	int  flagH = -1, flagV = -1;
	while (idx++ <= 3) {
		if (Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
			vector<double>HIntensityValues, VIntensityValues;
			CollectIntensity(ImA, HIntensityValues, 0); // Horizontal Pixel Intensity
			CollectIntensity(ImA, VIntensityValues, 1); // Vertical Pixel Intensity
			int HWindowSize = AT.GetSmoothingWindowSize(HIntensityValues);
			AT.SmoothData(3, HWindowSize, HIntensityValues);
			HPeakCnt = AT.getPeakCount(HIntensityValues, HWindowSize);
			int VWindowSize = AT.GetSmoothingWindowSize(VIntensityValues);
			AT.SmoothData(3, VWindowSize, VIntensityValues);
			VPeakCnt = AT.getPeakCount(VIntensityValues, VWindowSize);

			if (HPeakCnt < 2 && VPeakCnt < 2) break;

			if (HPeakCnt > 1 && VPeakCnt > 1) {
				if (HPeakCnt > VPeakCnt) {
					flagH = HorizontalFringeDirectionAdjustment();
					flagV = VerticalFringeDirectionAdjustment();
				}
				else {
					flagH = VerticalFringeDirectionAdjustment();
					flagV = HorizontalFringeDirectionAdjustment();
				}
			}
			else if (HPeakCnt > 1) flagH = HorizontalFringeDirectionAdjustment();
			else if (VPeakCnt > 1) flagV = VerticalFringeDirectionAdjustment();
		}
	}
}

bool CMoSetup::HorizontalFringeDirectionAdjustment() {
	// Horizontal Adjustment

	int range = 35;
	float step = 0.002; // micron
	bool flag1 = 0;
	int mn1 = 100000;
	int i, windowSize, nTime = 3;
	IMGL::CIM ImA;
	CAM::ECAM eID = CAM::PRICAM;

	for (i = 1; i <= range; i++) {
		if (Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
			vector<double>HIntensityValues;
			CollectIntensity(ImA, HIntensityValues, 0); // Horizontal Pixel Intensity
			windowSize = AT.GetSmoothingWindowSize(HIntensityValues);
			AT.SmoothData(nTime, windowSize, HIntensityValues);
			int HPeakCnt = AT.getPeakCount(HIntensityValues, windowSize);
			mn1 = min(mn1, HPeakCnt);
			if (HPeakCnt < 2) {
				flag1 = 1;
				break;
			}
			if (abs(mn1 - HPeakCnt) > 2) {
				TiltX(-step, 15000, bCheck);
				break;
			}
			TiltX(step, 15000, bCheck);
		}
		Yld(3);
	}
	if (flag1 == 0) {
		mn1 = 100000;
		for (i = 1; i <= range; i++) {
			if (Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
				vector<double>HIntensityValues;
				CollectIntensity(ImA, HIntensityValues, 0); // Horizontal Pixel Intensity
				windowSize = AT.GetSmoothingWindowSize(HIntensityValues);
				AT.SmoothData(nTime, windowSize, HIntensityValues);
				int HPeakCnt = AT.getPeakCount(HIntensityValues, windowSize);
				mn1 = min(mn1, HPeakCnt);
				if (HPeakCnt < 2) {
					break;
				}
				if (abs(mn1 - HPeakCnt) > 1) {
					TiltX(-step, 15000, bCheck);
					break;
				}
				TiltX(-step, 15000, bCheck);
			}
			Yld(3);
		}
	}

	return flag1;
}

bool CMoSetup::VerticalFringeDirectionAdjustment() {
	// Vertical Adjustment

	int range = 35;
	float step = 0.002; // micron
	bool flag2 = 0;
	int mn2 = 100000;
	int i, windowSize, nTime = 3;
	IMGL::CIM ImA;
	CAM::ECAM eID = CAM::PRICAM;

	for (i = 1; i <= range; i++) {
		if (Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
			vector<double>VIntensityValues;
			Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling);
			CollectIntensity(ImA, VIntensityValues, 1); // Vertical Pixel Intensity
			windowSize = AT.GetSmoothingWindowSize(VIntensityValues);
			AT.SmoothData(nTime, windowSize, VIntensityValues);
			int VPeakCnt = AT.getPeakCount(VIntensityValues, windowSize);
			mn2 = min(mn2, VPeakCnt);
			if (VPeakCnt < 2) {
				flag2 = 1;
				break;
			}
			if (abs(mn2 - VPeakCnt) > 2) {
				TiltY(-step, 15000, bCheck);
				break;
			}
			TiltY(step, 15000, bCheck);
		}
		Yld(3);
	}
	if (flag2 == 0) {
		mn2 = 100000;
		for (i = 1; i <= range; i++) {
			if (Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
				vector<double>VIntensityValues;
				Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling);
				CollectIntensity(ImA, VIntensityValues, 1); // Vertical Pixel Intensity
				windowSize = AT.GetSmoothingWindowSize(VIntensityValues);
				AT.SmoothData(nTime, windowSize, VIntensityValues);
				int VPeakCnt = AT.getPeakCount(VIntensityValues, windowSize);
				mn2 = min(mn2, VPeakCnt);
				if (VPeakCnt < 2) {
					break;
				}
				if (abs(mn2 - VPeakCnt) > 1) {
					TiltY(step, 15000, bCheck);
					break;
				}
				TiltY(-step, 15000, bCheck);
			}
			Yld(3);
		}
	}

	return flag2;
}

void CMoSetup::CollectIntensity(IMGL::CIM& ImR, std::vector<double>& IntensityValue, bool direction) {
	int ht, wd, bpp;
	ImR.GetDim(wd, ht, bpp);
	COLORREF colour1, colour2, colour3;
	if (direction) { // Vertical Pixel Intensity
		for (int ro = 0; ro < ht; ro++) {
			colour1 = ImR.GetPixel(wd / 4, ro);   // ImR.GetPixel(600, ro);
			colour2 = ImR.GetPixel(wd / 2, ro);          // ImR.GetPixel(900, ro);
			colour3 = ImR.GetPixel((wd / 2) + (wd / 4), ro); // 0
			double whiteChn1 = (0.2989 * (int)GetRValue(colour1)) + (0.5870 * (int)GetGValue(colour1)) + (0.1140 * (int)GetBValue(colour1));
			double whiteChn2 = (0.2989 * (int)GetRValue(colour2)) + (0.5870 * (int)GetGValue(colour2)) + (0.1140 * (int)GetBValue(colour2));
			double whiteChn3 = (0.2989 * (int)GetRValue(colour3)) + (0.5870 * (int)GetGValue(colour3)) + (0.1140 * (int)GetBValue(colour3));
			double AvgWhiteChn = (whiteChn1 + whiteChn2 + whiteChn3) / 3.0;
			IntensityValue.push_back(AvgWhiteChn);
		}
	}
	else { // Horizontal Pixel Intensity
		for (int col = 0; col < wd; col++) {
			colour1 = ImR.GetPixel(col, ht / 4);          // ImR.GetPixel(col, 800);
			colour2 = ImR.GetPixel(col, ht / 2);                 // ImR.GetPixel(col, 1600);
			colour3 = ImR.GetPixel(col, (ht / 2) + (ht / 4));      // 0
			double whiteChn1 = (0.2989 * (int)GetRValue(colour1)) + (0.5870 * (int)GetGValue(colour1)) + (0.1140 * (int)GetBValue(colour1));
			double whiteChn2 = (0.2989 * (int)GetRValue(colour2)) + (0.5870 * (int)GetGValue(colour2)) + (0.1140 * (int)GetBValue(colour2));
			double whiteChn3 = (0.2989 * (int)GetRValue(colour3)) + (0.5870 * (int)GetGValue(colour3)) + (0.1140 * (int)GetBValue(colour3));
			double AvgWhiteChn = (whiteChn1 + whiteChn2 + whiteChn3) / 3.0;
			IntensityValue.push_back(AvgWhiteChn);
		}
	}
}

// 11212023 - END

// 12052023 - START [ Shutter Control ]

void CMoSetup::OnBnClickedBtnShutter() {
	// TODO: Add your control notification handler code here
	ICC.bShutter = !ICC.bShutter;
	MO->stage.SHUTTER(ICC.bShutter);
}

// 12052023 - END

void CMoSetup::OnBnClickedAutoFocus() {
	// TODO: Add your control notification handler code here

	int StepSize, Range;
	CString temp, AFRange;
	m_StepSize.GetWindowText(temp);
	if (temp.IsEmpty()) {
		StepSize = 10;
	}
	else {
		StepSize = _wtoi(temp);
	}

	m_AFRange.GetWindowText(AFRange);
	if (AFRange.IsEmpty()) {
		Range = 300;
	}
	else {
		Range = _wtoi(AFRange);
	}

	isCamOpen = true;
	AutoFocus(Range, StepSize);
	if (isCamOpen) {
		getFringeCenter(30, 0.1);
	}
	AfxMessageBox(L"AF DONE");
}

void CMoSetup::OnBnClickedBtnAtCncl() {
	// TODO: Add your control notification handler code here
	isAtCancel = true;
}

void CMoSetup::AutoFocus(int Range, double StepSize, BOOL isGeometry) {
	IMGL::CIM ImA;
	CAM::ECAM eID = CAM::PRICAM;
	int tryCap = 1;
	while (tryCap++ <= 5 && !Dev.Cam.Grab(ImA, eID, Dev.Cam.pCm[eID]->subSampling)) {
	}
	ImA.Detach();
	if (tryCap == 6) {
		AfxMessageBox(L"Camera Not Running", MB_ICONINFORMATION);
		isCamOpen = false;
		return;
	}

	PiezoMoveAcc(50); // Piezo Home Pos
	UpdatePositions();

	//std::chrono::time_point<std::chrono::system_clock> start, end;
	//start = std::chrono::system_clock::now();
	MTH::SPoint3 N;
	float tiltZ, relativeFocusPosition = -1, x, y, z;
	if (AF.calibZPos != -1 && isGeometry) {
		tiltZ = GetTZPos(N);
		Dev.MC.get()->stage.devGetCurPos(MOT::MAXIS::Z, &z);
		relativeFocusPosition = (AF.calibZPos - z) + (AF.tiltZ - tiltZ);
		//relativeFocusPosition = (AF.calibZPos - z);
		MoveR(MAXIS::Z, relativeFocusPosition, 20000, true);
	}

	//TRACE("%f\n", relativeFocusPosition);
	float FocusPosition = AF.AutoFocus(-1, Range, StepSize, TRUE);	//direction, range, stepsize, multidirection
	MoveR(MAXIS::Z, FocusPosition, 20000, true);
	//FocusPosition = AF.CalibrateFocus();
	//MoveR(MAXIS::Z, FocusPosition, 20000, true);
	UpdatePositions();

	if (AF.calibZPos == -1 && isGeometry) {
		Dev.MC.get()->stage.devGetCurPos(MOT::MAXIS::Z, &z);
		AF.calibZPos = z;
		tiltZ = GetTZPos(N);
		AF.tiltZ = tiltZ;
	}

	//end = std::chrono::system_clock::now();
	//std::chrono::duration<double>elapsed_second = end - start;
	//TRACE("%lf\n", elapsed_second.count());
}

void CMoSetup::OnBnClickedCalibReset() {
	// TODO: Add your control notification handler code here

	ATWithGeo();
}

void CMoSetup::LiftZMot() {
	MoveR(MAXIS::Z, -5, 20000, bCheck); // Z-MOT 5mm Up
}

void CMoSetup::CalibReset() {
	Dev.MC.get()->stage.ResetZ();
	Dev.MC.get()->tilt.Reset();
	Dev.MC.get()->tilt.Home(true);
	Dev.MC.get()->stage.GotoXY(0, 0, 10000, true);
	UpdatePositions();
}

void CMoSetup::ATWithGeo() {
	double tiltX = (PiDisX / pixDisX) * 279.4;
	double tiltY = (PiDisY / pixDisY) * 279.4;
	//double tiltX = 0.00;
	//double tiltY = 0.00;

	/*if (fringDirX == FRNGDIR::RTL) {
		tiltX *= -1;
	}
	else {
	}
	if (fringDirY == FRNGDIR::DTT) {
		tiltY *= -1;
	}
	else {
	}*/

	LiftZMot();
	MoveR(MAXIS::T, tiltX, 15000, TRUE);
	MoveR(MAXIS::V, tiltY, 15000, TRUE);
}