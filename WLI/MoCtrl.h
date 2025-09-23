#pragma once

#include <sstream>
#include <iomanip>
#include "MOTOR/Motor.h"
#include "afxdialogex.h"

// CMoCtrl dialog

class CMoCtrl : public CDialogEx {
	DECLARE_DYNAMIC(CMoCtrl)

	float pX = 0, pY = 0;
	std::wstring GetPos_um(MOT::MAXIS nAxis);
	void UpdateMotorPosition();

public:
	CMoCtrl(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CMoCtrl();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnEnKillfocusEdit3();
};
