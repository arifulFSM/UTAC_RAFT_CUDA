// CameraProp.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "CameraProp.h"
#include <string>

// CameraProp dialog

IMPLEMENT_DYNAMIC(CameraProp, CDialogEx)

CameraProp::CameraProp(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CAM_PROP, pParent) {}

CameraProp::~CameraProp() {}

void CameraProp::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_AUTO_EXPO, m_AutoExpo);
	DDX_Control(pDX, IDC_EDIT_EXPO_VAL, m_ExpoVal);
	DDX_Control(pDX, IDC_COMB_AUTO_WB, m_AutoWB);
	DDX_Control(pDX, IDC_TRIG_MODE, m_TrigMode);
}

BEGIN_MESSAGE_MAP(CameraProp, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMB_AUTO_EXPO, &CameraProp::OnCbnSelchangeCombAutoExpo)
	ON_CBN_SELCHANGE(IDC_COMB_AUTO_WB, &CameraProp::OnCbnSelchangeCombAutoWb)
	ON_CBN_SELCHANGE(IDC_TRIG_MODE, &CameraProp::OnCbnSelchangeTrigMode)
	ON_EN_KILLFOCUS(IDC_EDIT_EXPO_VAL, &CameraProp::OnEnKillfocusEditExpoVal)
END_MESSAGE_MAP()

// CameraProp message handlers

BOOL CameraProp::OnInitDialog() {
	__super::OnInitDialog();

	float expoVal = Dev.Cam.GetCamera(EID)->GetExposure();
	CString s;
	s.Format(L"%f", expoVal);
	m_ExpoVal.SetWindowTextW(s);

	return TRUE;
}

void CameraProp::OnCbnSelchangeCombAutoExpo() {
	// TODO: Add your control notification handler code here
	m_AutoExpo.GetWindowTextW(AEVal);
	Dev.Cam.GetCamera(EID)->AutoExposure();
	float expoVal = Dev.Cam.GetCamera(EID)->GetExposure();
	CString s;
	s.Format(L"%f", expoVal);
	m_ExpoVal.SetWindowTextW(s);
}

void CameraProp::OnCbnSelchangeCombAutoWb() {
	// TODO: Add your control notification handler code here
	m_AutoWB.GetWindowTextW(AWBVal);
	Dev.Cam.GetCamera(EID)->WhiteBalance();
}

void CameraProp::OnCbnSelchangeTrigMode() {
	// TODO: Add your control notification handler code here
	m_TrigMode.GetWindowTextW(TRIGVal);
	Dev.Cam.GetCamera(EID)->SetTrigger();
}

void CameraProp::OnEnKillfocusEditExpoVal() {
	// TODO: Add your control notification handler code here
	CString ExpoVal = L"";
	m_ExpoVal.GetWindowTextW(ExpoVal);
	float val = atof(std::string(CT2A(ExpoVal)).c_str());
	Dev.Cam.GetCamera(EID)->SetExposure(val);
}