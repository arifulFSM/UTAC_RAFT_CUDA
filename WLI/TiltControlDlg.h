#pragma once
#include "afxdialogex.h"

#include "MOTOR/MotorCtrl.h"

class CTiltControlDlg : public CDialogEx {
	DECLARE_DYNAMIC(CTiltControlDlg)

	bool bBlock = false;
	bool bCheck = true;
	MOT::CMotorCtrl* MO = nullptr; // pointer only
	const float incTUV = INCTUV, incUD = INCUD;
	void UpdatePositions();
	void TiltUpDown(float inc, DWORD tout, bool bCheck);
	void MoveR(MOT::MAXIS nAx, float inc, DWORD tout, bool bCheck);

public:
	CTiltControlDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTiltControlDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG9 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton20();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton48();
	afx_msg void OnBnClickedButton51();
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton13();
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton49();
	afx_msg void OnBnClickedButton52();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedButton33();
	afx_msg void OnBnClickedButton34();
	afx_msg void OnBnClickedButton50();
	afx_msg void OnBnClickedButton53();
	afx_msg void OnBnClickedButton35();
	afx_msg void OnBnClickedButton36();
	afx_msg void OnBnClickedButton40();
	afx_msg void OnBnClickedButton41();
	afx_msg void OnBnClickedButton30();
	afx_msg void OnBnClickedButton29();
	afx_msg void OnBnClickedButton31();
	afx_msg void OnBnClickedButton32();
	afx_msg void OnBnClickedButton64();
	afx_msg void OnBnClickedButton62();
	afx_msg void OnBnClickedButton63();
	afx_msg void OnBnClickedButton65();
};
