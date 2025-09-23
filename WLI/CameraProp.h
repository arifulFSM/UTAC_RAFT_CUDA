#pragma once
#include "afxdialogex.h"
#include "Dev.h"

// CameraProp dialog

class CameraProp : public CDialogEx {
	DECLARE_DYNAMIC(CameraProp)

public:
	CameraProp(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CameraProp();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CAM_PROP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL OnInitDialog();
	CAM::ECAM EID = CAM::ECAM::INVD;
	CString AEVal = L"";
	CComboBox m_AutoExpo;
	afx_msg void OnCbnSelchangeCombAutoExpo();
	afx_msg void OnCbnSelchangeCombAutoWb();
	CEdit m_ExpoVal;
	CString AWBVal = L"";
	CComboBox m_AutoWB;
	CString TRIGVal = L"";
	afx_msg void OnCbnSelchangeTrigMode();
	CComboBox m_TrigMode;
	afx_msg void OnEnKillfocusEditExpoVal();
};
