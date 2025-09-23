// ResultDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "ResultDlg.h"
#include "RAFTApp.h"
#include "wdefine.h"

// ResultDlg dialog

IMPLEMENT_DYNAMIC(ResultDlg, CResizableDialog)

ResultDlg::ResultDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DLG_RESULT, pParent) {}

ResultDlg::~ResultDlg() {}

void ResultDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_Result);
}

BEGIN_MESSAGE_MAP(ResultDlg, CResizableDialog)
	ON_MESSAGE(WM_XTAB_SELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTAB_DESELECTED, OnTabDeselected)
	ON_BN_CLICKED(IDC_IMPORT, &ResultDlg::OnBnClickedImportResult)
	ON_BN_CLICKED(IDC_EXPORT, &ResultDlg::OnBnClickedExportResult)
	ON_MESSAGE(UM_RESULT_DLG, &ResultDlg::OnUmResultDlg)
END_MESSAGE_MAP()

// ResultDlg message handlers
BOOL ResultDlg::OnInitDialog() {
	__super::OnInitDialog();

	pResult = &pRAFTApp->Result;

	const wchar_t* dwCjName[] =
	{
		L"#", L"Position", L"Sq[um]", L"Sa[um]", L"Sy[um]"
	};
	int nSize[] = { 32, 160, 100, 100, 100 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = (LPWSTR)dwCjName[i];
		m_Result.InsertColumn(i, &nListColumn);
	}
	m_Result.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	return TRUE;
}

void ResultDlg::OnBnClickedImportResult() {
	// TODO: Add your control notification handler code here
	pResult->LoadResult();
	pResult->UpdateControl(m_Result);
}

void ResultDlg::OnBnClickedExportResult() {
	// TODO: Add your control notification handler code here
	pResult->SaveResult();
}

LRESULT ResultDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	if (!bTabSelected) {
		pResult = &pRAFTApp->Result;
		pResult->UpdateControl(m_Result);
		bTabSelected = TRUE;
	}
	return 0;
}

LRESULT ResultDlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		bTabSelected = FALSE;
	}
	return 0;
}

afx_msg LRESULT ResultDlg::OnUmResultDlg(WPARAM wParam, LPARAM lParam) {
	if (!bTabSelected) {
		pResult = &pRAFTApp->Result;
		pResult->UpdateControl(m_Result);
		bTabSelected = TRUE;
	}
	return true;
}