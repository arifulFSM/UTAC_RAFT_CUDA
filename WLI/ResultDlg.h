#pragma once
#include "afxdialogex.h"
#include "SRC/ResizableDialog.h"
#include "SRC/XTabDefine.h"
#include "ResultRAFT.h"

// ResultDlg dialog

class ResultDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(ResultDlg)

public:
	ResultDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ResultDlg();
	CResultRAFT* pResult;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RESULT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnTabSelected(WPARAM wP, LPARAM lP);
	LRESULT OnTabDeselected(WPARAM wP, LPARAM lP);
	afx_msg void OnBnClickedImportResult();
	afx_msg void OnBnClickedExportResult();
	CListCtrl m_Result;
	afx_msg LRESULT OnUmResultDlg(WPARAM wParam, LPARAM lParam);
};
