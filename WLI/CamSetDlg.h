#pragma once

#include "afxdialogex.h"

class CCamSetDlg : public CDialogEx {
	DECLARE_DYNAMIC(CCamSetDlg)

	CListBox cListBox;

public:
	CCamSetDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCamSetDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG6 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
