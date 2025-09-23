// RecipeDlg.cpp : implementation file
//

// 07252023

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "RecipeDlg.h"
#include <iostream>
#include "SRC/DOSUtil.h"
#include "PSI/Strip.h"
#include "PSI/ICC.h"
#include "RAFTApp.h"
#include "WLIView.h"
#include "SRC/XTabDefine.h"

// RecipeDlg dialog
RecipeDlg* recipeItems = nullptr;

IMPLEMENT_DYNAMIC(RecipeDlg, CResizableDialog)

RecipeDlg::RecipeDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_RCP_DLG, pParent) {
	recipeItems = this;
}

RecipeDlg::~RecipeDlg() {
	recipeItems = nullptr;
	if (measDlg) delete measDlg;
}

BOOL RecipeDlg::OnInitDialog() {
	__super::OnInitDialog();

	MO = Dev.MC.get();

	pRcp = &pRAFTApp->RcpSetup;
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;
	m_cWaferMap.Redraw();

	const wchar_t* dwCjName[] =
	{
		L"#", L"Position"
	};
	int nSize[] = { 32, 200 };
	LV_COLUMN nListColumn;
	for (int i = 0; i < sizeof(nSize) / sizeof(int); i++) {
		nListColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		nListColumn.fmt = LVCFMT_LEFT;
		nListColumn.cx = nSize[i];
		nListColumn.iSubItem = 0;
		nListColumn.pszText = (LPWSTR)dwCjName[i];
		m_cPoint.InsertColumn(i, &nListColumn);
	}
	m_cPoint.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	pWLIView->ShowMoSetup(0);
	camRun();

	return TRUE;
}

void RecipeDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_X1, m_X1);
	//DDX_Control(pDX, IDC_Y1, m_Y1);
	//DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_LIST1, m_cPoint);
	DDX_Control(pDX, IDC_AF_CAL_RANGE, m_AFRange);
	DDX_Control(pDX, IDC_AF_CAL_STEP_SIZE, m_AFStepSize);
	DDX_Control(pDX, IDC_ME_RANGE, m_MERange);
	DDX_Control(pDX, IDC_ME_FRAMES, m_MEFrames);
	DDX_Control(pDX, IDC_AF_CALZ, m_AFCalZ);
	DDX_Control(pDX, IDC_CAMERA, cLiveVid);
}

BEGIN_MESSAGE_MAP(RecipeDlg, CResizableDialog)
	//ON_BN_CLICKED(IDC_BTN_PNT_SAVE, &RecipeDlg::OnBnClickedBtnPntSave)
	//ON_BN_CLICKED(IDC_BTN_LOAD_RCP, &RecipeDlg::OnBnClickedBtnLoadRcp)
	//ON_BN_CLICKED(IDC_BTN_MEASURE, &RecipeDlg::OnBnClickedBtnMeasure)
	//ON_BN_CLICKED(IDC_BTN_ADD_POINTS, &RecipeDlg::OnBnClickedBtnAddPoints)
	//ON_BN_CLICKED(IDC_BTN_NEW_RCP, &RecipeDlg::OnBnClickedBtnNewRcp)
	ON_MESSAGE(IDC_ADDPOINT, OnAddPoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_MESSAGE(IDC_DELETEPOINT, OnDeletepoint)
	ON_MESSAGE(WM_XTAB_SELECTED, OnTabSelected)
	ON_MESSAGE(WM_XTAB_DESELECTED, OnTabDeselected)
	ON_BN_CLICKED(IDC_SAVE_RECIPE, &RecipeDlg::OnBnClickedSaveRecipe)
	ON_BN_CLICKED(IDC_LOAD_RECIPE, &RecipeDlg::OnBnClickedLoadRecipe)
	ON_BN_CLICKED(IDC_NEW_RECIPE, &RecipeDlg::OnBnClickedNewRecipe)
	ON_BN_CLICKED(IDC_BTN_CALIB_POS, &RecipeDlg::OnBnClickedBtnCalibPos)
	ON_BN_CLICKED(IDC_BTN_CALIB_POS, &RecipeDlg::OnBnClickedBtnCalibPos)
	ON_BN_CLICKED(IDC_MOT_SETUP, &RecipeDlg::OnBnClickedMotSetup)
	ON_BN_CLICKED(IDC_CAM_PROP, &RecipeDlg::OnBnClickedCamProp)
END_MESSAGE_MAP()

// RecipeDlg message handlers

void RecipeDlg::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(m_cPoint);
	m_cWaferMap.Redraw();
}

LRESULT RecipeDlg::OnAddPoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

LRESULT RecipeDlg::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	Renumber();
	return 0;
}

LRESULT RecipeDlg::OnDeletepoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(m_cPoint);
	return 0;
}

void RecipeDlg::OnBnClickedSaveRecipe() {
	// TODO: Add your control notification handler code here
	UpdateValue();
	if (pRcp) {
		if (pRAFTApp->ValidCheckRcp(*pRcp)) {
			pRcp->SaveRecipe();
		}
		//CDialog::OnOK();
	}
	//CDialog::OnCancel();
}

void RecipeDlg::OnBnClickedLoadRecipe() {
	// TODO: Add your control notification handler code here
	if (pRcp) {
		if (pRcp->LoadRecipe()) {
			RecipeToLocal();
		}

		//CDialog::OnOK();
	}
	//CDialog::OnCancel();
}

void RecipeDlg::RecipeToLocal() {
	pRcp = &pRAFTApp->RcpSetup;
	pRcp->UpdateControl(m_cPoint);
	SetValue();
	m_cWaferMap.Redraw();
}

void RecipeDlg::OnBnClickedNewRecipe() {
	// TODO: Add your control notification handler code here
	ReSetValue();
	if (pRcp) {
		pRcp->Clear();
		RecipeToLocal();
	}
}

void RecipeDlg::OnBnClickedBtnCalibPos() {
	// TODO: Add your control notification handler code here
	//Dev.MC.get()->stage.ResetZ();
	//Dev.MC.get()->tilt.Reset();
	//Dev.MC.get()->tilt.Home(true);
	//Dev.MC.get()->stage.GotoXY(0, 0, 10000, true);
	pRcp->AFTiltZ = pWLIView->pMSet->GetTiltZ();
	pWLIView->pMSet->UpdatePos();
	AfxMessageBox(L"Find Fringe Manually For The Calibration", MB_ICONINFORMATION);
}

void RecipeDlg::ReSetValue() {
	m_AFCalZ.SetWindowText(L"");
	m_AFRange.SetWindowText(L"");
	m_AFStepSize.SetWindowText(L"");
	m_MERange.SetWindowText(L"");
	m_MEFrames.SetWindowText(L"");
}

void RecipeDlg::UpdateValue() {
	CString temp;
	m_AFCalZ.GetWindowText(temp);
	pRcp->AFCalibZ = _wtof(temp);
	m_AFRange.GetWindowText(temp);
	pRcp->AFRange = _wtof(temp);
	m_AFStepSize.GetWindowText(temp);
	pRcp->AFStepSize = _wtof(temp);
	m_MERange.GetWindowText(temp);
	pRcp->MERange = _wtof(temp);
	m_MEFrames.GetWindowText(temp);
	pRcp->MEFrames = _wtof(temp);
}

void RecipeDlg::SetValue() {
	CString temp;
	temp.Format(L"%.02f", pRcp->AFCalibZ);
	m_AFCalZ.SetWindowText(temp);
	temp.Format(L"%.02f", pRcp->AFRange);
	m_AFRange.SetWindowText(temp);
	temp.Format(L"%.02f", pRcp->AFStepSize);
	m_AFStepSize.SetWindowText(temp);
	temp.Format(L"%.02f", pRcp->MERange);
	m_MERange.SetWindowText(temp);
	temp.Format(L"%.02f", pRcp->MEFrames);
	m_MEFrames.SetWindowText(temp);
}

LRESULT RecipeDlg::OnTabSelected(WPARAM wP, LPARAM lP) {
	if (!bTabSelected) {
		RecipeToLocal();
		m_cWaferMap.pRcp = pRcp;
		pRcp->UpdateControl(m_cPoint);
		bTabSelected = TRUE;
	}

	camRun();

	return 0;
}

LRESULT RecipeDlg::OnTabDeselected(WPARAM wP, LPARAM lP) {
	if (bTabSelected) {
		bTabSelected = FALSE;
	}
	return 0;
}

void RecipeDlg::camRun() {
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::ECAM::PRICAM);
	if (pCam != NULL) {
		Ctx.hWnd = cLiveVid.GetSafeHwnd();
		cLiveVid.GetClientRect(Ctx.rc);
		pCam->StopStream(Ctx, pCam->SCaM.ID);
		pCam->StartStream(Ctx, pCam->SCaM.ID);
	}
}

void RecipeDlg::OnBnClickedMotSetup() {
	// TODO: Add your control notification handler code here
	pWLIView->ShowMoSetup(0);
}

void RecipeDlg::OnBnClickedCamProp() {
	// TODO: Add your control notification handler code here
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->propertyPageDisplay();
}