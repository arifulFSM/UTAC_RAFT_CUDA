// MeasProgressDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "MeasProgressDlg.h"
#include "PSI/ICC.h"

// MeasProgressDlg dialog

IMPLEMENT_DYNAMIC(MeasProgressDlg, CDialogEx)

MeasProgressDlg::MeasProgressDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_RCP_MEAS_PBAR, pParent) {}

MeasProgressDlg::~MeasProgressDlg() {
	//progress = nullptr;
}

void MeasProgressDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_PROGRESS_MEAS, m_ProgressBar);
}

BEGIN_MESSAGE_MAP(MeasProgressDlg, CDialogEx)
END_MESSAGE_MAP()

// MeasProgressDlg message handlers
BOOL MeasProgressDlg::OnInitDialog() {
	__super::OnInitDialog();

	return TRUE;
}

void MeasProgressDlg::SetUpProgressBarProp(int sz) {
	//m_ProgressBar.SetRange(0, sz);
}

void MeasProgressDlg::SetPosProBar(int idx, int sz, CString Message) {
	//m_ProgressBar.SetPos(idx);
	ShowMessage(Message);
}

void MeasProgressDlg::ShowMessage(CString str) {
	GetDlgItem(IDC_STC_SHOW_PROGRESS)->SetWindowTextW(str);
}