#pragma once
#include "afxdialogex.h"
#include "SRC/ResizableDialog.h"
#include <vector>
#include "Dev.h"
#include "MeasProgressDlg.h"
#include "WaferMap.h"

// RecipeDlg dialog

class RecipeDlg : public CResizableDialog {
	DECLARE_DYNAMIC(RecipeDlg)

public:
	RecipeDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~RecipeDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RCP_DLG };
#endif

protected:
	CRecipeRAFT* pRcp;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	LRESULT OnAddPoint(WPARAM wP, LPARAM lP);
	LRESULT OnAddalgnpoint(WPARAM wP, LPARAM lP);
	LRESULT OnDeletepoint(WPARAM wP, LPARAM lP);

	DECLARE_MESSAGE_MAP()
public:
	//CEdit m_X1;
	//CEdit m_Y1;
	//afx_msg void OnBnClickedBtnPntSave();
	//afx_msg void OnBnClickedBtnLoadRcp();
	//afx_msg void OnBnClickedBtnMeasure();
	//void Serialize(CArchive& ar);
	std::vector<std::pair<float, float>>RecipePoints;
	MOT::CMotorCtrl* MO = nullptr; // pointer only
	int idx = 0;
	//void MoveXYPos();
	std::string recipeName = "";
	//void SaveImageData();
	//afx_msg void OnBnClickedBtnAddPoints();
	std::vector<std::pair<float, float>>tmpRcpPoints;
	//void AddRcpPointsListCtrl();
	//afx_msg void OnBnClickedBtnNewRcp();
	//afx_msg void OnNMDblclkListShowRcpPoint(NMHDR* pNMHDR, LRESULT* pResult);
	MeasProgressDlg* measDlg = nullptr;
	CWaferMap m_cWaferMap;
	void Renumber();
	CListCtrl	m_cPoint;
	afx_msg void OnBnClickedSaveRecipe();
	afx_msg void OnBnClickedLoadRecipe();
	void RecipeToLocal();
	afx_msg void OnBnClickedNewRecipe();
	afx_msg void OnBnClickedBtnCalibPos();
	void UpdateValue();
	void SetValue();
	void ReSetValue();
	CEdit m_AFRange;
	CEdit m_AFStepSize;
	CEdit m_MERange;
	CEdit m_MEFrames;
	CEdit m_AFCalZ;
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	CStatic cLiveVid;
	void camRun();
	afx_msg void OnBnClickedMotSetup();
	afx_msg void OnBnClickedCamProp();
};

extern RecipeDlg* recipeItems;