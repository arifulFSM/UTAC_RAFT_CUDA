#pragma once

#include "MoSetup.h"
#include "afxdialogex.h"

class CFocusLevelDlg : public CDialogEx, CMoSetupB {
	DECLARE_DYNAMIC(CFocusLevelDlg)

public:
	CFocusLevelDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFocusLevelDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG12 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
