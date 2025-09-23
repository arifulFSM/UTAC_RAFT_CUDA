#pragma once
#include "afxdialogex.h"

class CMoreSettingDlg : public CDialogEx {
	DECLARE_DYNAMIC(CMoreSettingDlg)

public:
	CMoreSettingDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMoreSettingDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG14 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio10();
	afx_msg void OnBnClickedRadio11();
	afx_msg void OnBnClickedRadio12();
};
