#pragma once

#include "afxdialogex.h"

#include "LOG/LogSys.h"
#include "PicWnd.h"
#include "SRC/ResizableDialog.h"
#include "ScanPar.h"
#include "BaslerCamDev.h"

class CAcqDlg : public CResizableDialog {
	DECLARE_DYNAMIC(CAcqDlg)

		bool bRun = false;
	CButton cScan;
	CButton cOK;
	CStatic cStatus;
	CPicWnd cPicWnd;
	CStatic cLiveVid;
	CString numScan;							// 10282022 / yukchiu

	void ActivateButton(bool bActv);
	void AbortOpr(short ErrNo, float iniPos, float now = 0);
	void Message(const std::wstring& msg);

public:
	CAcqDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAcqDlg();
	Pylon::CPylonBitmapImage tmpBMPImage;
	CBaslerCamDev pCam;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnBnClickedPsiscan();
	void OnBnClickedPsiscanWithPriorPiezo();
	void OnBnClickedPsiscanWithEOPiezo();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit12();
	afx_msg void OnEnKillfocusEdit20();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnBnClickedPsiscan2();
	afx_msg void OnBnClickedPsiscan3();
	afx_msg void OnBnClickedPsiscan4();
	afx_msg void OnBnClickedPsiscan5();
	afx_msg void OnBnClickedPsiscan6();
	afx_msg void OnBnClickedPsiscan7();
	afx_msg void OnSettingProperties();
	afx_msg void OnAwb();
	afx_msg void OnAec();
	afx_msg void OnSettingVideoformat();
	afx_msg void OnSetting();
	afx_msg void OnGrab();
	afx_msg void OnBnClickedCheck1();
protected:
	afx_msg LRESULT OnUmClose(WPARAM wParam, LPARAM lParam);
public:
	CString cNoScan;											// 10282022 / yukchiu / multiple scan
	afx_msg void OnBnClickedMutscanBut();						// 10282022 / yukchiu / multiple scan
	afx_msg void OnEnKillfocusMutScan();						// 10282022 / yukchiu / multiple scan
	CEdit m_PriorStepSize;
	CEdit m_PriorEndPos;
	CEdit m_PriorStartPos;
};
