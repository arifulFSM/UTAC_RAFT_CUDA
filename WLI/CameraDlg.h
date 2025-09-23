#pragma once
#include "afxdialogex.h"
#include "PicWnd.h"
#include "CAM/Camera.h"
#include "SRC/ResizableDialog.h"

// CCameraDlg dialog

class CCameraDlg : public CResizableDialog {
	DECLARE_DYNAMIC(CCameraDlg)

		CPicWnd cLiveVid;

public:
	CCameraDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CCameraDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif
	CAM::ECAM eID = CAM::ECAM::INVD;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnPause();
	afx_msg void OnContinue();
	afx_msg void OnGrab();
	afx_msg void OnSetting();
	afx_msg void OnAec();
	afx_msg void OnAwb();
	afx_msg void OnSettingProperties();
	afx_msg void OnSettingVideoformat();
};
