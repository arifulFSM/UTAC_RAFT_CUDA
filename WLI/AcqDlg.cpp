#include "pch.h"
#include "WLI.h"
#include "WLIView.h"

#include "CamSetDlg.h"
#include "Dev.h"
#include "LOG/LogSys.h"
#include "MoSetup.h"
#include "PSI/ICC.h"
#include "PSI/Strip.h"
#include "ScanPar.h"
#include "SRC/hightime.h"
#include "wdefine.h"

#include "afxdialogex.h"
#include "AcqDlg.h"
#include "RecipeDlg.h"

IMPLEMENT_DYNAMIC(CAcqDlg, CResizableDialog)

void CAcqDlg::AbortOpr(short ErrNo, float iniPos, float now) {
	std::wstring msg;
	//MOT::CPiezo& Piezo = Dev.MC->Piezo;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM);
	switch (ErrNo) {
	case 1:
		LgS.Log(L"Error: Camera not connected", LOG::CERROR);
		break;
	case 2:
		if (pCam->bIsStream) OnStart();
		LgS.Log(L"Error: Retrieve camera properties failed", LOG::CERROR);
		break;
	case 3:
		if (pCam->bIsStream) OnStart();
		LgS.Log(L"Error: Setup fast grab failed", LOG::CERROR);
		break;
	case 4:
		pCam->StopFastGrab();
		if (pCam->bIsStream) OnStart();
		msg = L"Error: Piezo position " + std::to_wstring(now) + L"cannot be reached";
		LgS.Log(msg.c_str(), LOG::CERROR);
		break;
	case 5:
		pCam->StopFastGrab();
		if (pCam->bIsStream) OnStart();
		LgS.Log(L"Error: Fail to grab background frame", LOG::CERROR);
		break;
	case 6:
		LgS.Log(L"Warning: Piezo goto timeout", LOG::WARNING);
		break;
	case 7:
		pCam->StopFastGrab();
		if (pCam->bIsStream) OnStart();
		LgS.Log(L"Error: Fail to allocate image memory", LOG::CERROR);
		break;
	case 8:
		pCam->StopFastGrab();
		if (pCam->bIsStream) OnStart();
		msg = L"Error: Fail to grab frame #" + std::to_wstring(int(now));
		LgS.Log(msg.c_str(), LOG::CERROR);
		break;
	default:
		break;
	}

	if (iniPos >= 0) Dev.MC->Piezo.Goto(iniPos, false);
	//Strip.AddTime(CHighTime::GetPresentTime());

	cOK.EnableWindow(TRUE);
	cScan.EnableWindow(TRUE);
}

void CAcqDlg::Message(const std::wstring& msg) {
	cStatus.SetWindowTextW(msg.c_str());
}

CAcqDlg::CAcqDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DIALOG3, pParent), cNoScan(_T("")) {}

CAcqDlg::~CAcqDlg() {}

void CAcqDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT4, ICC.SPar.ZRange);
	DDX_Text(pDX, IDC_EDIT12, ICC.SPar.NSteps);
	DDX_Text(pDX, IDC_EDIT20, ICC.SPar.NSlice);
	DDX_Text(pDX, IDC_EDIT10, ICC.SPar.Cwlen);
	DDX_Control(pDX, IDC_CAMERA, cLiveVid);
	DDX_Control(pDX, IDC_STATUS, cStatus);
	DDX_Control(pDX, IDC_PICWND, cPicWnd);
	DDX_Control(pDX, IDC_PSISCAN, cScan);
	DDX_Control(pDX, IDC_PSISCAN5, cOK);
	DDX_Text(pDX, IDC_MUT_SCAN, cNoScan);						// 10282022 / yukchiu / multiple scan
	DDX_Control(pDX, IDC_PRIOR_STEP, m_PriorStepSize);
	DDX_Control(pDX, IDC_PRIOR_END_POS, m_PriorEndPos);
	DDX_Control(pDX, IDC_PRIOR_START_POS, m_PriorStartPos);
}

BEGIN_MESSAGE_MAP(CAcqDlg, CResizableDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_PSISCAN, &CAcqDlg::OnBnClickedPsiscan)
	ON_EN_KILLFOCUS(IDC_EDIT4, &CAcqDlg::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT12, &CAcqDlg::OnEnKillfocusEdit12)
	ON_EN_KILLFOCUS(IDC_EDIT20, &CAcqDlg::OnEnKillfocusEdit20)
	ON_COMMAND(ID_START, &CAcqDlg::OnStart)
	ON_COMMAND(ID_STOP, &CAcqDlg::OnStop)
	ON_BN_CLICKED(IDC_PSISCAN2, &CAcqDlg::OnBnClickedPsiscan2)
	ON_BN_CLICKED(IDC_PSISCAN3, &CAcqDlg::OnBnClickedPsiscan3)
	ON_BN_CLICKED(IDC_PSISCAN4, &CAcqDlg::OnBnClickedPsiscan4)
	ON_BN_CLICKED(IDC_PSISCAN5, &CAcqDlg::OnBnClickedPsiscan5)
	ON_BN_CLICKED(IDC_PSISCAN6, &CAcqDlg::OnBnClickedPsiscan6)
	ON_BN_CLICKED(IDC_PSISCAN7, &CAcqDlg::OnBnClickedPsiscan7)
	ON_COMMAND(ID_SETTING_PROPERTIES, &CAcqDlg::OnSettingProperties)
	ON_COMMAND(ID_AWB, &CAcqDlg::OnAwb)
	ON_COMMAND(ID_AEC, &CAcqDlg::OnAec)
	ON_COMMAND(ID_SETTING_VIDEOFORMAT, &CAcqDlg::OnSettingVideoformat)
	ON_COMMAND(ID_SETTING, &CAcqDlg::OnSetting)
	ON_COMMAND(ID_GRAB, &CAcqDlg::OnGrab)
	ON_BN_CLICKED(IDC_CHECK1, &CAcqDlg::OnBnClickedCheck1)
	ON_MESSAGE(UM_CLOSE, &CAcqDlg::OnUmClose)
	ON_BN_CLICKED(IDC_MUTSCAN_BUT, &CAcqDlg::OnBnClickedMutscanBut)				// 10282022 / yukchiu / multiple scan
	ON_EN_KILLFOCUS(IDC_MUT_SCAN, &CAcqDlg::OnEnKillfocusMutScan)				// 10282022 / yukchiu / multiple scan
END_MESSAGE_MAP()

void CAcqDlg::OnClose() {
	MOT::CPriorPiezo& pPiezo = Dev.MC->pPiezo;
	if (pPiezo.isConnected) {
		pPiezo.CloseSession();
	}
	MOT::CEOPiezo& EOPiezo = Dev.MC->EOPiezo;
	if (EOPiezo.isConnected) {
		EOPiezo.CloseSession();
	}
	CResizableDialog::OnClose();
}

BOOL CAcqDlg::OnInitDialog() {
	AddAnchor(IDC_CAMERA, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_PICWND, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_STATUS, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_IMAGE, TOP_LEFT, TOP_RIGHT);

	AddAnchor(IDC_GROUP, TOP_RIGHT);
	AddAnchor(IDC_STATIC6, TOP_RIGHT);
	AddAnchor(IDC_STATIC7, TOP_RIGHT);
	AddAnchor(IDC_STATIC8, TOP_RIGHT);
	AddAnchor(IDC_STATIC9, TOP_RIGHT);

	AddAnchor(IDC_CHECK1, TOP_RIGHT);

	AddAnchor(IDC_EDIT4, TOP_RIGHT);
	AddAnchor(IDC_EDIT10, TOP_RIGHT);
	AddAnchor(IDC_EDIT12, TOP_RIGHT);
	AddAnchor(IDC_EDIT20, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN2, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN3, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN4, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN5, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN6, TOP_RIGHT);
	AddAnchor(IDC_PSISCAN7, TOP_RIGHT);

	CResizableDialog::OnInitDialog();

	cLiveVid.ShowWindow(SW_HIDE);
	cPicWnd.ShowWindow(SW_SHOW);

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(ICC.bShowProgress);

	ArrangeLayout();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/*
void CAcqDlg::OnBnClickedPsiscan() {
	UpdateData(TRUE);

	CWaitCursor wc;
	cOK.EnableWindow(FALSE);
	cScan.EnableWindow(FALSE);

	CAM::CCamDev& Cam = Dev.Cam;
	CAM::CCamera* pCam = Cam.GetCamera(CAM::PRICAM);
	if (!pCam->IsValid()) { AbortOpr(1, -1); return; }

	//bool bDisp = pCam->bDisp;
	if (pCam->bIsStream) OnStop();

	cLiveVid.ShowWindow(SW_HIDE);
	cPicWnd.ShowWindow(SW_SHOW);

	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	const float wlen = ICC.SPar.Cwlen / 2.f; // wavelength unit: um [4/14/2022 yuenl]
	const float range = ICC.SPar.ZRange / 2.f;
	float iniPos = Piezo.GetPos_um();
	float stPos = iniPos - range / 2.f;
	float shift = wlen / float(ICC.SPar.NSteps);
	int nTotal = int(range / shift + 0.5f);

	CAM::SCamProp prop;
	if (!pCam->GetCamProp(prop)) { AbortOpr(2, iniPos); return; }
	if (!pCam->StartFastGrab(prop, pCam->GetSubSample())) { AbortOpr(3, iniPos); return; }

	int settleTime = 5;
	int waitTime = 500;
	Strip.DeallocAll();
	Strip.ResetTime();
	Strip.AddTime(CHighTime::GetPresentTime());
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::X, &Strip.xPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Y, &Strip.yPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Z, &Strip.zPos);
	if (!Piezo.Goto(0, false)) { AbortOpr(4, iniPos); return; }
	if (!Piezo.WaitStop(waitTime + 5000)) { AbortOpr(6, iniPos); }

	Piezo.Yld(settleTime); // setteling time
	pCam->FastGrab(Strip.ImBG, prop);
	if (ICC.bShowProgress) {
		cPicWnd.SetImg2(Strip.ImBG); cPicWnd.Invalidate(FALSE); Piezo.Yld(5);
	}

	float now = stPos;
	if (!Piezo.Goto(now, false)) { AbortOpr(4, iniPos, now); return; }
	if (!Piezo.WaitStop(waitTime + 5000)) { AbortOpr(6, iniPos); }
	Piezo.Yld(settleTime); // setteling time
	bRun = true;
	TCHAR status[256];
	DWORD tick = GetTickCount();
	for (int i = 0; i <= nTotal; i++) {
		WLI::SIms* pImN = Strip.NewImgs(now);
		if (!pImN) { AbortOpr(7, iniPos); return; }
		if (!Piezo.WaitStop(waitTime)) { AbortOpr(6, iniPos); }
		//Piezo.Yld(settleTime);
		pImN->PzPos_um = Piezo.GetPos_um();

		// time: 128ms
		if (!pCam->FastGrab(prop)) { AbortOpr(8, iniPos, float(i + 1)); return; }
		now = stPos + (i + 1) * shift;

		if (!Piezo.Goto(now, false)) { AbortOpr(4, iniPos, now); return; }

		pCam->Convert24(pImN->Im);
		if (ICC.bShowProgress) {
			cPicWnd.SetImg2(pImN->Im); cPicWnd.Invalidate(FALSE); Piezo.Yld(5);
		}
		float dur = (GetTickCount() - tick) / 1000.f;
		swprintf(status, 256,
			L"Processing %d of %d  Duration: %.2f sec  FPS: %.2f  XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
			i + 1, nTotal + 1, dur, (i + 1) / dur,
			Strip.xPos, Strip.yPos, Strip.zPos,
			now);
		cStatus.SetWindowTextW(status);
		if (!bRun) break;
	}
	pCam->StopFastGrab();
	if (!Piezo.Goto(iniPos, false)) { AbortOpr(6, iniPos); }
	Strip.AddTime(CHighTime::GetPresentTime());
	if (pCam->bIsStream) OnStart();
	cScan.EnableWindow(TRUE);

	LgS.Log(L"Success: Scan operation completed");

	// inform StripDlg
	ICC.outfile = L"Acquired";
	Strip.InitCalc();
	CWLIView::GetView()->PostMessage(UM_STRIP_LOADED, 0, 0);
	cOK.EnableWindow(TRUE);
	//CDialog::OnOK();
}
*/

//void CAcqDlg::OnBnClickedPsiscan() {
//	UpdateData(TRUE);
//
//	CWaitCursor wc;
//	cOK.EnableWindow(FALSE);
//	cScan.EnableWindow(FALSE);
//	cPicWnd.ShowWindow(SW_SHOW);
//
//	MOT::CPiezo& Piezo = Dev.MC->Piezo;
//	const float wlen = ICC.SPar.Cwlen / 2.f; // wavelength unit: um [4/14/2022 yuenl]
//	const float range = ICC.SPar.ZRange / 2.f;
//	float iniPos = Piezo.GetPos_um();
//	float stPos = iniPos - range / 2.f;
//	float shift = wlen / float(ICC.SPar.NSteps);
//	int nTotal = int(range / shift + 0.5f);
//
//	int settleTime = 5;
//	int waitTime = 500;
//	Strip.DeallocAll();
//	Strip.ResetTime();
//	Strip.AddTime(CHighTime::GetPresentTime());
//	Dev.MC->stage.devGetCurPos(MOT::MAXIS::X, &Strip.xPos);
//	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Y, &Strip.yPos);
//	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Z, &Strip.zPos);
//	if (!Piezo.Goto(0, false)) { AbortOpr(4, iniPos); return; }
//	if (!Piezo.WaitStop(waitTime + 5000)) { AbortOpr(6, iniPos); }
//
//	Piezo.Yld(settleTime); // setteling time
//	CBaslerCamDev pCam;
//	pCam.StopGrabbing();
//	pCam.SetBaslerCamProp();
//	pCam.StartContGrabbing();
//	pCam.SendSWTrigger();
//	if (Strip.ImBG.IsNull()) {
//		Strip.ImBG.Attach(gBCam.m_bitmapImage);
//	}
//
//	if (ICC.bShowProgress) {
//		cPicWnd.SetImg2(Strip.ImBG); cPicWnd.Invalidate(FALSE); Piezo.Yld(5);
//	}
//
//	float now = stPos;
//	if (!Piezo.Goto(now, false)) { AbortOpr(4, iniPos, now); return; }
//	if (!Piezo.WaitStop(waitTime + 5000)) { AbortOpr(6, iniPos); }
//	Piezo.Yld(settleTime); // setteling time
//	bRun = true;
//	TCHAR status[256];
//	DWORD tick = GetTickCount();
//	for (int i = 0; i <= nTotal; i++) {
//		//WLI::SIms* pImN = Strip.NewImgs(now);
//		WLI::SIms* pImN = new WLI::SIms;
//		pImN->PzPos_um = now;
//
//		if (!pImN) { AbortOpr(7, iniPos); return; }
//		if (!Piezo.WaitStop(waitTime)) { AbortOpr(6, iniPos); }
//		//Piezo.Yld(settleTime);
//		pImN->PzPos_um = Piezo.GetPos_um();
//
//		pCam.SendSWTrigger();
//		// time: 128ms
//		//if (!pCam->FastGrab(prop)) { AbortOpr(8, iniPos, float(i + 1)); return; }
//		now = stPos + (i + 1) * shift;
//
//		if (!Piezo.Goto(now, false)) { AbortOpr(4, iniPos, now); return; }
//
//		//pCam->Convert24(pImN->Im);
//		IMGL::CIM tmp;
//		tmp.Attach(gBCam.m_bitmapImage);
//		pImN->Im = tmp;
//		tmp.Detach();
//		Strip.Imgs.push_back(pImN);
//
//		if (ICC.bShowProgress) {
//			cPicWnd.SetImg2(pImN->Im); cPicWnd.Invalidate(FALSE); Piezo.Yld(5);
//		}
//
//		float dur = (GetTickCount() - tick) / 1000.f;
//		/*swprintf(status, 256,
//			L"Processing %d of %d  Duration: %.2f sec  FPS: %.2f  XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
//			i + 1, nTotal + 1, dur, (i + 1) / dur,
//			Strip.xPos, Strip.yPos, Strip.zPos,
//			now);*/
//
//		float AcqFPS = pCam.getbCamFPS();
//		swprintf(status, 256,
//			L"Processing %d of %d  Duration: %.2f sec  [SET FPS: %.2f] [Working FPS: %.2f] XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
//			i + 1, nTotal + 1, dur, AcqFPS, (i + 1) / dur,
//			Strip.xPos, Strip.yPos, Strip.zPos,
//			now);
//		cStatus.SetWindowTextW(status);
//		if (!bRun) break;
//	}
//	//pCam->StopFastGrab();
//	if (!Piezo.Goto(iniPos, false)) { AbortOpr(6, iniPos); }
//	Strip.AddTime(CHighTime::GetPresentTime());
//	//if (pCam->bIsStream) OnStart();
//	cScan.EnableWindow(TRUE);
//
//	LgS.Log(L"Success: Scan operation completed");
//
//	//pCam.SaveImages();
//
//	// inform StripDlg
//	ICC.outfile = L"Acquired";
//	Strip.InitCalc();
//
//	CWLIView::GetView()->PostMessage(UM_STRIP_LOADED, 0, 0);
//	cOK.EnableWindow(TRUE);
//	//CDialog::OnOK();
//}

void CAcqDlg::OnBnClickedPsiscanWithEOPiezo() {
	int settleTime = 100;
	MOT::CEOPiezo& EOPiezo = Dev.MC->EOPiezo;
	pCam.StopGrabbing();
	gBCam.isScanActive = 1;

	UpdateData(TRUE);
	CWaitCursor wc;
	cOK.EnableWindow(FALSE);
	cScan.EnableWindow(FALSE);
	cPicWnd.ShowWindow(SW_SHOW);

	if (!EOPiezo.isConnected) {
		bool stat = EOPiezo.Initialize();
		if (stat == 0) {
			AfxMessageBox(L"EO Piezo Initialization Error!!", MB_ICONERROR);
			return;
		}
		//if (!EOPiezo.MoveA(50)) { AfxMessageBox(L"Piezo Move Error!!", MB_ICONERROR); return; }
	}
	const float wlen = ICC.SPar.Cwlen / 2.f; // wavelength unit: um [4/14/2022 yuenl]
	const float range = ICC.SPar.ZRange / 2.f;
	float iniPos;
	EOPiezo.MeasureCurPos(iniPos);
	float stPos = iniPos - range / 2.f;
	float shift = wlen / float(ICC.SPar.NSteps);
	int nTotal = int(range / shift + 0.5f);

	int waitTime = 5;
	Strip.DeallocAll();
	Strip.ResetTime();
	Strip.AddTime(CHighTime::GetPresentTime());
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::X, &Strip.xPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Y, &Strip.yPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Z, &Strip.zPos);

	pCam.SetBaslerCamProp();
	pCam.StartContGrabbing();
	pCam.SendSWTrigger();

	EOPiezo.Yld(settleTime); // setteling time
	if (Strip.ImBG.IsNull()) {
		Strip.ImBG.Attach(gBCam.m_bitmapImage);
	}

	int wd, ht, bpp;
	Strip.ImBG.GetDim(wd, ht, bpp);
	if (wd != gBCam.ImWidth || ht != gBCam.ImHeight) {
		Strip.ImBG.Detach();
		Strip.ImBG.Attach(gBCam.m_bitmapImage);
	}

	if (ICC.bShowProgress) {
		cPicWnd.SetImg2(Strip.ImBG); cPicWnd.Invalidate(FALSE); EOPiezo.Yld(settleTime);
	}

	float now = stPos;
	if (!EOPiezo.MoveA(now)) { AfxMessageBox(L"Piezo Move Error!!", MB_ICONERROR); return; }
	//EOPiezo.Yld(settleTime); // setteling time
	bRun = true;

	TCHAR status[256];
	BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
	EOPiezo.Yld(settleTime);
	DWORD tick = GetTickCount();
	for (int i = 0; i <= nTotal; i++) {
		WLI::SIms* pImN = Strip.NewImgs(now);
		EOPiezo.MeasureCurPos(now);
		pImN->PzPos_um = now;
		pCam.SendSWTrigger();
		IMGL::CIM tmp;
		tmp.Attach(gBCam.m_bitmapImage);
		pImN->Im = tmp;
		tmp.Detach();
		now = stPos + (i + 1) * shift;
		if (!EOPiezo.MoveA(now)) { AfxMessageBox(L"Piezo Move Error!!", MB_ICONERROR); return; }
		if (ICC.bShowProgress) {
			cPicWnd.SetImg2(pImN->Im); cPicWnd.Invalidate(FALSE);
		}
		//EOPiezo.Yld(2);

		float dur = (GetTickCount() - tick) / 1000.f;
		swprintf(status, 256,
			L"Processing %d of %d  Duration: %.2f sec [Working FPS: %.2lf] XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
			i + 1, nTotal + 1, dur, (i + 1) / dur,
			Strip.xPos, Strip.yPos, Strip.zPos,
			now);
		cStatus.SetWindowTextW(status);
		if (!bRun) break;
	}

	BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	pCam.StopGrabbing();
	EOPiezo.Yld(settleTime);
	gBCam.isScanActive = 0;
	pCam.StartContGrabbing();
	if (!EOPiezo.MoveA(iniPos)) { AfxMessageBox(L"Piezo Move Error!!", MB_ICONERROR); }
	Strip.AddTime(CHighTime::GetPresentTime());
	cScan.EnableWindow(TRUE);

	LgS.Log(L"Success: Scan operation completed");

	ICC.outfile = L"Acquired";
	Strip.InitCalc();
	CWLIView::GetView()->PostMessage(UM_STRIP_LOADED, 0, 0);
	cOK.EnableWindow(TRUE);
}

void CAcqDlg::OnBnClickedPsiscanWithPriorPiezo() {
	UpdateData(TRUE);

	CWaitCursor wc;
	cOK.EnableWindow(FALSE);
	cScan.EnableWindow(FALSE);
	cPicWnd.ShowWindow(SW_SHOW);

	MOT::CPriorPiezo& pPiezo = Dev.MC->pPiezo;

	Strip.DeallocAll();
	Strip.ResetTime();
	Strip.AddTime(CHighTime::GetPresentTime());
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::X, &Strip.xPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Y, &Strip.yPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Z, &Strip.zPos);

	pPiezo.Yld(5);

	CBaslerCamDev pCam;
	pCam.StopGrabbing();
	pCam.SetBaslerCamProp();
	pCam.StartContGrabbing();
	pCam.SendSWTrigger();

	if (Strip.ImBG.IsNull()) {
		Strip.ImBG.Attach(gBCam.m_bitmapImage);
	}

	int wd, ht, bpp;
	Strip.ImBG.GetDim(wd, ht, bpp);
	if (wd != gBCam.ImWidth || ht != gBCam.ImHeight) {
		Strip.ImBG.Detach();
		Strip.ImBG.Attach(gBCam.m_bitmapImage);
	}

	if (ICC.bShowProgress) {
		cPicWnd.SetImg2(Strip.ImBG); cPicWnd.Invalidate(FALSE);
	}

	TCHAR status[256];
	DWORD tick = GetTickCount();

	double stepSize, startPos, endPos;
	CString str = L"";
	m_PriorStepSize.GetWindowTextW(str);
	stepSize = _wtof(str);
	m_PriorEndPos.GetWindowTextW(str);
	endPos = _wtof(str);
	m_PriorStartPos.GetWindowTextW(str);
	startPos = _wtof(str);

	if (startPos < 1) {
		AfxMessageBox(L"Start Pos Must Greater Than 0!!!", MB_ICONINFORMATION);
		return;
	}
	else if (endPos > 400) {
		AfxMessageBox(L"End Pos Must Less than 401!!!", MB_ICONINFORMATION);
		return;
	}
	else if (startPos > endPos) {
		AfxMessageBox(L"End Pos Must Greater Than Start Pos!!!", MB_ICONINFORMATION);
		return;
	}

	float now;
	if (!pPiezo.isConnected) {
		pPiezo.Initialize();
	}

	pPiezo.MeasureCurPos(now);
	int xTime = 1;
	while (xTime++ <= 5) {
		pPiezo.MoveR(startPos - now);
		Sleep(10);
		pPiezo.MeasureCurPos(now);
	}

	int nTotal = (endPos - startPos + 1) / stepSize;
	double totMove = 0.0;
	//BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	for (int i = 1; i <= nTotal; i++) {
		WLI::SIms* pImN = Strip.NewImgs(now);
		//pPiezo.Yld(5);
		pPiezo.MeasureCurPos(now);
		pImN->PzPos_um = now;

		pCam.SendSWTrigger();
		IMGL::CIM tmp;
		tmp.Attach(gBCam.m_bitmapImage);
		pImN->Im = tmp;
		tmp.Detach();

		totMove += stepSize;
		if (!pPiezo.MoveR(stepSize)) { AfxMessageBox(L"Move Error!!!", MB_ICONERROR); pPiezo.MoveR(-totMove); return; } // um

		if (ICC.bShowProgress) {
			cPicWnd.SetImg2(pImN->Im);
			cPicWnd.Invalidate(FALSE);
			pPiezo.Yld(2);
		}

		float dur = (GetTickCount() - tick) / 1000.f;

		float AcqFPS = pCam.getbCamFPS();
		double wFPS = pCam.resultingFPS();
		/*swprintf(status, 256,
			L"Processing %d of %d  Duration: %.2f sec  [SET FPS: %.2f] [Working FPS: %.2f] XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
			i + 1, nTotal + 1, dur, AcqFPS, (i + 1) / dur,
			Strip.xPos, Strip.yPos, Strip.zPos,
			now);*/
		swprintf(status, 256,
			L"Processing %d of %d  Duration: %.2f sec [Working FPS: %.2lf] XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
			i + 1, nTotal, dur, (i + 1) / dur,
			Strip.xPos, Strip.yPos, Strip.zPos,
			now);
		cStatus.SetWindowTextW(status);
	}

	BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);

	pPiezo.MoveR(-totMove);
	//pPiezo.CloseSession();
	Strip.AddTime(CHighTime::GetPresentTime());
	cScan.EnableWindow(TRUE);

	LgS.Log(L"Success: Scan operation completed");

	ICC.outfile = L"Acquired";
	Strip.InitCalc();

	CWLIView::GetView()->PostMessage(UM_STRIP_LOADED, 0, 0);
	cOK.EnableWindow(TRUE);
}

#include <fstream>

void CAcqDlg::OnBnClickedPsiscan() {
	// [ For PRIOR SCAN TEST -----------------
	int isPriorPiezo = DosUtil.ReadCfgINI(L"Piezo", L"PriorPiezo", 1);
	if (isPriorPiezo == 1) {
		OnBnClickedPsiscanWithPriorPiezo();
		return;
	}
	else if (isPriorPiezo == 2) { // EO Piezo
		OnBnClickedPsiscanWithEOPiezo();
		return;
	}
	// For SIMU ] ----------------------------

	//std::ofstream myFile("C:/WLI/piezopos.txt");

	UpdateData(TRUE);

	CWaitCursor wc;
	cOK.EnableWindow(FALSE);
	cScan.EnableWindow(FALSE);
	cPicWnd.ShowWindow(SW_SHOW);

	MOT::CPiezo& Piezo = Dev.MC->Piezo;
	const float wlen = ICC.SPar.Cwlen / 2.f; // wavelength unit: um [4/14/2022 yuenl]
	const float range = ICC.SPar.ZRange / 2.f;
	float iniPos = Piezo.GetPos_um();
	float stPos = iniPos - range / 2.f;
	float shift = wlen / float(ICC.SPar.NSteps);
	int nTotal = int(range / shift + 0.5f);

	int settleTime = 5;
	int waitTime = 5;
	Strip.DeallocAll();
	Strip.ResetTime();
	Strip.AddTime(CHighTime::GetPresentTime());
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::X, &Strip.xPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Y, &Strip.yPos);
	Dev.MC->stage.devGetCurPos(MOT::MAXIS::Z, &Strip.zPos);
	if (!Piezo.Goto(0, false)) { AbortOpr(4, iniPos); return; }
	if (!Piezo.WaitStop(waitTime + 5000)) { AbortOpr(6, iniPos); }

	//Piezo.Yld(settleTime); // setteling time
	//CBaslerCamDev pCam;
	//pCam.StopGrabbing();
	//pCam.SetBaslerCamProp();
	//pCam.StartContGrabbing();
	//pCam.SendSWTrigger();

	while (!Dev.Cam.Grab(Strip.ImBG, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));
	if (Strip.ImBG) {
		if (ICC.bShowProgress) {
			cPicWnd.SetImg2(Strip.ImBG); cPicWnd.Invalidate(FALSE); Piezo.Yld(5);
		}
		Strip.ImBG.Detach();
	}

	float now = stPos;
	if (!Piezo.Goto(now, false)) { AbortOpr(4, iniPos, now); return; }
	if (!Piezo.WaitStop(waitTime)) { AbortOpr(6, iniPos); }
	Piezo.Yld(settleTime); // setteling time
	bRun = true;

	TCHAR status[256];

	//BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	//double FPs = BaslerCameraDlg::m_camera.AcquisitionFrameRate.GetValue();
	DWORD tick = GetTickCount();
	IMGL::CIM tmp;
	for (int i = 0; i <= nTotal; i++) {
		WLI::SIms* pImN = Strip.NewImgs(now);
		while (!Dev.Cam.Grab(tmp, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling));
		pImN->Im = tmp;
		tmp.Detach();
		now = stPos + (i + 1) * shift;
		if (!Piezo.Goto(now, false)) { AfxMessageBox(L"Piezo Move Error!", MB_ICONERROR); return; }
		if (ICC.bShowProgress) {
			cPicWnd.SetImg2(pImN->Im); cPicWnd.Invalidate(FALSE);
		}
		Piezo.Yld(3);

		float dur = (GetTickCount() - tick) / 1000.f;
		swprintf(status, 256,
			L"Processing %d of %d  Duration: %.2f sec [Working FPS: %.2lf] XYZ:(%.4f, %.4f, %.4f) um  P:%.4f um",
			i + 1, nTotal + 1, dur, (i + 1) / dur,
			Strip.xPos, Strip.yPos, Strip.zPos,
			now);
		cStatus.SetWindowTextW(status);
		if (!bRun) break;
	}

	//Piezo.Yld(15);
	//BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);

	//pCam->StopFastGrab();
	if (!Piezo.Goto(iniPos, false)) { AbortOpr(6, iniPos); }
	Strip.AddTime(CHighTime::GetPresentTime());
	//if (pCam->bIsStream) OnStart();
	cScan.EnableWindow(TRUE);

	LgS.Log(L"Success: Scan operation completed");

	//pCam.SaveImages();

	// inform StripDlg
	ICC.outfile = L"Acquired";
	//ICC.x1 = 10, ICC.y1 = 10, ICC.x2 = 10, ICC.y2 = 10; // 07252023
	Strip.InitCalc();
	//recipeItems->SaveImageData(); // 07252023

	CWLIView::GetView()->PostMessage(UM_STRIP_LOADED, 0, 0);
	cOK.EnableWindow(TRUE);
	//CDialog::OnOK();
}

void CAcqDlg::OnEnKillfocusEdit4() {
	UpdateData(TRUE);
}

void CAcqDlg::OnEnKillfocusEdit12() {
	UpdateData(TRUE);
}

void CAcqDlg::OnEnKillfocusEdit20() {
	UpdateData(TRUE);
}

void CAcqDlg::OnStart() {
	cPicWnd.ShowWindow(SW_HIDE);
	cLiveVid.ShowWindow(SW_SHOW);
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	Ctx.hWnd = cLiveVid.GetSafeHwnd();
	cLiveVid.GetClientRect(Ctx.rc);
	pCam->StartStream(Ctx, pCam->SCaM.ID);
}

void CAcqDlg::OnStop() {
	cLiveVid.ShowWindow(SW_HIDE);
	cPicWnd.ShowWindow(SW_SHOW);
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	Ctx.hWnd = cLiveVid.GetSafeHwnd();
	cLiveVid.GetClientRect(Ctx.rc);
	pCam->StopStream(Ctx, pCam->SCaM.ID);
}

void CAcqDlg::ActivateButton(bool bActv) {
	// to be continues
}

void CAcqDlg::OnBnClickedPsiscan2() {
	ActivateButton(false);
	pWLIView->ShowMoSetup(GetSafeHwnd());
}

void CAcqDlg::OnBnClickedPsiscan3() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling)) {
		//Im.SavePic();
		cPicWnd.SetImg2(Im); cPicWnd.Invalidate(FALSE);
	}
}

void CAcqDlg::OnBnClickedPsiscan4() {
	bRun = false;
}

void CAcqDlg::OnBnClickedPsiscan5() {
	if (recipeItems->RecipePoints.size() > 0) {
		//recipeItems->MoveXYPos(); // 07252023
	}
	CDialog::OnOK();
}

void CAcqDlg::OnBnClickedPsiscan6() {
	// TODO: Add your control notification handler code here
}

void CAcqDlg::OnBnClickedPsiscan7() {
	// TODO: Add your control notification handler code here
}

void CAcqDlg::OnSettingProperties() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->LucamPropPage();
}

void CAcqDlg::OnAwb() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->WhiteBalance();
}

void CAcqDlg::OnAec() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->AutoExposure();
}

void CAcqDlg::OnSettingVideoformat() {
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM); if (!pCam) return;
	pCam->LucamVideoFormat();
}

void CAcqDlg::OnSetting() {
	CCamSetDlg dlg; dlg.DoModal();
}

void CAcqDlg::OnGrab() {
	IMGL::CIM Im;
	if (Dev.Cam.Grab(Im, CAM::PRICAM, Dev.Cam.pCm[CAM::PRICAM]->subSampling)) {
		Im.SavePic();
	}
}

void CAcqDlg::OnBnClickedCheck1() {
	ICC.bShowProgress = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
}

afx_msg LRESULT CAcqDlg::OnUmClose(WPARAM wParam, LPARAM lParam) {
	ActivateButton(true);
	return 0;
}

// 10282022 / yukchiu / multiple scan
void CAcqDlg::OnBnClickedMutscanBut() {
	int num;
	CString filename, szTime, count, temp;
	CFile theFile;
	CTime time;
	int i;

	temp = L"C:\\WLI\\IMS\\";
	time = CTime::GetCurrentTime();
	szTime = time.Format(L"%m_%d_%Y_%H%M%S_");

	GetDlgItemText(IDC_MUT_SCAN, numScan);  // get data from the text
	num = _tstoi(numScan);

	for (i = 1; i < (num + 1); i++) {
		count.Format(L"%d", i);
		filename = temp + szTime + count + L".IMS";
		OnBnClickedPsiscan();
		Strip.Save(filename.GetBuffer(0));
	}
}

void CAcqDlg::OnEnKillfocusMutScan() {
	UpdateData(TRUE);
}
// ==================================