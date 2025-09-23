// TiltControlDlg.cpp : implementation file
//

#include "pch.h"

#include <thread>

#include "WLI.h"
#include "Dev.h"
#include "MoSetup.h"
#include "MOTOR/MotorCtrl.h"
#include "afxdialogex.h"
#include "TiltControlDlg.h"

// CTiltControlDlg dialog

IMPLEMENT_DYNAMIC(CTiltControlDlg, CDialogEx)

CTiltControlDlg::CTiltControlDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG9, pParent) {}

CTiltControlDlg::~CTiltControlDlg() {}

void CTiltControlDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTiltControlDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON20, &CTiltControlDlg::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_CHECK1, &CTiltControlDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON19, &CTiltControlDlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON48, &CTiltControlDlg::OnBnClickedButton48)
	ON_BN_CLICKED(IDC_BUTTON51, &CTiltControlDlg::OnBnClickedButton51)
	ON_BN_CLICKED(IDC_BUTTON15, &CTiltControlDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON16, &CTiltControlDlg::OnBnClickedButton16)
	ON_BN_CLICKED(IDC_BUTTON13, &CTiltControlDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CTiltControlDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON49, &CTiltControlDlg::OnBnClickedButton49)
	ON_BN_CLICKED(IDC_BUTTON52, &CTiltControlDlg::OnBnClickedButton52)
	ON_BN_CLICKED(IDC_BUTTON17, &CTiltControlDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CTiltControlDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON33, &CTiltControlDlg::OnBnClickedButton33)
	ON_BN_CLICKED(IDC_BUTTON34, &CTiltControlDlg::OnBnClickedButton34)
	ON_BN_CLICKED(IDC_BUTTON50, &CTiltControlDlg::OnBnClickedButton50)
	ON_BN_CLICKED(IDC_BUTTON53, &CTiltControlDlg::OnBnClickedButton53)
	ON_BN_CLICKED(IDC_BUTTON35, &CTiltControlDlg::OnBnClickedButton35)
	ON_BN_CLICKED(IDC_BUTTON36, &CTiltControlDlg::OnBnClickedButton36)
	ON_BN_CLICKED(IDC_BUTTON40, &CTiltControlDlg::OnBnClickedButton40)
	ON_BN_CLICKED(IDC_BUTTON41, &CTiltControlDlg::OnBnClickedButton41)
	ON_BN_CLICKED(IDC_BUTTON30, &CTiltControlDlg::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON29, &CTiltControlDlg::OnBnClickedButton29)
	ON_BN_CLICKED(IDC_BUTTON31, &CTiltControlDlg::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CTiltControlDlg::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON64, &CTiltControlDlg::OnBnClickedButton64)
	ON_BN_CLICKED(IDC_BUTTON62, &CTiltControlDlg::OnBnClickedButton62)
	ON_BN_CLICKED(IDC_BUTTON63, &CTiltControlDlg::OnBnClickedButton63)
	ON_BN_CLICKED(IDC_BUTTON65, &CTiltControlDlg::OnBnClickedButton65)
END_MESSAGE_MAP()

void CTiltControlDlg::UpdatePositions() {
	CString str;
	MOT::SMotPar* M = &MO->tilt.Mpar[0];

	str.Format(_T("T:%.3f, U:%.3f, V:%.3f au"),
		M[int(MOT::MAXIS::T)].now,
		M[int(MOT::MAXIS::U)].now,
		M[int(MOT::MAXIS::V)].now);
	GetDlgItem(IDC_EDIT8)->SetWindowTextW(str);
}

// CTiltControlDlg message handlers
BOOL CTiltControlDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	MO = Dev.MC.get();

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(bCheck);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTiltControlDlg::MoveR(MOT::MAXIS nAx, float inc, DWORD tout, bool bCheck) {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->MoveR(nAx, inc, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::TiltUpDown(float inc, DWORD tout, bool bCheck) {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->tilt.UpDownTUV(inc, tout, bCheck);
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedCheck1() {
	bCheck = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
}

void CTiltControlDlg::OnBnClickedButton20() {
	MoveR(MOT::MAXIS::T, incTUV, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton19() {
	MoveR(MOT::MAXIS::T, incTUV / 10.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton48() {
	MoveR(MOT::MAXIS::T, incTUV / 100.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton51() {
	MoveR(MOT::MAXIS::T, -incTUV / 100.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton15() {
	MoveR(MOT::MAXIS::T, -incTUV / 10.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton16() {
	MoveR(MOT::MAXIS::T, -incTUV, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton13() {
	MoveR(MOT::MAXIS::U, incTUV, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton14() {
	MoveR(MOT::MAXIS::U, incTUV / 10.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton49() {
	MoveR(MOT::MAXIS::U, incTUV / 100.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton52() {
	MoveR(MOT::MAXIS::U, -incTUV / 100.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton17() {
	MoveR(MOT::MAXIS::U, -incTUV / 10.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton18() {
	MoveR(MOT::MAXIS::U, -incTUV, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton33() {
	MoveR(MOT::MAXIS::V, incTUV, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton34() {
	MoveR(MOT::MAXIS::V, incTUV / 10.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton50() {
	MoveR(MOT::MAXIS::V, incTUV / 100.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton53() {
	MoveR(MOT::MAXIS::V, -incTUV / 100.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton35() {
	MoveR(MOT::MAXIS::V, -incTUV / 10.f, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton36() {
	MoveR(MOT::MAXIS::V, -incTUV, 20000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton40() {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->tilt.ResetXY0();
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedButton41() {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	MO->tilt.SetZeroPos();
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedButton30() {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	// tilt reset
	MO->tilt.Reset();
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedButton29() {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	// tilt home
	MO->tilt.Home(bCheck);
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedButton31() {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	// tilt max
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	std::thread A(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::T, M[int(MOT::MAXIS::T)].max, 10000, true);
	std::thread B(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::U, M[int(MOT::MAXIS::U)].max, 10000, true);
	std::thread C(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::V, M[int(MOT::MAXIS::V)].max, 10000, true);
	A.join(); B.join(); C.join();
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedButton32() {
	CWaitCursor wc;
	if (bBlock) return; bBlock = true;
	// tilt min
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	std::thread A(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::T, M[int(MOT::MAXIS::T)].min, 10000, true);
	std::thread B(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::U, M[int(MOT::MAXIS::U)].min, 10000, true);
	std::thread C(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::V, M[int(MOT::MAXIS::V)].min, 10000, true);
	A.join(); B.join(); C.join();
	UpdatePositions();
	bBlock = false;
}

void CTiltControlDlg::OnBnClickedButton64() {
	if (CMoSetup::bFineTilt) TiltUpDown(incUD / 5000.f, 25000, bCheck);
	else TiltUpDown(incUD, 25000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton62() {
	if (CMoSetup::bFineTilt) TiltUpDown(incUD / 50000.f, 25000, bCheck);
	else TiltUpDown(incUD / 100.f, 25000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton63() {
	if (CMoSetup::bFineTilt) TiltUpDown(-incUD / 50000.f, 25000, bCheck);
	else TiltUpDown(-incUD / 100.f, 25000, bCheck);
}

void CTiltControlDlg::OnBnClickedButton65() {
	if (CMoSetup::bFineTilt) TiltUpDown(-incUD / 5000.f, 25000, bCheck);
	else TiltUpDown(-incUD, 25000, bCheck);
}