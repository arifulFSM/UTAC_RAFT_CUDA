#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "MOTOR/MotorCtrl.h"
#include "MotPropDlg.h"

IMPLEMENT_DYNAMIC(CMotPropDlg, CResizableDialog)

CMotPropDlg::CMotPropDlg(MOT::CMotorCtrl* mc, CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DIALOG7, pParent), MC(mc) {}

CMotPropDlg::~CMotPropDlg() {}

void CMotPropDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, cGrid);
}

BEGIN_MESSAGE_MAP(CMotPropDlg, CResizableDialog)
	ON_BN_CLICKED(IDC_MOT_PROP_RESTORE, &CMotPropDlg::OnBnClickedMotPropRestore)
	ON_MESSAGE(WM_NOTIFY_DESCRIPTION_EDITED, &CMotPropDlg::OnNotifyDescriptionEdited)
	ON_MESSAGE(WM_NOTIFY_KILL_FOCUS, &CMotPropDlg::OnNotifyKillFocus)
	ON_BN_CLICKED(IDOK, &CMotPropDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CMotPropDlg::OnInitDialog() {
	AddAnchor(IDC_LIST1, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT);
	AddAnchor(IDC_MOT_PROP_RESTORE, BOTTOM_RIGHT);

	CResizableDialog::OnInitDialog();

	if (MC) {
		for (int i = 0; i < 6; i++) Mlocal[i] = MC->stage.Mpar[i];
	}
	else ASSERT(0);

	cGrid.Initialize(IDD_DIALOG7);
	if (InitGrid()) LoadGrid();

	ArrangeLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool CMotPropDlg::InitGrid() {
	CRect rc;  cGrid.GetClientRect(&rc);
	int wd = (rc.Width() - 65) / 4;
	int n = 0;
	cGrid.InsertHeader(n++, _T("Properties"), 100, FALSE);
	cGrid.InsertHeader(n++, _T("X Motor"), wd, TRUE);
	cGrid.InsertHeader(n++, _T("Y Motor"), wd, TRUE);
	cGrid.InsertHeader(n++, _T("Z Motor"), wd, TRUE);
	cGrid.InsertHeader(n++, _T("T Motors"), wd, TRUE);
	return true;
}

bool CMotPropDlg::LoadGrid() {
	cGrid.DeleteAllItems();

	for (int i = 0; i < 8; i++) {
		CString ch;
		switch (i) {
		case 0: ch = _T("AC"); break;
		case 1: ch = _T("VE"); break;
		case 2: ch = _T("CC"); break;
		case 3: ch = _T("SPMM"); break;
		case 4: ch = _T("Polarity"); break;
		case 5: ch = _T("Min"); break;
		case 6: ch = _T("Max"); break;
		case 7: ch = _T("Home"); break;
		}
		int l_iItem = cGrid.InsertRow(i, ch);
		int n1 = 1;
		switch (i) {
		case 0:	//AC
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].acc, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].acc, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].acc, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].acc, 3));
			break;
		case 1:	//VE
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].vel, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].vel, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].vel, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].vel, 3));
			break;
		case 2:	//CC
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].current, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].current, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].current, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].current, 3));
			break;
		case 3:	//SPMM
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].SPMM, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].SPMM, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].SPMM, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].SPMM, 3));
			break;
		case 4:	//Polarity
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].polarity));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].polarity));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].polarity));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].polarity));
			break;
		case 5:	//Min
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].min, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].min, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].min, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].min, 3));
			break;
		case 6:	//Max
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].max, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].max, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].max, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].max, 3));
			break;
		case 7:	//Home
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::X)].home, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Y)].home, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::Z)].home, 3));
			cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Mlocal[int(MOT::MAXIS::T)].home, 3));
			break;
		default:
			break;
		}
	}
	return true;
}

void CMotPropDlg::OnBnClickedMotPropRestore() {
	LoadGrid();
}

void CMotPropDlg::OnOK() {
	if (AfxMessageBox(_T("Click OK will overwrite values in INI file\nDo you want to continue?"), MB_OKCANCEL) == IDOK) {
		for (int i = int(MOT::MAXIS::X); i <= int(MOT::MAXIS::Z); i++) {
			MC->stage.Mpar[i] = Mlocal[i];
		}
		for (int i = int(MOT::MAXIS::T); i <= int(MOT::MAXIS::V); i++) {
			MC->stage.Mpar[i] = Mlocal[int(MOT::MAXIS::T)];
		}
		//TODO:Reprogram motors here
		for (int i = int(MOT::MAXIS::X); i <= int(MOT::MAXIS::Z); i++) {
			MOT::CMotor::SaveParam(MOT::MAXIS(i), MOT::CMotor::section[i].c_str());
			MC->stage.SetCurrent(MOT::MAXIS(i), Mlocal[i].current);
			MC->stage.SetSpeed(MOT::MAXIS(i), Mlocal[i].vel);
			MC->stage.SetAcceleration(MOT::MAXIS(i), Mlocal[i].acc);
		}
		for (int i = int(MOT::MAXIS::T); i <= int(MOT::MAXIS::V); i++) {
			MOT::CMotor::SaveParam(MOT::MAXIS(i), MOT::CMotor::section[i].c_str());
			MC->tilt.SetCurrent(MOT::MAXIS(i), Mlocal[int(MOT::MAXIS::T)].current);
			MC->tilt.SetSpeed(MOT::MAXIS(i), Mlocal[int(MOT::MAXIS::T)].vel);
			MC->tilt.SetAcceleration(MOT::MAXIS(i), Mlocal[int(MOT::MAXIS::T)].acc);
		}
		CResizableDialog::OnOK();
	}
}

afx_msg LRESULT CMotPropDlg::OnNotifyDescriptionEdited(WPARAM wParam, LPARAM lParam) {
	// Get the changed Description field text via the callback
	LV_DISPINFO* dispinfo = reinterpret_cast<LV_DISPINFO*>(lParam);

	iItem = dispinfo->item.iItem;
	iSubItem = dispinfo->item.iSubItem;
	text = dispinfo->item.pszText;

	MOT::SMotPar& M = Mlocal[iSubItem - 1];
	switch (iItem) {
	case 0:
		M.acc = float(_wtof(text));
		text.Format(_T("%.3f"), M.acc);
		break; // float
	case 1: M.vel = float(_wtof(text));
		text.Format(_T("%.3f"), M.vel);
		break;// float
	case 2: M.current = float(_wtof(text));
		text.Format(_T("%.3f"), M.current);
		break;// float
	case 3: M.SPMM = float(_wtof(text));
		text.Format(_T("%.3f"), M.SPMM);
		break;// float
	case 4: M.polarity = float(_wtoi(text));
		text.Format(_T("%d"), M.polarity);
		break;// int
	case 5: M.min = float(_wtof(text));
		text.Format(_T("%.3f"), M.min);
		break;// float
	case 6: M.max = float(_wtof(text));
		text.Format(_T("%.3f"), M.max);
		break;// float
	case 7: M.home = float(_wtof(text));
		text.Format(_T("%.3f"), M.home);
		break;// float
	}
	// Persist the selected attachment details upon updating its text
	//cGrid.SetItemText(dispinfo->item.iItem, dispinfo->item.iSubItem, dispinfo->item.pszText);
	return 0;
}

afx_msg LRESULT CMotPropDlg::OnNotifyKillFocus(WPARAM wParam, LPARAM lParam) {
	cGrid.SetItemText(iItem, iSubItem, text);
	return 0;
}

void CMotPropDlg::OnBnClickedOk() {
	// TODO: Add your control notification handler code here
}