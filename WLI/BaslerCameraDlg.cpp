// BaslerCameraDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "BaslerCameraDlg.h"
#include "afxdialogex.h"
#include <afxwin.h>
#include<pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include<pylon/PylonGUI.h>
#endif
#include<string>
#include"BaslerCamDev.h"

using namespace Pylon;
using namespace std;
// BaslerCameraDlg dialog

IMPLEMENT_DYNAMIC(BaslerCameraDlg, CResizableDialog)

Pylon::CBaslerUniversalInstantCamera BaslerCameraDlg::m_camera;

BaslerCameraDlg::BaslerCameraDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_DIALOG15, pParent) {}

BaslerCameraDlg::~BaslerCameraDlg() {}

void BaslerCameraDlg::DoDataExchange(CDataExchange* pDX) {
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERA_LIST, m_CameraDeviceListCtrl);
	DDX_Control(pDX, IDC_EXPOSURE_SLIDER, m_ctrlExposureSlider);
	DDX_Control(pDX, IDC_EXPOSURE, m_ctrlExposureText);
	DDX_Control(pDX, IDC_GAIN_SLIDER, m_ctrlGainSlider);
	DDX_Control(pDX, IDC_GAIN, m_ctrlGainText);
	DDX_Control(pDX, IDC_TRIG_SELECTOR, m_triggerSelector);
	DDX_Control(pDX, IDC_TRIG_MODE, m_triggerMode);
	DDX_Control(pDX, IDC_TRIG_SOURCE, m_triggerSource);
	DDX_Control(pDX, IDC_IMAGE_WIND, cPicWnd);
	DDX_Control(pDX, IDC_COMB_BAT, m_AutoWB);
	DDX_Control(pDX, IDC_COMB_EXPOAUTO, m_AutoExpos);
	DDX_Control(pDX, IDC_IM_WIDTH, m_ImWidth);
	DDX_Control(pDX, IDC_IM_HEIGHT, m_ImHeight);
	DDX_Control(pDX, IDC_IM_FRAME_RATE, m_ImFrame);
}

BEGIN_MESSAGE_MAP(BaslerCameraDlg, CResizableDialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()
	ON_WM_HSCROLL()
	ON_COMMAND(ID_REFRESH, &BaslerCameraDlg::OnViewRefresh)
	ON_COMMAND(ID_CAM_GRAB_ONE, &BaslerCameraDlg::OnGrabOne)
	ON_UPDATE_COMMAND_UI(ID_CAM_GRAB_ONE, &BaslerCameraDlg::OnUpdateGrabOne)
	ON_COMMAND(ID_CAMERA_GRABCONTINUOUS, &BaslerCameraDlg::OnStartGrabbing)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_GRABCONTINUOUS, &BaslerCameraDlg::OnUpdateStartGrabbing)
	ON_COMMAND(ID_STOP_GRABBING, &BaslerCameraDlg::OnStopGrab)
	ON_UPDATE_COMMAND_UI(ID_STOP_GRABBING, &BaslerCameraDlg::OnUpdateStopGrab)
	ON_BN_CLICKED(ID_CAM_GRAB_ONE, &BaslerCameraDlg::OnBnClickedSingleShot)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CAMERA_LIST, &BaslerCameraDlg::OnLvnItemchangedCameraList)
	ON_CBN_SELCHANGE(IDC_TRIG_SELECTOR, &BaslerCameraDlg::OnCbnSelchangeTrigSelector)
	ON_CBN_SELCHANGE(IDC_TRIG_MODE, &BaslerCameraDlg::OnCbnSelchangeTrigMode)
	ON_CBN_SELCHANGE(IDC_TRIG_SOURCE, &BaslerCameraDlg::OnCbnSelchangeTrigSource)
	ON_BN_CLICKED(IDC_EXEC_BTN, &BaslerCameraDlg::OnBnClickedExecBtn)
	ON_CBN_SELCHANGE(IDC_COMB_BAT, &BaslerCameraDlg::OnCbnSelchangeCombBat)
	ON_CBN_SELCHANGE(IDC_COMB_EXPOAUTO, &BaslerCameraDlg::OnCbnSelchangeCombExpoauto)
END_MESSAGE_MAP()

// BaslerCameraDlg message handlers
void BaslerCameraDlg::OnClose() {
	//OnStop();
	DeleteContents();
	CResizableDialog::OnClose();
}

void BaslerCameraDlg::OnDestroy() {
	CResizableDialog::OnDestroy();
}

BOOL BaslerCameraDlg::OnInitDialog() {
	CResizableDialog::OnInitDialog();

	AddAnchor(IDC_IMAGE_WIND, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_STATIC_IMG_CNT, TOP_RIGHT);
	AddAnchor(IDC_FRAME_CNT, TOP_RIGHT);
	AddAnchor(ID_CAM_GRAB_ONE, TOP_RIGHT);
	AddAnchor(IDC_CAMERA_LIST, TOP_RIGHT);
	AddAnchor(IDC_STATIC_GRP_PROP, TOP_RIGHT);
	AddAnchor(IDC_STATIC_EXPOS, TOP_RIGHT);
	AddAnchor(IDC_EXPOSURE, TOP_RIGHT);
	AddAnchor(IDC_EXPOSURE_SLIDER, TOP_RIGHT);
	AddAnchor(IDC_STATIC_GAIN, TOP_RIGHT);
	AddAnchor(IDC_GAIN, TOP_RIGHT);
	AddAnchor(IDC_GAIN_SLIDER, TOP_RIGHT);
	AddAnchor(IDC_STATIC_TS, TOP_RIGHT);
	AddAnchor(IDC_TRIG_SELECTOR, TOP_RIGHT);
	AddAnchor(IDC_STATIC_GAIN, TOP_RIGHT);
	AddAnchor(IDC_STATIC_TM, TOP_RIGHT);
	AddAnchor(IDC_TRIG_MODE, TOP_RIGHT);
	AddAnchor(IDC_STATIC_TSRC, TOP_RIGHT);
	AddAnchor(IDC_TRIG_SOURCE, TOP_RIGHT);
	AddAnchor(IDC_STATIC_ST, TOP_RIGHT);
	AddAnchor(IDC_EXEC_BTN, TOP_RIGHT);
	AddAnchor(IDC_STATIC_BAT, TOP_RIGHT);
	AddAnchor(IDC_COMB_BAT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_EXPOAUTO, TOP_RIGHT);
	AddAnchor(IDC_COMB_EXPOAUTO, TOP_RIGHT);
	AddAnchor(IDC_STATIC_IMWIDTH, TOP_RIGHT);
	AddAnchor(IDC_IM_WIDTH, TOP_RIGHT);
	AddAnchor(IDC_STATIC_IMHEIGHT, TOP_RIGHT);
	AddAnchor(IDC_IM_HEIGHT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_IMFRAMERATE, TOP_RIGHT);
	AddAnchor(IDC_IM_FRAME_RATE, TOP_RIGHT);

	/*try {
		m_camera.RegisterImageEventHandler(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
		m_camera.RegisterConfiguration(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
	}
	catch (const Pylon::GenericException& e) {
		TRACE(CUtf82W(e.what()));
		return FALSE;
		UNUSED(e);
	}*/

	if (string(CT2A(camName)).size() > 0) {
		OpenCamera(camName);
	}

	m_ImWidth.SetWindowTextW(L"1280");
	m_ImHeight.SetWindowTextW(L"1024");
	m_ImFrame.SetWindowTextW(L"10");

	gBCam.ImWidth = 1280;
	gBCam.ImHeight = 1024;
	gBCam.ImFrameRate = 10;

	ArrangeLayout();
	OnViewRefresh();

	return TRUE;
}

void BaslerCameraDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) {
	// Make sure this is actually a menu. When clicking the program icon
	// in the window title bar this function will trigger and pPopupMenu
	// will NOT be a menu.
	if (!IsMenu(pPopupMenu->m_hMenu))
		return;

	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu; // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL) {
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL) {
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) {
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu) {
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++) {
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1) {
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1) {
				continue; // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE); // Popups are never auto disabled.
		}
		else {
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax) {
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID) {
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void BaslerCameraDlg::OnStart() {
	SCtx Ctx;
	Ctx.hWnd = cDisplayImage.GetSafeHwnd();
	cDisplayImage.GetClientRect(Ctx.rc);
	//pCam->StartStream(Ctx, pCam->SCaM.ID);
}

void BaslerCameraDlg::OnStop() {
	/*SCtx Ctx;
	Ctx.hWnd = cDisplayImage.GetSafeHwnd();
	cDisplayImage.GetClientRect(Ctx.rc);*/
	//pCam->StopStream(Ctx, pCam->SCaM.ID);
}
static const uint32_t c_countOfImagesToGrab = 100;

void BaslerCameraDlg::OnViewRefresh() {
	try {
		// Refresh the list of all attached cameras.
		EnumerateDevices();

		// Always update the device list and the image
		EUpdateHint hint = EUpdateHint(UpdateHint_DeviceList | UpdateHint_Image);

		if (m_camera.IsPylonDeviceAttached()) {
			hint = EUpdateHint(hint | UpdateHint_Feature);
		}
		else {
			CString s;
			s.LoadString(AFX_IDS_UNTITLED);
			//SetTitle(s);
		}
	}
	catch (const Pylon::GenericException& e) {
		TRACE(_T("Error during Refresh: %s"), (LPCWSTR)CUtf82W(e.what()));
		UNUSED(e);
	}
}

void Dispatch(HWND hw) {
	MSG msg;

	while (PeekMessage(&msg, hw, 0, 0xFFFF, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};
};

void Delay(HWND hw, int ms) {
	__int64 l, l1;

	l = GetTickCount64();
	do {
		Dispatch(hw);
		l1 = GetTickCount64() - l;
	} while (l1 < ms);
};

int  CameraList_GetCount() {
	Pylon::DeviceInfoList_t ListCameras;

	int iRet = (int)ListCameras.size();
	if (iRet == 0) {
		Delay(HWND_BROADCAST, 200);
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		ListCameras.clear();
		tlFactory.EnumerateDevices(ListCameras);
		iRet = (int)ListCameras.size();
	}
	if (iRet == 0) {
		Delay(HWND_BROADCAST, 500);
		CTlFactory& tlFactory = CTlFactory::GetInstance();
		ListCameras.clear();
		tlFactory.EnumerateDevices(ListCameras);
		iRet = (int)ListCameras.size();
	}

	return iRet;
}

int BaslerCameraDlg::EnumerateDevices() {
	Pylon::DeviceInfoList_t devices;

	try {
		// Get the transport layer factory.
		Pylon::CTlFactory& TlFactory = Pylon::CTlFactory::GetInstance();

		// Get all attached cameras.
		TlFactory.EnumerateDevices(devices);
	}
	catch (const Pylon::GenericException& e) {
		UNUSED(e);
		devices.clear();

		TRACE(CUtf82W(e.what()));
	}
	int size = (int)devices.size();
	m_devices = devices;
	FillDeviceListCtrl(devices);
	return (int)m_devices.size();
}

void BaslerCameraDlg::FillDeviceListCtrl(Pylon::DeviceInfoList_t devices) {
	const Pylon::String_t fullNameSelected = m_fullNameSelected;

	m_updatingList = TRUE;

	// Delete the device list items before refilling.
	m_CameraDeviceListCtrl.DeleteAllItems();

	if (!devices.empty()) {
		int i = 0;
		for (Pylon::DeviceInfoList_t::const_iterator it = devices.begin(); it != devices.end(); ++it) {
			// Get the pointer to the current device info.
			const Pylon::CDeviceInfo* const pDeviceInfo = &(*it);
			LPCTSTR str = CUtf82W(pDeviceInfo->GetFriendlyName());
			// Add the item to the list.
			int nItem = m_CameraDeviceListCtrl.InsertItem(i++, CUtf82W(pDeviceInfo->GetFriendlyName()));

			// Remember the pointer to the device info.
			m_CameraDeviceListCtrl.SetItemData(nItem, (DWORD_PTR)pDeviceInfo);

			// Restore selection if necessary.
			if (pDeviceInfo->GetFullName() == fullNameSelected) {
				m_CameraDeviceListCtrl.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
				m_CameraDeviceListCtrl.SetSelectionMark(nItem);
			}
		}
	}

	m_updatingList = FALSE;
}

void BaslerCameraDlg::OnBnClickedSingleShot() {
	if (!m_camera.IsOpen() || m_camera.IsGrabbing()) {
		AfxMessageBox(L"Camera Not Open Or Another Instance Running");
		return;
	}
	cntFrame = 0;
	m_singleConfiguration.ApplyConfiguration(m_camera.GetNodeMap());
	m_camera.StartGrabbing(1, Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}

void BaslerCameraDlg::OnGrabOne() {
	if (!m_camera.IsOpen() || m_camera.IsGrabbing()) {
		AfxMessageBox(L"Camera Not Open Or Another Instance Running");
		return;
	}
	cntFrame = 0;
	m_singleConfiguration.ApplyConfiguration(m_camera.GetNodeMap());
	m_camera.StartGrabbing(1, Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}

void BaslerCameraDlg::OnUpdateGrabOne(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_camera.IsOpen() && !m_camera.IsGrabbing());
}

void BaslerCameraDlg::OnStartGrabbing() {
	// Camera may have been disconnected.
	if (!m_camera.IsOpen() || m_camera.IsGrabbing()) {
		AfxMessageBox(L"Camera Not Open Or Another Instance Running");
		return;
	}
	cntFrame = 0;
	CString str = L"";
	m_ImWidth.GetWindowTextW(str);
	gBCam.ImWidth = stoi(string(CT2A(str)));
	m_ImHeight.GetWindowTextW(str);
	gBCam.ImHeight = stoi(string(CT2A(str)));
	m_ImFrame.GetWindowTextW(str);
	gBCam.ImFrameRate = stoi(string(CT2A(str)));
	m_ImWidth.EnableWindow(0);
	m_ImHeight.EnableWindow(0);
	m_ImFrame.EnableWindow(0);
	gBCam.StopGrabbing();
	gBCam.SetBaslerCamProp();
	//m_camera.AcquisitionMode.SetValue(Basler_UniversalCameraParams::AcquisitionMode_Continuous);
	m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
}

void BaslerCameraDlg::OnUpdateStartGrabbing(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_camera.IsOpen() && !m_camera.IsGrabbing());
}

void BaslerCameraDlg::OnStopGrab() {
	// Camera may have been disconnected.
	if (m_camera.IsGrabbing()) {
		m_camera.StopGrabbing();
		m_ImWidth.EnableWindow();
		m_ImHeight.EnableWindow();
		m_ImFrame.EnableWindow();
	}
}

void BaslerCameraDlg::OnUpdateStopGrab(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_camera.IsOpen() && m_camera.IsGrabbing());
}

void BaslerCameraDlg::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult) {
	CSingleLock lock(&m_MemberLock, TRUE);
	m_ptrGrabResult = grabResult;

	if (m_ptrGrabResult.IsValid() && m_ptrGrabResult->GrabSucceeded()) {
		if (gBCam.isScanActive) {
			gBCam.m_bitmapImage.CopyImage(m_ptrGrabResult);
		}
		else {
			if (cntFrame < 10000000)
				cntFrame++;
			else cntFrame = 1;
			TRACE("FPS = %d\n", cntFrame);
			CString str;
			int imWidth = m_camera.Width.GetValue();
			int imHeight = m_camera.Height.GetValue();
			int imFrameRate = m_camera.AcquisitionFrameRate.GetValue();
			str.Format(L"%d, W=%d, H=%d", cntFrame, imWidth, imHeight);
			SetDlgItemText(IDC_FRAME_CNT, str);
			m_bitmapImage.CopyImage(m_ptrGrabResult);
			tmpBitmapImage.Attach(m_bitmapImage);
			cPicWnd.SetImg2(tmpBitmapImage);
			cPicWnd.Invalidate(FALSE);
			tmpBitmapImage.Detach();
		}
	}

	lock.Unlock();
}

BOOL BaslerCameraDlg::OpenCamera(LPCTSTR lpszPathName) {
	// Make sure everything is closed.
	DeleteContents();

	try {
		m_camera.RegisterImageEventHandler(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
		m_camera.RegisterConfiguration(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
	}
	catch (const Pylon::GenericException& e) {
		TRACE(CUtf82W(e.what()));
		return FALSE;
		UNUSED(e);
	}

	ASSERT(!m_camera.IsPylonDeviceAttached());

	try {
		// Add the AutoPacketSizeConfiguration and let pylon delete it when not needed anymore.
		//m_camera.RegisterConfiguration(new CAutoPacketSizeConfiguration(), Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);

		// Create the device and attach it to CInstantCamera.
		// Let CInstantCamera take care of destroying the device.
		const Pylon::String_t strDeviceFullName = GetString_t(lpszPathName);
		Pylon::IPylonDevice* pDevice = Pylon::CTlFactory::GetInstance().CreateDevice(strDeviceFullName);
		m_camera.Attach(pDevice, Pylon::Cleanup_Delete);

		// Open camera.
		m_camera.Open();

		//BaslerCameraDlg::m_camera.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_RGB8);

		// Get the Exposure Time feature.
		// On GigE cameras, the feature is named 'ExposureTimeRaw'.
		// On USB cameras, it is named 'ExposureTime'.
		m_exposureTime = GetIntegerParameter(m_camera.GetNodeMap().GetNode("ExposureTime"));
		if (!m_exposureTime.IsValid()) {
			m_exposureTime = GetIntegerParameter(m_camera.GetNodeMap().GetNode("ExposureTimeRaw"));
		}
		if (m_exposureTime.IsValid()) {
			//Add a callback that triggers the update.
			m_hExposureTime = GenApi::Register(m_exposureTime.GetNode(), *this, &BaslerCameraDlg::OnNodeChanged);
		}

		/*Get the Gain feature.
		On GigE cameras, the feature is named 'GainRaw'.
		On USB cameras, it is named 'Gain'.*/
		m_gain = GetIntegerParameter(m_camera.GetNodeMap().GetNode("Gain"));
		if (!m_gain.IsValid()) {
			m_gain = GetIntegerParameter(m_camera.GetNodeMap().GetNode("GainRaw"));
		}
		if (m_gain.IsValid()) {   // Add a callback that triggers the update.
			m_hGain = GenApi::Register(m_gain.GetNode(), *this, &BaslerCameraDlg::OnNodeChanged);
		}

		/*
		// Get the Test Image Selector feature.
		// The feature is called 'TestImageSelector' on SFNC 1.x cameras and 'TestPattern' on SFNC 2.x cameras.
		m_testImage.Attach(m_camera.GetNodeMap(), "TestImageSelector");
		if (!m_testImage.IsValid()) {
			m_testImage.Attach(m_camera.GetNodeMap(), "TestPattern");
		}
		if (m_testImage.IsValid()) {
			// Add a callback that triggers the update.
			m_hTestImage = GenApi::Register(m_testImage.GetNode(), *this, &BaslerCameraDlg::OnNodeChanged);
		}

		// Get the Pixel Format feature.
		m_pixelFormat.Attach(m_camera.GetNodeMap(), "PixelFormat");
		if (m_pixelFormat.IsValid()) {
			// Add a callback that triggers the update.
			m_hPixelFormat = GenApi::Register(m_pixelFormat.GetNode(), *this, &BaslerCameraDlg::OnNodeChanged);
		}

		*/

		// Clear everything.
		CSingleLock lock(&m_MemberLock, TRUE);
		m_ptrGrabResult.Release();
		m_bitmapImage.Release();

		return TRUE;
	}
	catch (const Pylon::GenericException& e) {
		UNUSED(e);
		TRACE(CUtf82W(e.what()));

		// Something went wrong. Make sure we don't leave the document object partially initialized.
		DeleteContents();

		return FALSE;
	}
}

Pylon::CIntegerParameter BaslerCameraDlg::GetIntegerParameter(GenApi::INode* pNode) const {
	Pylon::CFloatParameter parameterFloat(pNode);
	Pylon::CIntegerParameter parameterInteger;

	// Is this a float feature?
	if (parameterFloat.IsValid()) {
		// Get the alias.
		// If it doesn't exist, return an empty parameter.
		parameterFloat.GetAlternativeIntegerRepresentation(parameterInteger);
	}
	else {
		// Convert the node to an integer parameter.
		// If conversion isn't possible, return an empty parameter.
		parameterInteger.Attach(pNode);
	}

	return parameterInteger;
}

// Called when a node was possibly changed
void BaslerCameraDlg::OnNodeChanged(GenApi::INode* pNode) {
	if (pNode == NULL) {
		return;
	}

	UpdateSlider(&m_ctrlExposureSlider, m_exposureTime);
	UpdateSliderText(&m_ctrlExposureText, m_exposureTime);
	UpdateSlider(&m_ctrlGainSlider, m_gain);
	UpdateSliderText(&m_ctrlGainText, m_gain);
}

// Called to update value of slider.
void BaslerCameraDlg::UpdateSlider(CSliderCtrl* pCtrl, Pylon::CIntegerParameter& integerParameter) {
	if (integerParameter.IsReadable()) {
		int64_t minimum = integerParameter.GetMin();
		int64_t maximum = integerParameter.GetMax();
		int64_t value = integerParameter.GetValue();

		if (maximum == 1000000) maximum = 10000;
		// Possible loss of data because Windows controls only supports
		// 32-bitness while GenApi supports 64-bitness.
		pCtrl->SetRange(static_cast<int>(minimum), static_cast<int>(maximum));
		pCtrl->SetPos(static_cast<int>(value));
	}
	pCtrl->EnableWindow(integerParameter.IsWritable());
}

// Called to update the value of a label.
void BaslerCameraDlg::UpdateSliderText(CStatic* pString, Pylon::CIntegerParameter& integerParameter) {
	if (integerParameter.IsValid() && integerParameter.IsReadable()) {
		// Set the value as a string in wide character format.
		pString->SetWindowText(CUtf82W(integerParameter.ToString().c_str()));
	}
	else {
		pString->SetWindowText(_T("N/A"));
	}
	pString->EnableWindow(integerParameter.IsWritable());
}

void BaslerCameraDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	// TODO: Add your message handler code here and/or call default
	nPos = OnScroll(pScrollBar, &m_ctrlExposureSlider, m_exposureTime);
	if (m_gain.IsValid()) {
		nPos = OnScroll(pScrollBar, &m_ctrlGainSlider, m_gain);
	}
	__super::OnHScroll(nSBCode, nPos, pScrollBar);
}

// Round a value to a valid value
int64_t RoundTo(int64_t newValue, int64_t oldValue, int64_t minimum, int64_t maximum, int64_t increment) {
	const int Direction = (newValue - oldValue) > 0 ? 1 : -1;
	const int64_t nIncr = (newValue - minimum) / increment;

	switch (Direction) {
	case 1: // Up
		return min(maximum, minimum + nIncr * increment);
	case -1: // Down
		return max(minimum, minimum + nIncr * increment);
	}
	return newValue;
}

// Update a slider and set a valid value.
UINT BaslerCameraDlg::OnScroll(CScrollBar* pScrollBar, CSliderCtrl* pCtrl, Pylon::CIntegerParameter& integerParameter) {
	if (pScrollBar->GetSafeHwnd() == pCtrl->GetSafeHwnd()) {
		if (integerParameter.IsWritable()) {
			// Fetch current value, range, and increment of the camera feature.
			int64_t value = integerParameter.GetValue();
			const int64_t minimum = integerParameter.GetMin();
			const int64_t maximum = integerParameter.GetMax();
			const int64_t increment = integerParameter.GetInc();

			// Adjust the pointer to the slider to get the correct position.
			int64_t newvalue = 0;
			CSliderCtrl* pSlider = DYNAMIC_DOWNCAST(CSliderCtrl, pScrollBar);
			newvalue = pSlider->GetPos();

			// Round to the next valid value.
			int64_t roundvalue = RoundTo(newvalue, value, minimum, maximum, increment);
			if (roundvalue == value) {
				return 0;
			}

			// Try to set the value. If successful, update the scroll position.
			try {
				integerParameter.SetValue(roundvalue);
				gBCam.exposureTime = roundvalue;
				pSlider->SetPos((int)roundvalue);
			}
			catch (GenICam::GenericException& e) {
				UNUSED(e);
				TRACE("Failed to set '%s':%s", integerParameter.GetInfo(Pylon::ParameterInfo_DisplayName).c_str(), e.GetDescription());
			}
			catch (...) {
				TRACE("Failed to set '%s'", integerParameter.GetInfo(Pylon::ParameterInfo_DisplayName).c_str());
			}

			return static_cast<UINT>(value);
		}
	}

	return 0;
}

void BaslerCameraDlg::OnLvnItemchangedCameraList(NMHDR* pNMHDR, LRESULT* pResult) {
	if (m_updatingList) {
		*pResult = 0;
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int iItem = pNMLV->iItem;

	if (!(LVIF_STATE & pNMLV->uChanged)) {
		*pResult = 0;
		return;
	}

	if (LVIS_SELECTED & pNMLV->uNewState) {
		/*
		// A device was selected.
		// Remember the full name of the currently selected device so it can be restored in case of new enumeration.
		const Pylon::CDeviceInfo* const pSelectedDeviceInfo = (const Pylon::CDeviceInfo*)m_CameraDeviceListCtrl.GetItemData(iItem);
		ASSERT(pSelectedDeviceInfo != NULL);

		//m_fullNameSelected = pSelectedDeviceInfo ? pSelectedDeviceInfo->GetFullName() : Pylon::String_t("");

		int vv = m_CameraDeviceListCtrl.GetItemCount();
		m_fullNameSelected = m_CameraDeviceListCtrl.GetItemText(0, 0);
		*/

		// FIND SELECTED CAM ITEM - START USING ITERATION
		Pylon::DeviceInfoList_t devices;
		try {
			// Get the transport layer factory.
			Pylon::CTlFactory& TlFactory = Pylon::CTlFactory::GetInstance();

			// Get all attached cameras.
			TlFactory.EnumerateDevices(devices);
		}
		catch (const Pylon::GenericException& e) {
			UNUSED(e);
			devices.clear();

			TRACE(CUtf82W(e.what()));
		}
		if (!devices.empty()) {
			int i = 0;
			for (Pylon::DeviceInfoList_t::const_iterator it = devices.begin(); it != devices.end(); ++it) {
				if (i == iItem) {
					const Pylon::CDeviceInfo* const pDeviceInfo = &(*it);
					m_fullNameSelected = pDeviceInfo->GetFullName();
					break;
				}
				i++;
			}
		}
		// FIND SELECTED CAM ITEM - END USING ITERATION

		m_CameraDeviceListCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
		m_CameraDeviceListCtrl.SetSelectionMark(iItem);

		CString strFullname(CUtf82W(m_fullNameSelected.c_str()));
		camName = strFullname;
		OpenCamera(strFullname);
	}
	else if (LVIS_SELECTED & pNMLV->uOldState) {
		m_CameraDeviceListCtrl.SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
	}

	*pResult = 0;
}

void BaslerCameraDlg::DeleteContents() {
	// Make sure the device is not grabbing.
	OnStopGrab();

	// Free the grab result, if present.
	m_bitmapImage.Release();
	m_ptrGrabResult.Release();

	m_camera.DeregisterImageEventHandler(this);
	m_camera.DeregisterConfiguration(this);

	// Perform cleanup.
	if (m_camera.IsPylonDeviceAttached()) {
		try {
			// Deregister the node callbacks.
			if (m_hExposureTime) {
				GenApi::Deregister(m_hExposureTime);
				m_hExposureTime = NULL;
			}
			if (m_hGain) {
				GenApi::Deregister(m_hGain);
				m_hGain = NULL;
			}

			/*if (m_hPixelFormat) {
				GenApi::Deregister(m_hPixelFormat);
				m_hPixelFormat = NULL;
			}
			if (m_hTestImage) {
				GenApi::Deregister(m_hTestImage);
				m_hTestImage = NULL;
			}*/

			// Clear the pointer to the features.
			m_exposureTime.Release();
			m_gain.Release();
			//m_testImage.Release();
			m_pixelFormat.Release();

			// Close camera.
			// This will also stop the grab.
			m_camera.Close();

			// Free the camera.
			// This will also stop the grab and close the camera.
			m_camera.DestroyDevice();
		}
		catch (const Pylon::GenericException& e) {
			TRACE(_T("Error during cleanup: %s"), (LPCWSTR)CUtf82W(e.what()));
			UNUSED(e);
		}
	}
}

void BaslerCameraDlg::OnStnClickedExposure() {
	// TODO: Add your control notification handler code here
}

void BaslerCameraDlg::OnCbnSelchangeTrigSelector() {
	// TODO: Add your control notification handler code here
	m_camera.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
}

void BaslerCameraDlg::OnCbnSelchangeTrigMode() {
	// TODO: Add your control notification handler code here
	CString val = L"";
	m_triggerMode.GetLBText(m_triggerMode.GetCurSel(), val);
	string textVal = string(CT2A(val));
	if (textVal == "On") {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
	}
	else {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	}
}

void BaslerCameraDlg::OnCbnSelchangeTrigSource() {
	// TODO: Add your control notification handler code here
	m_camera.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_Software);
}

void BaslerCameraDlg::OnBnClickedExecBtn() {
	// TODO: Add your control notification handler code here
	if (!m_camera.IsGrabbing()) {
		return;
	}

	// Only wait if software trigger is currently turned on.
	if (m_camera.TriggerSource.GetValue() == Basler_UniversalCameraParams::TriggerSource_Software
		&& m_camera.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On) {
		// If the camera is currently processing a previous trigger command,
		// it will silently discard trigger commands.
		// We wait until the camera is ready to process the next trigger.
		m_camera.WaitForFrameTriggerReady(3000, Pylon::TimeoutHandling_ThrowException);
	}
	// Send trigger
	m_camera.ExecuteSoftwareTrigger();
}

std::vector<Pylon::CBaslerUniversalInstantCamera> BaslerCameraDlg::GetBasCamInstance() {
	//pCam.clear();
	std::vector<Pylon::CBaslerUniversalInstantCamera>ppCam;
	/*if (m_camera.IsOpen()) {
		ppCam.push_back(m_camera);
	}*/
	return ppCam;
}

void BaslerCameraDlg::OnCbnSelchangeCombBat() {
	// TODO: Add your control notification handler code here
	CString val = L"";
	if (!m_camera.IsOpen()) {
		m_AutoWB.SetCurSel(0);
		AfxMessageBox(L"You have to open the CAM !!!", MB_ICONWARNING);
		UpdateData(FALSE);
		return;
	}
	m_AutoWB.GetLBText(m_AutoWB.GetCurSel(), val);
	string textVal = string(CT2A(val));
	if (m_camera.IsOpen()) {
		if (textVal == "Continuous") {
			m_camera.BalanceWhiteAuto.SetValue(Basler_UniversalCameraParams::BalanceWhiteAuto_Continuous);
		}
		else if (textVal == "Off") {
			m_camera.BalanceWhiteAuto.SetValue(Basler_UniversalCameraParams::BalanceWhiteAuto_Off);
		}
		else {
			m_camera.BalanceWhiteAuto.SetValue(Basler_UniversalCameraParams::BalanceWhiteAuto_Once);
		}
	}
}

void BaslerCameraDlg::OnCbnSelchangeCombExpoauto() {
	// TODO: Add your control notification handler code here
	CString val = L"";
	if (!m_camera.IsOpen()) {
		m_AutoExpos.SetCurSel(0);
		AfxMessageBox(L"You have to open the CAM !!!", MB_ICONWARNING);
		UpdateData(FALSE);
		return;
	}
	m_AutoExpos.GetLBText(m_AutoExpos.GetCurSel(), val);
	string textVal = string(CT2A(val));
	if (textVal == "Continuous") {
		m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Continuous);
	}
	else if (textVal == "Off") {
		m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Off);
	}
	else {
		m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Once);
	}
}