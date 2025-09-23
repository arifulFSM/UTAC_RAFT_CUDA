#pragma once
#include "afxdialogex.h"

#include "PicWnd.h"
#include "FringeWnd.h"
#include "PSI/Frng.h"
#include "SRC/ResizableDialog.h"

class CHeightDlg : public CResizableDialog {
	DECLARE_DYNAMIC(CHeightDlg)

	CStatic cInfo;
	CPicWnd cPicWnd;
	CFringeWnd cPlot;

	bool bHorz = true;
	int X = 0, Y = 0;

	void ShowPlot(int x, int y);

public:
	CHeightDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CHeightDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG4 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	afx_msg LRESULT OnUmHeightCalced(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWlMousemove(WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton5();
};
