#include"pch.h"
#include"BaslerCamDev.h"
#include <vector>

CBaslerCamDev gBCam;

void CBaslerCamDev::SetBaslerCamProp() {
	BaslerCameraDlg::m_camera.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_RGB8);
	//BaslerCameraDlg::m_camera.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
	BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
	BaslerCameraDlg::m_camera.AcquisitionMode.SetValue(Basler_UniversalCameraParams::AcquisitionMode_Continuous);
	//BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
	//BaslerCameraDlg::m_camera.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_Software);
	BaslerCameraDlg::m_camera.Width.SetValue(gBCam.ImWidth);
	BaslerCameraDlg::m_camera.Height.SetValue(gBCam.ImHeight);
	BaslerCameraDlg::m_camera.AcquisitionFrameRateEnable.SetValue(true); // 12272023
	BaslerCameraDlg::m_camera.AcquisitionFrameRate.SetValue(gBCam.ImFrameRate); // 12272023
	//BaslerCameraDlg::m_camera.ExposureAuto.SetValue(Basler_UniversalCameraParams::ExposureAuto_Off); // 12272023
	//BaslerCameraDlg::m_camera.ExposureMode.SetValue(Basler_UniversalCameraParams::ExposureMode_Timed); // 12272023
	//BaslerCameraDlg::m_camera.ExposureTime.SetValue(gBCam.exposureTime); // 12272023
	BaslerCameraDlg::m_camera.SensorReadoutMode.SetValue(Basler_UniversalCameraParams::SensorReadoutMode_Fast);
}

void CBaslerCamDev::StartContGrabbing() {
	if (BaslerCameraDlg::m_camera.IsOpen()) {
		if (gBCam.isScanActive) {
			//BaslerCameraDlg::m_camera.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_Mono8); // 12272023
			BaslerCameraDlg::m_camera.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_RGB8); // 12272023
			BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_On);
			BaslerCameraDlg::m_camera.TriggerSelector.SetValue(Basler_UniversalCameraParams::TriggerSelector_FrameStart);
			BaslerCameraDlg::m_camera.TriggerSource.SetValue(Basler_UniversalCameraParams::TriggerSource_Software);
		}
		else {
			BaslerCameraDlg::m_camera.PixelFormat.SetValue(Basler_UniversalCameraParams::PixelFormat_RGB8);
			BaslerCameraDlg::m_camera.TriggerMode.SetValue(Basler_UniversalCameraParams::TriggerMode_Off);
		}
		BaslerCameraDlg::m_camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
	}
	else {
		AfxMessageBox(L"Camera Is Not Opening", MB_ICONERROR);
	}
}

void CBaslerCamDev::StopGrabbing() {
	if (BaslerCameraDlg::m_camera.IsGrabbing()) {
		BaslerCameraDlg::m_camera.StopGrabbing();
	}
}

void CBaslerCamDev::SendSWTrigger() {
	if (!BaslerCameraDlg::m_camera.IsGrabbing()) {
		return;
	}

	// Only wait if software trigger is currently turned on.
	if (BaslerCameraDlg::m_camera.TriggerSource.GetValue() == Basler_UniversalCameraParams::TriggerSource_Software
		&& BaslerCameraDlg::m_camera.TriggerMode.GetValue() == Basler_UniversalCameraParams::TriggerMode_On) {
		// If the camera is currently processing a previous trigger command,
		// it will silently discard trigger commands.
		// We wait until the camera is ready to process the next trigger.
		BaslerCameraDlg::m_camera.WaitForFrameTriggerReady(3000, Pylon::TimeoutHandling_ThrowException);
	}
	// Send trigger
	BaslerCameraDlg::m_camera.ExecuteSoftwareTrigger();
}

float CBaslerCamDev::getbCamFPS() {
	return (float)BaslerCameraDlg::m_camera.AcquisitionFrameRate.GetValue();
}

double CBaslerCamDev::resultingFPS() {
	return BaslerCameraDlg::m_camera.ResultingFrameRate.GetValue();
}

void CBaslerCamDev::UpdateImageProp() {}

void CBaslerCamDev::SaveImages() {
	CFileDialog dlg(
		FALSE, _T("IMS"), NULL,
		OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_NOCHANGEDIR,
		_T("IMS|*.IMS|"));
	std::wstring fname = DosUtil.GetProgramDir();
	fname += _T("IMS"); int a = _wmkdir(fname.c_str());
	dlg.m_ofn.lpstrInitialDir = fname.c_str();
	dlg.m_ofn.lpstrTitle = _T("Save image strip");
	if (dlg.DoModal() == IDOK) {
		CWaitCursor wc;
		Strip.Save(dlg.GetPathName().GetBuffer());
	}
}