// CameraDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "Dev.h"
#include "CamSetDlg.h"
#include "afxdialogex.h"
#include "CameraDlg.h"

// CCameraDlg dialog

IMPLEMENT_DYNAMIC(CCameraDlg, CResizableDialog)

CCameraDlg::CCameraDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DIALOG2, pParent) {}

CCameraDlg::~CCameraDlg() {}

void CCameraDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERA, cLiveVid);
}

BEGIN_MESSAGE_MAP(CCameraDlg, CResizableDialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_START, &CCameraDlg::OnStart)
	ON_COMMAND(ID_STOP, &CCameraDlg::OnStop)
	ON_COMMAND(ID_PAUSE, &CCameraDlg::OnPause)
	ON_COMMAND(ID_CONTINUE, &CCameraDlg::OnContinue)
	ON_COMMAND(ID_GRAB, &CCameraDlg::OnGrab)
	ON_COMMAND(ID_SETTING, &CCameraDlg::OnSetting)
	ON_COMMAND(ID_AEC, &CCameraDlg::OnAec)
	ON_COMMAND(ID_AWB, &CCameraDlg::OnAwb)
	ON_COMMAND(ID_SETTING_PROPERTIES, &CCameraDlg::OnSettingProperties)
	ON_COMMAND(ID_SETTING_VIDEOFORMAT, &CCameraDlg::OnSettingVideoformat)
END_MESSAGE_MAP()

// CCameraDlg message handlers

void CCameraDlg::OnClose() {
	OnStop();
	CResizableDialog::OnClose();
}

void CCameraDlg::OnDestroy() {
	CResizableDialog::OnDestroy();
}

BOOL CCameraDlg::OnInitDialog() {
	AddAnchor(IDC_CAMERA, TOP_LEFT, BOTTOM_RIGHT);

	CResizableDialog::OnInitDialog();
	ArrangeLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCameraDlg::OnStart() {
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	Ctx.hWnd = cLiveVid.GetSafeHwnd();
	cLiveVid.GetClientRect(Ctx.rc);
	pCam->StartStream(Ctx, pCam->SCaM.ID);
}

void CCameraDlg::OnStop() {
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	Ctx.hWnd = cLiveVid.GetSafeHwnd();
	cLiveVid.GetClientRect(Ctx.rc);
	pCam->StopStream(Ctx, pCam->SCaM.ID);
}

void CCameraDlg::OnPause() {
	// TODO: Add your command handler code here
}

void CCameraDlg::OnContinue() {
	// TODO: Add your command handler code here
}

void CCameraDlg::OnGrab() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, eID, Dev.Cam.pCm[eID]->subSampling)) {
		Im.SavePic();
	}
}

void CCameraDlg::OnSetting() {
	CCamSetDlg dlg; dlg.DoModal();
}

void CCameraDlg::OnAec() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	pCam->AutoExposure();
}

void CCameraDlg::OnAwb() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	pCam->WhiteBalance();
}

void CCameraDlg::OnSettingProperties() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	pCam->propertyPageDisplay();
	//pCam->LucamPropPage();
}

void CCameraDlg::OnSettingVideoformat() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(eID); if (!pCam) return;
	pCam->LucamVideoFormat();
}