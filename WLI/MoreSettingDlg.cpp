#include "pch.h"
#include "WLI.h"
#include "PSI/Strip.h"
#include "afxdialogex.h"
#include "MoreSettingDlg.h"

IMPLEMENT_DYNAMIC(CMoreSettingDlg, CDialogEx)

CMoreSettingDlg::CMoreSettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG14, pParent) {}

CMoreSettingDlg::~CMoreSettingDlg() {}

void CMoreSettingDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMoreSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO1, &CMoreSettingDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO10, &CMoreSettingDlg::OnBnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO11, &CMoreSettingDlg::OnBnClickedRadio11)
	ON_BN_CLICKED(IDC_RADIO12, &CMoreSettingDlg::OnBnClickedRadio12)
END_MESSAGE_MAP()

void CMoreSettingDlg::OnBnClickedRadio1() {
	Strip.WhtCalc = WLI::ANALY;
}

void CMoreSettingDlg::OnBnClickedRadio10() {
	Strip.WhtCalc = WLI::PCPTN;
}

void CMoreSettingDlg::OnBnClickedRadio11() {
	Strip.WhtCalc = WLI::R121;
}

void CMoreSettingDlg::OnBnClickedRadio12() {
	Strip.WhtCalc = WLI::R131;
}

BOOL CMoreSettingDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	switch (Strip.WhtCalc) {
	case WLI::PCPTN:
		((CButton*)GetDlgItem(IDC_RADIO10))->SetCheck(TRUE);
		break;
	case WLI::R121:
		((CButton*)GetDlgItem(IDC_RADIO11))->SetCheck(TRUE);
		break;
	case WLI::R131:
		((CButton*)GetDlgItem(IDC_RADIO12))->SetCheck(TRUE);
		break;
	default:
		((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}