#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "CamSetDlg.h"

IMPLEMENT_DYNAMIC(CCamSetDlg, CDialogEx)

CCamSetDlg::CCamSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG6, pParent) {}

CCamSetDlg::~CCamSetDlg() {}

void CCamSetDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, cListBox);
}

BEGIN_MESSAGE_MAP(CCamSetDlg, CDialogEx)
END_MESSAGE_MAP()

// CCamSetting message handlers

BOOL CCamSetDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}