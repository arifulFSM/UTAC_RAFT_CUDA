#pragma once

#include "MoCtrl.h"
#include "MotionControlDlg.h"
#include "CameraDlg.h"
#include "BaslerCameraDlg.h" //12222022
#include "RecipeDlg.h" //07252023
#include "RoughnessDlg.h"

//#include "StripDlg.h"
//#include "HeightDlg.h"
#include "SRC/XTabCtrl.h"
#include "SRC/ResizableFormView.h"

class CWLIDoc;
class CStripDlg;
class CHeightDlg;
class HeightPlot; // 05302023 - Mortuja
class CAcqDlg;
class MeasurementDlg;
class AnalysisDlg;
class ResultDlg;

class CWLIView : public CResizableFormView {
	CStripDlg* pStrip = nullptr;
	CHeightDlg* pHeight = nullptr;
	HeightPlot* heightPlotDlg = nullptr; // 05302023 - Mortuja
	RecipeDlg* rcpDlg = nullptr; //07252023
	MeasurementDlg* measDlg = nullptr;
	AnalysisDlg* analysisDlg = nullptr;
	RoughnessDlg* roughDlg = nullptr;
	CAcqDlg* acqDlg = nullptr;
	ResultDlg* rsltDlg = nullptr;

	void Refresh() {
		// show image and plot
	}

public:
	static CMoCtrl* pMCtr;
	static CMotionControlDlg* pMSet;
	static CCameraDlg* pCam1, * pCam2;
	//static BaslerCameraDlg* pBaslerCam;
	BaslerCameraDlg* pBaslerCam = nullptr;

protected: // create from serialization only
	CWLIView() noexcept;
	DECLARE_DYNCREATE(CWLIView)

		CXTabCtrl cTab;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WLI_FORM };
#endif

public:
	CWLIDoc* GetDocument() const;
	static CWLIView* GetView();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

public:
	virtual ~CWLIView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	void HideMoCtrl();
	void HideMoSetup();
	void ShowMoCtrl();
	void ShowMoSetup(HWND hParent);

public:
	afx_msg void OnDestroy();
	afx_msg void OnWliCamera1();
	afx_msg void OnWliCamera2();
	afx_msg void OnWliMotioncontroller();
	afx_msg void OnWliAcquire();
	afx_msg void OnWliMotorsetup();
protected:
	afx_msg LRESULT OnUmStripLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmHeightCalced(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUmHeightCalc(WPARAM wParam, LPARAM lParam); // 05302023 - Mortuja
	afx_msg LRESULT OnRoughnessDlg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBaslerCamera();
	afx_msg void OnRecipeCreaterecipe();
	afx_msg LRESULT OnUmResultDlg(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // debug version in WLIView.cpp
inline CWLIDoc* CWLIView::GetDocument() const {
	return reinterpret_cast<CWLIDoc*>(m_pDocument);
}
#endif

extern CWLIView* pWLIView;
