#include "pch.h"
#include <thread>
#include "afxdialogex.h"
#include "WLI.h"

#include "Dev.h"
#include "MoCtrl.h"
#include "MoSetup.h"
#include "WLIView.h"

using namespace MOT;

IMPLEMENT_DYNAMIC(CMoCtrl, CDialogEx)

CMoCtrl::CMoCtrl(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent) {}

CMoCtrl::~CMoCtrl() {}

void CMoCtrl::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, pX);
	DDX_Text(pDX, IDC_EDIT3, pY);
}

BEGIN_MESSAGE_MAP(CMoCtrl, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CMoCtrl::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMoCtrl::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMoCtrl::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMoCtrl::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON7, &CMoCtrl::OnBnClickedButton7)
	ON_EN_KILLFOCUS(IDC_EDIT1, &CMoCtrl::OnEnKillfocusEdit1)
	ON_EN_KILLFOCUS(IDC_EDIT3, &CMoCtrl::OnEnKillfocusEdit3)
END_MESSAGE_MAP()

std::wstring CMoCtrl::GetPos_um(MOT::MAXIS nAxis) {
	CString str;
	float fPos;
	if (MOT::CMotor::Mpar[int(nAxis)].bDirty) {
		switch (nAxis) {
		case MAXIS::X:
		case MAXIS::Y:
		case MAXIS::Z:
			Dev.MC.get()->stage.devGetCurPos(nAxis, &fPos);
			break;
		case MAXIS::T:
		case MAXIS::U:
		case MAXIS::V:
			Dev.MC.get()->tilt.devGetCurPos(nAxis, &fPos);
			break;
		default: ASSERT(0); return _T("?"); break;
		}
	}
	str.Format(_T("%.4f"), MOT::CMotor::Mpar[int(nAxis)].now);
	return str.GetBuffer();
}

void CMoCtrl::UpdateMotorPosition() {
	std::wstring msg;
	msg =
		_T("x:") + GetPos_um(MAXIS::X) +
		_T("y:") + GetPos_um(MAXIS::Y) +
		_T("z:") + GetPos_um(MAXIS::Z) +
		_T("t:") + GetPos_um(MAXIS::T) +
		_T("u:") + GetPos_um(MAXIS::U) +
		_T("v:") + GetPos_um(MAXIS::V);
	GetDlgItem(IDC_MESSAGE)->SetWindowTextW(msg.c_str());
}

void CMoCtrl::OnBnClickedButton1() {  // Reset
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	MOT::CTilt& Tilt = Dev.MC->tilt;
	MOT::CStage& Stage = Dev.MC->stage;

	Stage.Reset(); Tilt.Reset();
	Stage.Home(true); Tilt.Home(true);
	UpdateMotorPosition();
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
}

void CMoCtrl::OnBnClickedButton2() {
	// move tilt stage up than down than mid position

	// tilt stage left than right than up than down and finally level
	UpdateMotorPosition();
}

void CMoCtrl::OnBnClickedButton3() {
	// move xy stage to 5 different random point while maintaining tilt and z position
	MOT::CStage& Stage = Dev.MC->stage;
	Stage.GotoXY(pX, pY, 60000, true);
	UpdateMotorPosition();
}

void CMoCtrl::OnBnClickedButton4() {
	// fAST UI update, may not be accurate
	UpdateMotorPosition();
}

void CMoCtrl::OnBnClickedButton7() {
	if (!CWLIView::pMSet) {
		CWLIView::pMSet = new CMotionControlDlg;
		if (CWLIView::pMSet) CWLIView::pMSet->Create(IDD_DIALOG10);
	}
	if (CWLIView::pMSet) {
		if (CWLIView::pMSet->IsWindowVisible()) return;
		CWLIView::pMSet->ShowWindow(SW_SHOW);
	}
}

void CMoCtrl::OnEnKillfocusEdit1() {
	UpdateData(TRUE);
}

void CMoCtrl::OnEnKillfocusEdit3() {
	UpdateData(TRUE);
}