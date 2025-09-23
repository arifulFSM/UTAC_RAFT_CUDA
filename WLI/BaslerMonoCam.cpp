#include "pch.h"
#include "SRC\DOSUtil.h"
#include "BaslerMonoCam.h"
#include "../IM/IM.h"

#include <string>

using namespace Pylon;
using namespace GenApi;
using namespace CAM;

#ifdef _SIMUBASMONOCAM
CAM::CCamera BasMono;
#endif

BOOL CAM::CBaslerMonoCam::IsValid() { return true; }

CString CAM::CBaslerMonoCam::GetCameraInfo() {
	return L"Simulation camera";
}

CString CAM::CBaslerMonoCam::GetCameraFeatureInfo() {
	return L"No feature";
}

BOOL CBaslerMonoCam::GrabOneFrame(IMGL::CIM& Im, int subSample) {
	Sleep(100);
	if (Im) Im.Detach();
	if (!m_camera.IsOpen()) {
		//AfxMessageBox(L"Camera Not Open Or Another Instance Running");
		return FALSE;
	}
	if (m_camera.IsGrabbing()) {
		StopStream();
	}
	m_singleConfiguration.ApplyConfiguration(m_camera.GetNodeMap());
	m_camera.StartGrabbing(1, Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
	isImgCapSuccess = false;
	Yld(5000);
	tmp_bitmapimage.CopyImage(m_bitmapImage);
	Im.Attach(tmp_bitmapimage);
	tmp_bitmapimage.Detach();
	m_bitmapImage.Release();
	return TRUE;
}

void CBaslerMonoCam::Yld(int _tOut) {
	int initTime = GetTickCount();
	while (GetTickCount() - initTime < _tOut) {
		if (isImgCapSuccess) {
			Sleep(50);
			break;
		}
	}
}

BOOL CBaslerMonoCam::SaveOneFrame(char* filename, int subSample) {
	return TRUE;
}

HANDLE CAM::CBaslerMonoCam::GetCameraHandle() {
	return INVALID_HANDLE_VALUE;
}

BOOL CAM::CBaslerMonoCam::InitCapture(USHORT ID, short subSampling) {
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

	for (Pylon::DeviceInfoList_t::const_iterator it = devices.begin(); it != devices.end(); ++it) {
		const Pylon::CDeviceInfo* const pDeviceInfo = &(*it); // Secondary Mono Camera [ Second Camera ]
		strDeviceFullName = pDeviceInfo->GetFullName();
		CString strFullname(CUtf82W(strDeviceFullName.c_str()));
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

BOOL CAM::CBaslerMonoCam::GrabOneFrame(IMGL::CIM& Im) {
	return TRUE;
}

BOOL CAM::CBaslerMonoCam::StartStream() { return TRUE; };

void CAM::CBaslerMonoCam::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult) {
	CSingleLock lock(&m_MemberLock, TRUE);
	m_ptrGrabResult = grabResult;
	if (m_ptrGrabResult.IsValid() && m_ptrGrabResult->GrabSucceeded()) {
		m_bitmapImage.CopyImage(m_ptrGrabResult);
		if (isImgCapSuccess) DisplayImage();
	}
	lock.Unlock();
	isImgCapSuccess = true;
}

void CAM::CBaslerMonoCam::DisplayImage() {
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
	ReleaseDC(Ctx.hWnd, hDc);

	// Clean up
	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
}

BOOL CAM::CBaslerMonoCam::StartStream(SCtx& _Ctx, USHORT ID) {
	if (!m_camera.IsOpen() || m_camera.IsGrabbing()) {
		//AfxMessageBox(L"Camera Not Open Or Another Instance Running");
		return FALSE;
	}
	Ctx = _Ctx;
	m_camera.AcquisitionMode.SetValue(Basler_UniversalCameraParams::AcquisitionMode_Continuous);
	m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);

	return TRUE;
}

BOOL CAM::CBaslerMonoCam::StopStream(SCtx& Ctx, USHORT ID) {
	StopStream();
	return TRUE;
}

BOOL CAM::CBaslerMonoCam::StopStream() {
	// Camera may have been disconnected.
	if (m_camera.IsGrabbing()) {
		m_camera.StopGrabbing();
		Sleep(250);
	}
	return TRUE;
};

BOOL CAM::CBaslerMonoCam::DeinitCapture(USHORT ID) {
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

void CAM::CBaslerMonoCam::propertyPageDisplay() {
	AfxMessageBox(L"Property Page Displayed");
}

BOOL CAM::CBaslerMonoCam::WhiteBalance() {
	return TRUE;
}

BOOL CAM::CBaslerMonoCam::Gamma(BOOL bOn) {
	return 0;
}

float CAM::CBaslerMonoCam::GetGamma() {
	return 0.0f;
}

float CAM::CBaslerMonoCam::GetHue() {
	return 0.0f;
}

BOOL CAM::CBaslerMonoCam::SetHue(float hu) {
	return 0;
}

BOOL CAM::CBaslerMonoCam::SetGamma(float gamma) {
	return 0;
}

BOOL CAM::CBaslerMonoCam::AutoExposure() {
	return 0;
}

BOOL CAM::CBaslerMonoCam::SetExposure(float exposure) {
	return 0;
}

float CAM::CBaslerMonoCam::GetSaturation() {
	return 0.0f;
}

BOOL CAM::CBaslerMonoCam::SetGain(float ga) {
	return 0;
}

float CAM::CBaslerMonoCam::GetGain() {
	return 0.0f;
}

BOOL CAM::CBaslerMonoCam::AutoGain() {
	return 0;
}

short CAM::CBaslerMonoCam::GetSubSample() {
	return 1;
}

// camera specific

BOOL CAM::CBaslerMonoCam::GetCamProp(SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerMonoCam::SetCamProp(SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerMonoCam::StartFastGrab(SCamProp& prop, int subSample) { return TRUE; }

BOOL CAM::CBaslerMonoCam::StopFastGrab() { return TRUE; }

BOOL CAM::CBaslerMonoCam::FastGrab(IMGL::CIM& Im, SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerMonoCam::FastGrab(SCamProp& prop) { return TRUE; }

BOOL CAM::CBaslerMonoCam::Convert24(IMGL::CIM& Im) { return TRUE; }

bool CAM::CBaslerMonoCam::LucamPropPage() { return 1; }

bool CAM::CBaslerMonoCam::LucamVideoFormat() { return 1; }

BOOL CAM::CBaslerMonoCam::SetBrightness(float bright) { return 0; }

BOOL CAM::CBaslerMonoCam::SetContrast(float contrast) { return 0; }

float CAM::CBaslerMonoCam::GetBrightness() { return 0.0f; }

float CAM::CBaslerMonoCam::GetContrast() { return 0.0f; }

BOOL CAM::CBaslerMonoCam::SetSaturation(float saturate) { return 0; }

BOOL CAM::CBaslerMonoCam::SetProperties() { return TRUE; }

float CAM::CBaslerMonoCam::GetExposure() { return 0.0f; }

CAM::CBaslerMonoCam::CBaslerMonoCam() {}

CAM::CBaslerMonoCam::~CBaslerMonoCam() {}