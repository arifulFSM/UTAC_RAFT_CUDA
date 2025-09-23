#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"

#include "Recipe.h"

#include "LevelSetupDlg.h"

IMPLEMENT_DYNAMIC(CLevelSetupDlg, CDialogEx)

CLevelSetupDlg::CLevelSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG13, pParent) {}

CLevelSetupDlg::~CLevelSetupDlg() {}

void CLevelSetupDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, cGrid);
}

BEGIN_MESSAGE_MAP(CLevelSetupDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLevelSetupDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CLevelSetupDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	for (int i = 0; i < MAXSPOTCTX; i++) {
		Ctx[i] = Rcp.SCtx[i];
	}

	cGrid.Initialize(IDD_DIALOG7);
	if (InitGrid()) LoadGrid();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

bool CLevelSetupDlg::InitGrid() {
	CRect rc;  cGrid.GetClientRect(&rc);
	int pwd = 250;
	int wd = (rc.Width() - pwd) / 4;
	int n = 0;
	cGrid.InsertHeader(n++, _T("Props"), pwd, FALSE);
	cGrid.InsertHeader(n++, _T("Blob 1"), wd, TRUE);
	cGrid.InsertHeader(n++, _T("Blob 2"), wd, TRUE);
	cGrid.InsertHeader(n++, _T("Strip 1"), wd, TRUE);
	cGrid.InsertHeader(n++, _T("Strip 2"), wd, TRUE);
	return true;
}

bool CLevelSetupDlg::LoadGrid() {
	cGrid.DeleteAllItems();

	int n0 = 0;
	int l_iItem = cGrid.InsertRow(n0, SSpotCtx::Prop[n0]);
	int n1 = 1;
	for (int j = 0; j < MAXSPOTCTX; j++) {
		cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Ctx[j].Obj));
	}
	n0++; l_iItem = cGrid.InsertRow(n0, SSpotCtx::Prop[n0]);
	n1 = 1;
	for (int j = 0; j < MAXSPOTCTX; j++) {
		cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Ctx[j].Thres));
	}
	n0++; l_iItem = cGrid.InsertRow(n0, SSpotCtx::Prop[n0]);
	n1 = 1;
	for (int j = 0; j < MAXSPOTCTX; j++) {
		cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Ctx[j].lmt));
	}
	n0++; l_iItem = cGrid.InsertRow(n0, SSpotCtx::Prop[n0]);
	n1 = 1;
	for (int j = 0; j < MAXSPOTCTX; j++) {
		cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Ctx[j].clmt));
	}
	n0++; l_iItem = cGrid.InsertRow(n0, SSpotCtx::Prop[n0]);
	n1 = 1;
	for (int j = 0; j < MAXSPOTCTX; j++) {
		cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Ctx[j].Wdw));
	}
	n0++; l_iItem = cGrid.InsertRow(n0, SSpotCtx::Prop[n0]);
	n1 = 1;
	for (int j = 0; j < MAXSPOTCTX; j++) {
		cGrid.SetItemText(l_iItem, n1++, cGrid.VToS(Ctx[j].Sz));
	}
	return true;
}

void CLevelSetupDlg::OnBnClickedOk() {
	for (int i = 0; i < MAXSPOTCTX; i++) {
		Rcp.SCtx[i] = Ctx[i];
	}
	CDialogEx::OnOK();
}