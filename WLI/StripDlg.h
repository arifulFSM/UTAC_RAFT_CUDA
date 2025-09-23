#pragma once
#include "afxdialogex.h"

#include "PicWnd.h"
#include "FringeWnd.h"
#include "PSI/Frng.h"
#include "SRC/ResizableDialog.h"

#include<pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include<pylon/PylonGUI.h>
#endif

class CStripDlg : public CResizableDialog {
	DECLARE_DYNAMIC(CStripDlg)

		bool bFrnSrchStop = false;
	int X = 0, Y = 0;
	IMGL::CIM ImT; // intermediate image
	HWND hWndParent = 0;
	CPicWnd cPicWnd;
	CStatic cReport;
	CFringeWnd cPlot;
	CStatic cMsg;
	CStatic cMsg2;

	void Refresh();
	bool Make(IMGL::Slin& lin, WLI::SFrng& F, WLI::FRP Ch, SROI& R);
	void ShowPlot(int x, int y);
	void Yld(DWORD nTime);
	void UpdateImage(int idx);
	void UpdateUIMessages(int idx);

public:
	CStripDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CStripDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG8 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedCheck4();
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedCheck6();

protected:
	afx_msg LRESULT OnUmStripLoaded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWlMousemove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWlMouseDown(WPARAM wParam, LPARAM lParam); // 05302023 - Mortuja
	afx_msg LRESULT OnWlMouseUp(WPARAM wParam, LPARAM lParam);  // 05302023 - Mortuja

public:
	afx_msg void OnBnClickedCheck7();
	afx_msg void OnBnClickedButton16();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedButton71();
	afx_msg void OnBnClickedCheck12();
	afx_msg void OnBnClickedCheck13();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio9();
	afx_msg void OnBnClickedButton78();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton88();
	afx_msg void OnBnClickedButton91();
	afx_msg void OnBnClickedCheck14();
	afx_msg void OnBnClickedButton99();
	afx_msg void OnBnClickedButton100();
	afx_msg void OnBnClickedButton15();

	int EnumerateDevices();

private:
	Pylon::DeviceInfoList_t m_devices;
	Pylon::CInstantCamera m_camera;

	// 05302023 - Mortuja
public:
	afx_msg void OnBnClickedRadArea();
	afx_msg void OnBnClickedRadLine();
	afx_msg void OnBnClickedRadHorizon();
	afx_msg void OnBnClickedRadVertic();
	//afx_msg void OnBnClickedEnableDisableHfunc();
};
