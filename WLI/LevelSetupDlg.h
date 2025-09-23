#pragma once
#include "afxdialogex.h"

#include "Spot.h"
#include "SRC/InPEdit.h"
#include "SRC/EdListCtrl.h"

class CLevelSetupDlg : public CDialogEx {
	DECLARE_DYNAMIC(CLevelSetupDlg)

	SSpotCtx Ctx[MAXSPOTCTX];
	CEdListCtrl cGrid;

	bool InitGrid();
	bool LoadGrid();

public:
	CLevelSetupDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CLevelSetupDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG13 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
