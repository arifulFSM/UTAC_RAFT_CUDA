#pragma once

#include "Dev.h"
#include "afxdialogex.h"

class CXYStageDlg : public CDialogEx {
	DECLARE_DYNAMIC(CXYStageDlg)

	bool bCheck = true;
	const float incXY = 10.f;
	float Xpos = 0, Ypos = 0, Rpos = 0, Apos = 0;
	MOT::CMotorCtrl* MO = nullptr; // pointer only

	void UpdatePositions();

	void MoveR(MOT::MAXIS nAx, float inc, DWORD tout, bool bCheck);

public:
	CXYStageDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CXYStageDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG11 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton42();
	afx_msg void OnBnClickedButton43();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton44();
	afx_msg void OnBnClickedButton45();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCheck11();
	afx_msg void OnBnClickedButton38();
	afx_msg void OnBnClickedButton39();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton24();
	afx_msg void OnBnClickedButton80();
	afx_msg void OnBnClickedButton81();
};
