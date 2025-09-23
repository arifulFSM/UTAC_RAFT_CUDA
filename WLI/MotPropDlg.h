#pragma once
#include "afxdialogex.h"

#include "MOTOR/Motor.h"
#include "SRC/InPEdit.h"
#include "SRC/EdListCtrl.h"
#include "SRC/ResizableDialog.h"

class CMotPropDlg : public CResizableDialog {
	DECLARE_DYNAMIC(CMotPropDlg)

	CEdListCtrl cGrid;
	MOT::CMotorCtrl* MC = nullptr; // reference only
	MOT::SMotPar Mlocal[int(MOT::MAXIS::LAST)];
	int iItem = 0, iSubItem = 0;
	CString text = _T("");

public:
	CMotPropDlg(MOT::CMotorCtrl* mc, CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMotPropDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG7 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	bool InitGrid();
	bool LoadGrid();
	virtual BOOL OnInitDialog();
	virtual void OnOK();

public:
	afx_msg void OnBnClickedMotPropRestore();

protected:
	afx_msg LRESULT OnNotifyDescriptionEdited(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNotifyKillFocus(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOk();
};
