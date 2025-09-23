#include "pch.h"
#include "SRC\DOSUtil.h"
#include "BaslerColourCam.h"
#include "../IM/IM.h"
#include <exception>
#include <string>
#include "PSI/Strip.h"

using namespace Pylon;
using namespace GenApi;
using namespace CAM;

#ifdef _SIMUBASCOLRCAM
CAM::CCamera BasCol;
#endif

CAM::CBaslerColourCam::CBaslerColourCam() {}

CAM::CBaslerColourCam::~CBaslerColourCam() {
	if (camProp) {
		delete camProp;
		camProp = NULL;
	}
}

BOOL CAM::CBaslerColourCam::IsValid() { return true; }

CString CAM::CBaslerColourCam::GetCameraInfo() {
	return L"Simulation camera";
}

CString CAM::CBaslerColourCam::GetCameraFeatureInfo() {
	return L"No feature";
}

BOOL CBaslerColourCam::GrabOneFrame(IMGL::CIM& Im, int subSample) {
	//Sleep(100); // Working
	//Sleep(2);
	if (Im) Im.Detach();
	if (!m_camera.IsOpen() || !m_camera.IsGrabbing()) {
		//AfxMessageBox(L"Camera Not Open Or Running");
		return FALSE;
	}
	isImgCapSuccess = false;
	ExecuteTriggerLoc();
	Yld(15);
	if (!isImgCapSuccess) {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
		Sleep(5);
		return FALSE;
	}
	//try {
	tmp_bitmapimage.CopyImage(m_bitmapImage);
	Im.Attach(tmp_bitmapimage);
	tmp_bitmapimage.Detach();
	m_bitmapImage.Release();
	//}
	//catch (std::exception& e) {
	m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	//return FALSE;
//}
	return TRUE;
}

void CBaslerColourCam::ExecuteTriggerLoc() {
	m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
	// Only wait if software trigger is currently turned on.
	if (m_camera.TriggerSource.GetValue() == Basler_UniversalCameraParams::TriggerSource_Software && m_camera.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On) {
		m_camera.WaitForFrameTriggerReady(3000, Pylon::TimeoutHandling_ThrowException);
	}
	// Send trigger
	m_camera.ExecuteSoftwareTrigger();
}

BOOL CBaslerColourCam::ExecuteTrigger() {
	// Only wait if software trigger is currently turned on.
	if (m_camera.TriggerSource.GetValue() == Basler_UniversalCameraParams::TriggerSource_Software && m_camera.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On) {
		m_camera.WaitForFrameTriggerReady(250, Pylon::TimeoutHandling_ThrowException);
	}
	// Send trigger
	m_camera.ExecuteSoftwareTrigger();
	return TRUE;
}

BOOL CBaslerColourCam::SetTriggerMode(bool isTrigOn) {
	if (isTrigOn) {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
	}
	else {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	}
	return TRUE;
}

BOOL CBaslerColourCam::GetBitmapImage(IMGL::CIM& Im) {
	if (m_bitmapImage.IsValid()) {
		Im.Attach(m_bitmapImage);
		return TRUE;
	}
	return FALSE;
}

void CBaslerColourCam::Yld(int _tOut) {
	int initTime = GetTickCount();
	while (GetTickCount() - initTime < _tOut) {
		if (isImgCapSuccess) {
			//Sleep(100); // Working
			Sleep(20);
			break;
		}
	}
}

BOOL CBaslerColourCam::SaveOneFrame(char* filename, int subSample) {
	return TRUE;
}

HANDLE CAM::CBaslerColourCam::GetCameraHandle() {
	return INVALID_HANDLE_VALUE;
}

BOOL CAM::CBaslerColourCam::InitCapture(USHORT ID, short subSampling) {
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
		return FALSE;
	}

	int size = (int)devices.size();
	m_devices = devices;
	Pylon::String_t strDeviceFullName;

	if (devices.empty()) return FALSE;

	int camNum = 0;
	for (Pylon::DeviceInfoList_t::const_iterator it = devices.begin(); it != devices.end(); ++it) {
		const Pylon::CDeviceInfo* const pDeviceInfo = &(*it); // Primary Colour Camera
		strDeviceFullName = pDeviceInfo->GetFullName();
		CString strFullname(CUtf82W(strDeviceFullName.c_str()));
		camNum++;
		if (camNum == 4) break;
	}

	try {
		m_camera.RegisterImageEventHandler(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
		m_camera.RegisterConfiguration(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_None);
	}
	catch (const Pylon::GenericException& e) {
		TRACE(CUtf82W(e.what()));
		UNUSED(e);
		return FALSE;
	}

	try {
		Pylon::IPylonDevice* pDevice = Pylon::CTlFactory::GetInstance().CreateDevice(strDeviceFullName);
		m_camera.Attach(pDevice, Pylon::Cleanup_Delete);

		// Open camera.
		m_camera.Open();

		m_camera.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
		m_camera.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_Software);
		m_camera.SensorReadoutMode.SetValue(Basler_UniversalCameraParams::SensorReadoutMode_Fast);
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);

		// Clear everything.
		CSingleLock lock(&m_MemberLock, TRUE);
		m_ptrGrabResult.Release();
		m_bitmapImage.Release();
	}
	catch (const Pylon::GenericException& e) {
		UNUSED(e);
		TRACE(CUtf82W(e.what()));
		return FALSE;
	}

	return (int)m_devices.size();
}

BOOL CAM::CBaslerColourCam::GrabOneFrame(IMGL::CIM& Im) {
	return TRUE;
}

BOOL CAM::CBaslerColourCam::StartStream() { return TRUE; };

void CAM::CBaslerColourCam::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult) {
	CSingleLock lock(&m_MemberLock, TRUE);
	m_ptrGrabResult = grabResult;
	if (m_ptrGrabResult.IsValid() && m_ptrGrabResult->GrabSucceeded()) {
		try {
			m_bitmapImage.CopyImage(m_ptrGrabResult);
			DisplayImage();
		}
		catch (std::exception& e) {
			TRACE("%s\n", e.what());
		}
	}
	lock.Unlock();
	isImgCapSuccess = true;
}

void CAM::CBaslerColourCam::DisplayImage() {
	if (ImgStream) ImgStream.Detach();
	ImgStream.Attach(m_bitmapImage);

	HDC hDc = GetDC(Ctx.hWnd);

	BITMAP bm;
	GetObject(ImgStream, sizeof(bm), &bm);

	// Create a compatible device context and select the bitmap into it
	HDC hdcMem = CreateCompatibleDC(hDc);
	HGDIOBJ oldBitmap = SelectObject(hdcMem, ImgStream);

	// Use BitBlt to copy the image directly to the window HDC
	BitBlt(hDc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
	//StretchBlt(hDc, 0, 0, Ctx.rc.right - Ctx.rc.left, Ctx.rc.bottom - Ctx.rc.top, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	ReleaseDC(Ctx.hWnd, hDc);

	// Clean up
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
}

BOOL CAM::CBaslerColourCam::StartStream(SCtx& _Ctx, USHORT ID) {
	if (!m_camera.IsOpen() || m_camera.IsGrabbing()) {
		AfxMessageBox(L"Camera Not Open Or Another Instance Running");
		return FALSE;
	}
	Ctx = _Ctx;
	m_camera.AcquisitionMode.SetValue(Basler_UniversalCameraParams::AcquisitionMode_Continuous);
	m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

	return TRUE;
}

BOOL CAM::CBaslerColourCam::StopStream(SCtx& Ctx, USHORT ID) {
	StopStream();
	return TRUE;
}

BOOL CAM::CBaslerColourCam::StopStream() {
	// Camera may have been disconnected.
	if (m_camera.IsGrabbing()) {
		m_camera.StopGrabbing();
	}
	return TRUE;
};

BOOL CAM::CBaslerColourCam::DeinitCapture(USHORT ID) {
	StopStream();
	// Free the grab result, if present.
	m_bitmapImage.Release();
	m_ptrGrabResult.Release();

	m_camera.DeregisterImageEventHandler(this);
	m_camera.DeregisterConfiguration(this);

	// Perform cleanup.
	if (m_camera.IsPylonDeviceAttached()) {
		try {
			// Deregister the node callbacks.
			/*if (m_hExposureTime) {
				GenApi::Deregister(m_hExposureTime);
				m_hExposureTime = NULL;
			}
			if (m_hGain) {
				GenApi::Deregister(m_hGain);
				m_hGain = NULL;
			}*/

			/*if (m_hPixelFormat) {
				GenApi::Deregister(m_hPixelFormat);
				m_hPixelFormat = NULL;
			}
			if (m_hTestImage) {
				GenApi::Deregister(m_hTestImage);
				m_hTestImage = NULL;
			}*/

			// Clear the pointer to the features.
			/*m_exposureTime.Release();
			m_gain.Release();*/
			//m_testImage.Release();
			//m_pixelFormat.Release();

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
			return FALSE;
		}
	}
	return TRUE;
}

void CAM::CBaslerColourCam::propertyPageDisplay() {
	if (!m_camera.IsOpen()) {
		AfxMessageBox(L"Camera Is Not Open Or Another Instance Running", MB_ICONINFORMATION);
		return;
	}

	if (camProp) {
		delete camProp;
		camProp = NULL;
	}

	camProp = new CameraProp;
	camProp->EID = CAM::ECAM::PRICAM;
	camProp->Create(IDD_DLG_CAM_PROP);
	camProp->ShowWindow(SW_SHOW);
}

BOOL CAM::CBaslerColourCam::WhiteBalance() {
	if (camProp->AWBVal == "Continuous") {
		m_camera.BalanceWhiteAuto.SetValue(Basler_UniversalCameraParams::BalanceWhiteAuto_Continuous);
	}
	else if (camProp->AWBVal == "Off") {
		m_camera.BalanceWhiteAuto.SetValue(Basler_UniversalCameraParams::BalanceWhiteAuto_Off);
	}
	else {
		m_camera.BalanceWhiteAuto.SetValue(Basler_UniversalCameraParams::BalanceWhiteAuto_Once);
	}
	return TRUE;
}

BOOL CAM::CBaslerColourCam::Gamma(BOOL bOn) {
	return 0;
}

float CAM::CBaslerColourCam::GetGamma() {
	return 0.0f;
}

float CAM::CBaslerColourCam::GetHue() {
	return 0.0f;
}

BOOL CAM::CBaslerColourCam::SetHue(float hu) {
	return 0;
}

BOOL CAM::CBaslerColourCam::SetGamma(float gamma) {
	return 0;
}

BOOL CAM::CBaslerColourCam::AutoExposure() {
	if (camProp->AEVal == "Continuous") {
		m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Continuous);
	}
	else if (camProp->AEVal == "Off") {
		m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Off);
	}
	else {
		m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Once);
	}
	return TRUE;
}

BOOL CAM::CBaslerColourCam::SetExposure(float exposure) {
	if (exposure > 0.00) m_camera.ExposureTime.SetValue(exposure);
	return TRUE;
}

float CAM::CBaslerColourCam::GetExposure() {
	return  m_camera.ExposureTime.GetValue();
}

bool CAM::CBaslerColourCam::SetTrigger() {
	if (camProp->TRIGVal == "On") {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
	}
	else {
		m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	}
	return true;
}

float CAM::CBaslerColourCam::GetSaturation() {
	return 0.0f;
}

BOOL CAM::CBaslerColourCam::SetGain(float ga) {
	return 0;
}

float CAM::CBaslerColourCam::GetGain() {
	return 0.0f;
}

BOOL CAM::CBaslerColourCam::AutoGain() {
	return 0;
}

short CAM::CBaslerColourCam::GetSubSample() {
	return 1;
}

// camera specific

BOOL CAM::CBaslerColourCam::GetCamProp(SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerColourCam::SetCamProp(SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerColourCam::StartFastGrab(SCamProp& prop, int subSample) { return TRUE; }

BOOL CAM::CBaslerColourCam::StopFastGrab() { return TRUE; }

BOOL CAM::CBaslerColourCam::FastGrab(IMGL::CIM& Im, SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerColourCam::FastGrab(SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerColourCam::Convert24(IMGL::CIM& Im) { return TRUE; }

bool CAM::CBaslerColourCam::LucamPropPage() { return 1; }

bool CAM::CBaslerColourCam::LucamVideoFormat() { return 1; }

BOOL CAM::CBaslerColourCam::SetBrightness(float bright) { return 0; }

BOOL CAM::CBaslerColourCam::SetContrast(float contrast) { return 0; }

float CAM::CBaslerColourCam::GetBrightness() { return 0.0f; }

float CAM::CBaslerColourCam::GetContrast() { return 0.0f; }

BOOL CAM::CBaslerColourCam::SetSaturation(float saturate) { return 0; }

BOOL CAM::CBaslerColourCam::SetProperties() { return TRUE; }