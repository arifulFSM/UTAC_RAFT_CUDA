#include "pch.h"
#include "framework.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WLI.h"
#endif

#include "AcqDlg.h"
#include "wdefine.h"
#include "StripDlg.h"
#include "HeightDlg.h"
#include "HeightPlot.h" // 05302023 - Mortuja
#include "MeasurementDlg.h"
#include "AnalysisDlg.h"

#include "WLIDoc.h"
#include "WLIView.h"
#include "BaslerCameraDlg.h" //12222022
#include "RAFTApp.h"
#include "ResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMoCtrl* CWLIView::pMCtr = nullptr;
CMotionControlDlg* CWLIView::pMSet = nullptr;
CCameraDlg* CWLIView::pCam1 = nullptr;
CCameraDlg* CWLIView::pCam2 = nullptr;
CWLIView* pWLIView = nullptr;
//BaslerCameraDlg* CWLIView::pBaslerCam = nullptr; //12222022

IMPLEMENT_DYNCREATE(CWLIView, CResizableFormView)

BEGIN_MESSAGE_MAP(CWLIView, CResizableFormView)
	ON_WM_DESTROY()
	ON_COMMAND(ID_WLI_CAMERA, &CWLIView::OnWliCamera1)
	ON_COMMAND(ID_WLI_MOTIONCONTROLLER, &CWLIView::OnWliMotioncontroller)
	ON_COMMAND(ID_WLI_ACQUIRE, &CWLIView::OnWliAcquire)
	ON_COMMAND(ID_WLI_MOTORSETUP, &CWLIView::OnWliMotorsetup)
	ON_MESSAGE(UM_STRIP_LOADED, &CWLIView::OnUmStripLoaded)
	ON_COMMAND(ID_CAMERA2, &CWLIView::OnWliCamera2)
	ON_MESSAGE(UM_HEIGHT_CALCED, &CWLIView::OnUmHeightCalced)
	ON_COMMAND(ID_BASLERCAMERA, &CWLIView::OnBaslerCamera)
	ON_MESSAGE(IDC_HEIGHT_MAP_PM, &CWLIView::OnUmHeightCalc) // 05302023 - Mortuja
	ON_MESSAGE(IDC_ROUGHNESS_DLG, &CWLIView::OnRoughnessDlg)
	ON_COMMAND(ID_RECIPE_CREATERECIPE, &CWLIView::OnRecipeCreaterecipe)
	ON_MESSAGE(UM_RESULT_DLG, &CWLIView::OnUmResultDlg)
END_MESSAGE_MAP()

CWLIView::CWLIView() noexcept
	: CResizableFormView(IDD_WLI_FORM) {
	pWLIView = this;
}

CWLIView::~CWLIView() {
	pWLIView = nullptr;
	if (pRAFTApp != NULL) {
		delete pRAFTApp;
	}
}

void CWLIView::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, cTab);
}

CWLIView* CWLIView::GetView() {
	CFrameWnd* pFrame = (CFrameWnd*)(AfxGetApp()->m_pMainWnd);
	CView* pView = pFrame->GetActiveView();
	if (!pView) return nullptr;
	if (!pView->IsKindOf(RUNTIME_CLASS(CWLIView))) return nullptr;
	return (CWLIView*)pView;
}

BOOL CWLIView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CResizableFormView::PreCreateWindow(cs);
}

void CWLIView::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);
	CResizableFormView::OnInitialUpdate();
	//GetParentFrame()->RecalcLayout();
	//ResizeParentToFit();

	short nTab = 0;
	pRAFTApp = new CRAFTApp;
	rcpDlg = new RecipeDlg;
	if (rcpDlg) {
		rcpDlg->Create(IDD_RCP_DLG, &cTab);
		cTab.AddTab(rcpDlg, CString("Recipe").GetBuffer(), nTab++);
	}
	measDlg = new MeasurementDlg;
	if (measDlg) {
		measDlg->hWndParent = GetSafeHwnd();
		measDlg->Create(IDD_MEASUREMENT_DLG, &cTab);
		cTab.AddTab(measDlg, CString("Measurement").GetBuffer(), nTab++);
	}
	rsltDlg = new ResultDlg;
	if (rsltDlg) {
		rsltDlg->Create(IDD_DLG_RESULT, &cTab);
		cTab.AddTab(rsltDlg, CString("Result").GetBuffer(), nTab++);
	}
	analysisDlg = new AnalysisDlg;
	if (analysisDlg) {
		analysisDlg->Create(IDD_ANALYSIS_DLG, &cTab);
		cTab.AddTab(analysisDlg, CString("Analysis").GetBuffer(), nTab++);
	}
	pStrip = new CStripDlg;
	if (pStrip) {
		pStrip->hWndParent = GetSafeHwnd();
		pStrip->Create(IDD_DIALOG8, &cTab);
		cTab.AddTab(pStrip, CString("Interferogram").GetBuffer(), nTab++); // Enable Morsalin
	}
	pHeight = new CHeightDlg;
	if (pHeight) {
		pHeight->Create(IDD_DIALOG4, &cTab);
		//cTab.AddTab(pHeight, CString("Height").GetBuffer(), nTab++);
	}

	AnchorControls();
}

#ifdef _DEBUG
void CWLIView::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CWLIView::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}

CWLIDoc* CWLIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWLIDoc)));
	return (CWLIDoc*)m_pDocument;
}
#endif //_DEBUG

void CWLIView::OnDestroy() {
	CResizableFormView::OnDestroy();

	if (pCam1) delete pCam1;
	if (pCam2) delete pCam2;
	if (pMCtr) delete pMCtr;
	if (pMSet) delete pMSet;
	if (pStrip) delete pStrip;
	if (pHeight) delete pHeight;
	if (pBaslerCam) delete pBaslerCam;
	if (heightPlotDlg) delete heightPlotDlg;
	if (rcpDlg) delete rcpDlg; //07252023
	if (measDlg) delete measDlg;
	if (analysisDlg) delete analysisDlg;
	if (roughDlg) delete roughDlg;
	if (rsltDlg) delete rsltDlg;
}

void CWLIView::HideMoCtrl() {
	if (pMSet) pMSet->ShowWindow(SW_HIDE);
}

void CWLIView::HideMoSetup() {
	if (pMCtr) {
		pMCtr->ShowWindow(SW_HIDE);
	}
}

void CWLIView::ShowMoCtrl() {
	if (!pMCtr) {
		pMCtr = new CMoCtrl;
		if (pMCtr) pMCtr->Create(IDD_DIALOG1);
	}
	if (pMCtr) {
		if (pMCtr->IsWindowVisible()) return;
		pMCtr->ShowWindow(SW_SHOW);
	}
}

void CWLIView::ShowMoSetup(HWND hParent) {
	if (!pMSet) {
		pMSet = new CMotionControlDlg;
		if (pMSet) pMSet->Create(IDD_DIALOG10);
		return;
	}
	if (pMSet) {
		pMSet->SetHParent(hParent);
		if (pMSet->IsWindowVisible()) return;
		pMSet->ShowWindow(SW_SHOW);
	}
}

void CWLIView::OnWliMotioncontroller() {
	ShowMoCtrl();
}

void CWLIView::OnWliMotorsetup() {
	ShowMoSetup(0);
}

void CWLIView::OnWliAcquire() {
	CAcqDlg dlg;
	if (dlg.DoModal() == IDOK) {
		Refresh();
	}
	/*if (!acqDlg) {
		acqDlg = new CAcqDlg;
		if (acqDlg) {
			acqDlg->Create(IDD_DIALOG3);
			acqDlg->SetWindowTextW(L"Acquisition Dialog");
			acqDlg->ShowWindow(SW_SHOW);
		}
	}*/
}

afx_msg LRESULT CWLIView::OnUmStripLoaded(WPARAM wParam, LPARAM lParam) {
	CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Interferogram")));
	if (pWnd) {
		::PostMessage(pWnd->GetSafeHwnd(), UM_STRIP_LOADED, wParam, lParam);
	}
	return 0;
}

void CWLIView::OnWliCamera1() {
	if (!pCam1) {
		pCam1 = new CCameraDlg;
		if (pCam1) {
			pCam1->eID = CAM::PRICAM;
			pCam1->Create(IDD_DIALOG2);
			pCam1->SetWindowTextW(L"Main Camera");
		}
	}
	if (pCam1) {
		if (pCam1->IsWindowVisible()) return;
		pCam1->ShowWindow(SW_SHOW);
	}
}

void CWLIView::OnWliCamera2() {
	if (!pCam2) {
		pCam2 = new CCameraDlg;
		if (pCam2) {
			pCam2->eID = CAM::SECCAM;
			pCam2->Create(IDD_DIALOG2);
			pCam2->SetWindowTextW(L"Secondary Camera");
		}
	}
	if (pCam2) {
		if (pCam2->IsWindowVisible()) return;
		pCam2->ShowWindow(SW_SHOW);
	}
}

afx_msg LRESULT CWLIView::OnUmHeightCalced(WPARAM wParam, LPARAM lParam) {
	//TODO: Perform Height calculation based on selected method

	//! Switch & Notify Height dialog
	if (pHeight) {
		CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Height")));
		if (pWnd) {
			::PostMessage(pWnd->GetSafeHwnd(), UM_HEIGHT_CALCED, wParam, lParam);
		}
	}
	return 0;
}

void CWLIView::OnBaslerCamera() {
	if (pBaslerCam) {
		pBaslerCam->DeleteContents();
		delete pBaslerCam;
		pBaslerCam = nullptr;
	}

	if (!pBaslerCam) {
		pBaslerCam = new BaslerCameraDlg;
		if (pBaslerCam) {
			pBaslerCam->Create(IDD_DIALOG15); //Basler Camera Dlg
			pBaslerCam->SetWindowTextW(L"Main Basler Camera");
			pBaslerCam->ShowWindow(SW_SHOW);
		}
	}
	/*if (pBaslerCam) {
		if (pBaslerCam->IsWindowVisible()) return;
		pBaslerCam->ShowWindow(SW_SHOW);
	}*/
}

// 05302023 - Mortuja [START]

afx_msg LRESULT CWLIView::OnUmHeightCalc(WPARAM wParam, LPARAM lParam) {
	if (heightPlotDlg) {
		delete heightPlotDlg;
		heightPlotDlg = NULL;
	}

	heightPlotDlg = new HeightPlot;
	heightPlotDlg->Create(IDD_DLG_HEIGHT_PLOT);
	heightPlotDlg->ShowWindow(SW_SHOW);

	HWND hWnd = heightPlotDlg->GetSafeHwnd();
	::PostMessage(hWnd, IDC_HEIGHT_MAP_PM, wParam, lParam);

	return 0;
}

// 05302023 - Mortuja [END]

afx_msg LRESULT CWLIView::OnRoughnessDlg(WPARAM wParam, LPARAM lParam) {
	if (roughDlg) {
		delete roughDlg;
		roughDlg = NULL;
	}

	roughDlg = new RoughnessDlg;
	roughDlg->Create(IDD_DLG_ROUGHNESS);
	roughDlg->ShowWindow(SW_SHOW);

	return 0;
}

void CWLIView::OnRecipeCreaterecipe() {
	// TODO: Add your command handler code here
	return;
	if (rcpDlg) {
		delete rcpDlg;
		rcpDlg = NULL;
	}
	rcpDlg = new RecipeDlg;
	if (rcpDlg) {
		rcpDlg->Create(IDD_RCP_DLG);
		rcpDlg->ShowWindow(SW_SHOW);
	}
	//07252023 - END
}

afx_msg LRESULT CWLIView::OnUmResultDlg(WPARAM wParam, LPARAM lParam) {
	//! Switch & Notify Result dialog
	if (rsltDlg) {
		CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Result")));
		if (pWnd) {
			::PostMessage(pWnd->GetSafeHwnd(), UM_RESULT_DLG, wParam, lParam);
		}
	}
	return 0;
}