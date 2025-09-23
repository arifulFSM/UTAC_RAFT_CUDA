#include "pch.h"

#include <string>

#include "IM/IM.h"
#include "LOG/LogSys.h"
#include "LucamDev.h"

#ifndef _SIMULUCAM

#pragma comment(lib,"lucamapi.lib")

CAM::CLucamDev Lucam;

BOOL bBlock = FALSE;

void CAM::CLucamDev::CloseHandle() {
	if (hCamera != INVALID_HANDLE_VALUE) {
		LucamCameraClose(hCamera); hCamera = INVALID_HANDLE_VALUE;
	}
}

CString CAM::CLucamDev::GetVersionInfo(LUCAM_VERSION* pVersions) {
	CString str;
	str.Format(L"CameraID: %ld\nSerial number: %ld\nFirmware: %ld\nFPGA: %ld\nDriver: %ld\nAPI: %ld\n",
		pVersions->cameraid, pVersions->serialnumber, pVersions->firmware,
		pVersions->fpga, pVersions->driver, pVersions->api);
	return str;
}

CAM::CLucamDev::CLucamDev() {
	lsSettings.gainRed = 1.0;
	lsSettings.gainBlue = 1.0;
	lsSettings.gainGrn1 = 1.0;
	lsSettings.gainGrn2 = 1.0;
	lsSettings.useStrobe = FALSE;
	lsSettings.strobeDelay = 0;
	lsSettings.format.pixelFormat = LUCAM_PF_8;
	lsSettings.useHwTrigger = FALSE;
	lsSettings.timeout = 10000;
	lsSettings.useHwTrigger = FALSE;
	lsSettings.timeout = 10000;
	lsSettings.shutterType = LUCAM_SHUTTER_TYPE_GLOBAL;
	lsSettings.exposureDelay = 0;
	lsSettings.flReserved1 = 0.0;
	lsSettings.flReserved2 = 0.0;
	lsSettings.ulReserved1 = 0;
	lsSettings.ulReserved2 = 0;
}

CAM::CLucamDev::~CLucamDev() {}

BOOL CAM::CLucamDev::IsValid() { return hCamera != INVALID_HANDLE_VALUE; }

short CAM::CLucamDev::GetSubSample() {
	return CamFrameFormat.subSampleX;
}

CString CAM::CLucamDev::GetCameraInfo() {
	CString info, str;
	int nCam = LucamNumCameras();
	info.Format(L"Lucam camera:\nNumber of camera %ld\n", nCam);
	LUCAM_VERSION* pVersionsArray = new LUCAM_VERSION;
	int n = LucamEnumCameras(pVersionsArray, nCam);
	for (int i = 0; i < n; i++) {
		info += GetVersionInfo(&pVersionsArray[i]);
	}
	delete pVersionsArray;
	return info;
}

CString CAM::CLucamDev::GetCameraFeatureInfo() {
	if (hCamera == INVALID_HANDLE_VALUE) return L"Error";
	if (!LucamGetFormat(hCamera, &CamFrameFormat, &fFrameRate)) { return L"Error"; }
	CString info, str;
	info.Format(L"Frame format\nDimension: %ld, %ld\nPixel format: %ld\nSubsampling: X=%d, Y=%d\n",
		CamFrameFormat.width / CamFrameFormat.subSampleX, CamFrameFormat.height / CamFrameFormat.subSampleY,
		CamFrameFormat.pixelFormat,
		CamFrameFormat.subSampleX, CamFrameFormat.subSampleY);
	str.Format(L"Frame rate: %.2f\n", fFrameRate); info += str;

	FLOAT fValue; LONG lFlags;
	FLOAT prop1 = 0, prop2 = 0, prop3 = 0;
	if (LucamGetProperty(hCamera, LUCAM_PROP_COLOR_FORMAT, &fValue, &lFlags)) {
		info += L"Color Format: ";
		if (fValue == LUCAM_CF_MONO) {
			info += L"Black && White\n";
		}
		else {
			info += L"RGB color\n";
		}
	}
	LucamGetProperty(hCamera, LUCAM_PROP_BRIGHTNESS, &prop1, &lFlags);
	LucamGetProperty(hCamera, LUCAM_PROP_CONTRAST, &prop2, &lFlags);
	str.Format(L"Brightness: %.2f, Contrast: %.2f\n", prop1, prop2); info += str;
	LucamGetProperty(hCamera, LUCAM_PROP_EXPOSURE, &prop1, &lFlags);
	LucamGetProperty(hCamera, LUCAM_PROP_GAIN, &prop2, &lFlags);
	LucamGetProperty(hCamera, LUCAM_PROP_GAMMA, &prop3, &lFlags);
	str.Format(L"Exposure: %.2f, Gain: %.2f, Gamma: %.2f\n", prop1, prop2, prop3); info += str;
	LucamGetProperty(hCamera, LUCAM_PROP_HUE, &prop1, &lFlags);
	LucamGetProperty(hCamera, LUCAM_PROP_SATURATION, &prop2, &lFlags);
	str.Format(L"Hue: %.2f, Saturation: %.2f\n", prop1, prop2); info += str;
	LucamGetProperty(hCamera, LUCAM_PROP_DIGITAL_WHITEBALANCE_U, &prop1, &lFlags);
	LucamGetProperty(hCamera, LUCAM_PROP_DIGITAL_WHITEBALANCE_V, &prop2, &lFlags);
	str.Format(L"White Balance: u=%.2f, v=: %.2f\n", prop1, prop2); info += str;

	return info;
}

BOOL CAM::CLucamDev::Gamma(BOOL bOn) {
	return 0;
}

float CAM::CLucamDev::GetGamma() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_GAMMA, &prop1, &lFlags);
	return prop1;
}

BOOL CAM::CLucamDev::SetGamma(float gamma) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_GAMMA, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_GAMMA, gamma, lFlags);
}

BOOL CAM::CLucamDev::AutoExposure() {
	if (!hCamera) return FALSE;
	if (!LucamGetFormat(hCamera, &CamFrameFormat, &fFrameRate)) return FALSE;
	LUCAM_FRAME_FORMAT* pFF = &CamFrameFormat;
	int wd = pFF->width / pFF->subSampleX, ht = pFF->height / pFF->subSampleY;
	return LucamOneShotAutoExposure(hCamera, 250, 0, 0, wd, ht);
}

BOOL CAM::CLucamDev::SetExposure(float exposure) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_EXPOSURE, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_EXPOSURE, exposure, lFlags);
}

float CAM::CLucamDev::GetSaturation() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_SATURATION, &prop1, &lFlags);
	return prop1;
}

BOOL CAM::CLucamDev::SetGain(float ga) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_GAIN, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_GAIN, ga, lFlags);
}

float CAM::CLucamDev::GetGain() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_GAIN, &prop1, &lFlags);
	return prop1;
}

BOOL CAM::CLucamDev::AutoGain() {
	if (!hCamera) return FALSE;
	if (!LucamGetFormat(hCamera, &CamFrameFormat, &fFrameRate)) return FALSE;
	LUCAM_FRAME_FORMAT* pFF = &CamFrameFormat;
	int wd = pFF->width / pFF->subSampleX, ht = pFF->height / pFF->subSampleY;
	return LucamOneShotAutoGain(hCamera, 130, 0, 0, wd, ht);
}

BOOL CAM::CLucamDev::GetCamProp(SCamProp& prop) {
	BOOL bOK = TRUE;
	LONG lFlags;
	if (!LucamGetProperty(hCamera, LUCAM_PROP_EXPOSURE, &prop.fExposure, &lFlags)) bOK = FALSE;
	if (!LucamGetProperty(hCamera, LUCAM_PROP_GAIN, &prop.fGain, &lFlags)) bOK = FALSE;
	if (Bpp == 24) {
		if (!LucamGetProperty(hCamera, LUCAM_PROP_GAIN_RED, &prop.fGainRed, &lFlags)) bOK = FALSE;
		if (!LucamGetProperty(hCamera, LUCAM_PROP_GAIN_GREEN1, &prop.fGainGrn1, &lFlags)) bOK = FALSE;
		if (!LucamGetProperty(hCamera, LUCAM_PROP_GAIN_GREEN2, &prop.fGainGrn2, &lFlags)) bOK = FALSE;
		if (!LucamGetProperty(hCamera, LUCAM_PROP_GAIN_BLUE, &prop.fGainBlue, &lFlags)) bOK = FALSE;
		if (!LucamGetProperty(hCamera, LUCAM_PROP_GAMMA, &prop.fGamma, &lFlags)) bOK = FALSE;
		if (!LucamGetProperty(hCamera, LUCAM_PROP_HUE, &prop.fHue, &lFlags)) bOK = FALSE;
		if (!LucamGetProperty(hCamera, LUCAM_PROP_SATURATION, &prop.fSaturate, &lFlags)) bOK = FALSE;
	}
	if (!LucamGetProperty(hCamera, LUCAM_PROP_CONTRAST, &prop.fContrast, &lFlags)) bOK = FALSE;
	if (!LucamGetProperty(hCamera, LUCAM_PROP_BRIGHTNESS, &prop.fBrightness, &lFlags)) bOK = FALSE;
	return bOK;
}

BOOL CAM::CLucamDev::SetCamProp(SCamProp& prop) {
	BOOL bOK = TRUE;
	LONG lFlags = 0;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_EXPOSURE, prop.fExposure, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN, prop.fGain, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_RED, prop.fGainRed, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_GREEN1, prop.fGainGrn1, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_GREEN2, prop.fGainGrn2, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_GAIN_BLUE, prop.fGainBlue, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_GAMMA, prop.fGamma, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_CONTRAST, prop.fContrast, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_BRIGHTNESS, prop.fBrightness, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_HUE, prop.fHue, lFlags)) bOK = FALSE;
	if (!LucamSetProperty(hCamera, LUCAM_PROP_SATURATION, prop.fSaturate, lFlags)) bOK = FALSE;
	return bOK;
}

BOOL CAM::CLucamDev::StartFastGrab(SCamProp& prop, int subSample) {
	LONG lFlags;
	FLOAT fValue;
	BOOL bIsMono = FALSE;

	lsSettings.exposure = prop.fExposure;
	lsSettings.gain = prop.fGain;

	if (LucamGetProperty(hCamera, LUCAM_PROP_COLOR_FORMAT, &fValue, &lFlags)) {
		bIsMono = ((LONG)fValue == LUCAM_CF_MONO) ? TRUE : FALSE;
	}
	else { LgS.Log(L"Unable to get properties from camera."); }
	if (!bIsMono) {
		lsSettings.gainRed = prop.fGainRed;
		lsSettings.gainGrn1 = prop.fGainGrn1;
		lsSettings.gainGrn2 = prop.fGainGrn2;
		lsSettings.gainBlue = prop.fGainBlue;
	}
	else {
		lsSettings.gainRed = 1.0;
		lsSettings.gainBlue = 1.0;
		lsSettings.gainGrn1 = 1.0;
		lsSettings.gainGrn2 = 1.0;
	}
	// Disable the GPO ports to output strobe (i.e. set to manual mode)
	if (!LucamGpoSelect(hCamera, 0x3)) {
		LgS.Log(L"Failed to enable strobe output.");
	}

	if (!LucamGetFormat(hCamera, &CamFrameFormat, &fFrameRate)) {
		LgS.Log(L"Unable to get camera video format. Snapshot mode disabled.", LOG::CERROR);
		return FALSE;
	}
	lsSettings.useStrobe = FALSE;
	lsSettings.strobeDelay = 0.0;

	lsSettings.format = CamFrameFormat;
	LUCAM_FRAME_FORMAT& format = CamFrameFormat;
	lsSettings.format.pixelFormat = LUCAM_PF_8; // enable 8 bit mode
	lsSettings.format.subSampleX = lsSettings.format.subSampleY = subSample;
	if (!Frame.Set(format.width / format.subSampleX, format.height / format.subSampleY)) {
		bSnapMode = false;
		LgS.Log(L"Error: Frame buffer allocation", LOG::CERROR);
		return FALSE;
	}
	lsSettings.useHwTrigger = FALSE;
	lsSettings.timeout = 5000.f + lsSettings.exposure;
	lsSettings.shutterType = LUCAM_SHUTTER_TYPE_GLOBAL;;
	lsSettings.exposureDelay = 0.1f;
	lsSettings.flReserved1 = 0.0;
	lsSettings.flReserved2 = 0.0;
	lsSettings.ulReserved1 = 0;
	lsSettings.ulReserved2 = 0;

	if (!LucamEnableFastFrames(hCamera, &lsSettings)) return FALSE;
	return TRUE;
}

BOOL CAM::CLucamDev::StopFastGrab() {
	return LucamDisableFastFrames(hCamera);
}

BOOL CAM::CLucamDev::FastGrab(IMGL::CIM& Im, SCamProp& prop) {
	if (!LucamTakeFastFrame(hCamera, Frame.pcFrame)) {
		DWORD dwError = LucamGetLastError();
		LgS.Log(L"Could not take fast snapshot.");
		Frame.Clear();
		LucamDisableFastFrames(hCamera);
		return FALSE;
	}
	LUCAM_FRAME_FORMAT& format = lsSettings.format;
	//if (!Im.IsNull()) Im.Destroy();
	//if (!Im.Create(format.width / format.subSampleX, format.height / format.subSampleY, 24)) {
	//	Frame.Clear(); return FALSE;
	//}
	if (Im.IsNull()) {
		if (!Im.Create(format.width / format.subSampleX, format.height / format.subSampleY, 24)) {
			Frame.Clear(); return FALSE;
		}
	}
	LUCAM_CONVERSION lcConversion;
	lcConversion.CorrectionMatrix = LUCAM_CM_LED;
	lcConversion.DemosaicMethod = LUCAM_DM_HIGHER_QUALITY;
	if (!LucamConvertFrameToRgb24(hCamera, Im.GetBufferStart(),
		Frame.pcFrame, format.width / format.subSampleX, format.height / format.subSampleY,
		format.pixelFormat, &lcConversion)) {
		Frame.Clear();
		LgS.Log(L"Error: Could not convert snapshot to rgb", LOG::CERROR);
	}
	return TRUE;
}

BOOL CAM::CLucamDev::FastGrab(SCamProp& prop) {
	if (!LucamTakeFastFrame(hCamera, Frame.pcFrame)) {
		DWORD dwError = LucamGetLastError();
		LgS.Log(L"Could not take fast snapshot.");
		//Frame.Clear();
		//LucamDisableFastFrames(hCamera);
		return FastGrab(prop);
	}
	return TRUE;
}

BOOL CAM::CLucamDev::Convert24(IMGL::CIM& Im) {
	LUCAM_FRAME_FORMAT& format = lsSettings.format;
	if (Im.IsNull()) {
		if (!Im.Create(format.width / format.subSampleX, format.height / format.subSampleY, 24)) {
			Frame.Clear(); return FALSE;
		}
	}
	LUCAM_CONVERSION lcConversion;
	lcConversion.CorrectionMatrix = LUCAM_CM_LED;
	lcConversion.DemosaicMethod = LUCAM_DM_HIGHER_QUALITY;
	if (!LucamConvertFrameToRgb24(hCamera, Im.GetBufferStart(),
		Frame.pcFrame, format.width / format.subSampleX, format.height / format.subSampleY,
		format.pixelFormat, &lcConversion)) {
		Frame.Clear();
		LgS.Log(L"Error: Could not convert snapshot to rgb", LOG::CERROR);
	}
	return TRUE;
}

bool CAM::CLucamDev::LucamPropPage() {
	if (hCamera == INVALID_HANDLE_VALUE) return false;
	return LucamDisplayPropertyPage(hCamera, NULL);
}

bool CAM::CLucamDev::LucamVideoFormat() {
	if (hCamera == INVALID_HANDLE_VALUE) return false;
	return LucamDisplayVideoFormatPage(hCamera, NULL);
}

BOOL CAM::CLucamDev::SetBrightness(float bright) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_BRIGHTNESS, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_BRIGHTNESS, bright, lFlags);
}

BOOL CAM::CLucamDev::SetContrast(float contrast) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_CONTRAST, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_CONTRAST, contrast, lFlags);
}

float CAM::CLucamDev::GetBrightness() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_BRIGHTNESS, &prop1, &lFlags);
	return prop1;
}

float CAM::CLucamDev::GetContrast() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_CONTRAST, &prop1, &lFlags);
	return prop1;
}

float CAM::CLucamDev::GetHue() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_HUE, &prop1, &lFlags);
	return prop1;
}

BOOL CAM::CLucamDev::SetHue(float hu) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_HUE, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_HUE, hu, lFlags);
}

BOOL CAM::CLucamDev::SetSaturation(float saturate) {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_SATURATION, &prop1, &lFlags);
	return LucamSetProperty(hCamera, LUCAM_PROP_SATURATION, saturate, lFlags);
}

float CAM::CLucamDev::GetExposure() {
	LONG lFlags; FLOAT prop1 = 0;
	LucamGetProperty(hCamera, LUCAM_PROP_EXPOSURE, &prop1, &lFlags);
	return prop1;
}

BOOL CAM::CLucamDev::WhiteBalance() {
	if (hCamera == INVALID_HANDLE_VALUE) return FALSE;
	int wd = CamFrameFormat.width / CamFrameFormat.subSampleX / 3;
	int ht = CamFrameFormat.height / CamFrameFormat.subSampleY / 3;
	return LucamOneShotAutoWhiteBalance(hCamera, 0, 0, wd, ht);
}

HANDLE CAM::CLucamDev::GetCameraHandle() {
	return hCamera;
}

BOOL CAM::CLucamDev::InitCapture(USHORT ID, short subSampling) {
	if (CamFrameFormat.subSampleX == subSampling) {
		if (bInit) return TRUE;
	}
	if (hCamera == INVALID_HANDLE_VALUE) hCamera = LucamCameraOpen(ID);
	if (hCamera == INVALID_HANDLE_VALUE) return FALSE;
	std::wstring msg = L"Camera " + std::to_wstring(ID) + L" opened"; LgS.Log(msg.c_str());
	if (!LucamGetFormat(hCamera, &CamFrameFormat, &fFrameRate)) { CloseHandle(); return FALSE; }
	CamFrameFormat.subSampleX = CamFrameFormat.subSampleY = subSampling;
	if (!LucamSetFormat(hCamera, &CamFrameFormat, fFrameRate)) { CloseHandle(); return FALSE; }
	if (!LucamGetFormat(hCamera, &CamFrameFormat, &fFrameRate)) { CloseHandle(); return FALSE; }
	LucamSetProperty(hCamera, LUCAM_PROP_FLIPPING, LUCAM_PROP_FLIPPING_Y, 0);
	LucamSetProperty(hCamera, LUCAM_PROP_DEMOSAICING_METHOD, LUCAM_DM_HIGHER_QUALITY, 0);

	Wd = CamFrameFormat.width;
	Ht = CamFrameFormat.height;
	FLOAT fValue; LONG lFlags;
	if (LucamGetProperty(hCamera, LUCAM_PROP_COLOR_FORMAT, &fValue, &lFlags)) {
		if (fValue == LUCAM_CF_MONO) Bpp = 8; else Bpp = 24;
	}
	bInit = TRUE;
	return TRUE;
}

BOOL CAM::CLucamDev::DeinitCapture(USHORT ID) {
	if (bInit) {
		if (!LucamCameraClose(hCamera)) {
			LgS.Log(L"Error: Close camera FAILED", LOG::CERROR); return FALSE;
		}
		bInit = FALSE;
		hCamera = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

BOOL CAM::CLucamDev::StartStream(SCtx& Ctx, USHORT ID) {
	if (hCamera == INVALID_HANDLE_VALUE) {
		if (!InitCapture(ID, subSampling)) CloseHandle();
		if (hCamera == INVALID_HANDLE_VALUE) return FALSE;
	}
	if (bIsStream) LucamStreamVideoControl(hCamera, STOP_STREAMING, Ctx.hWnd);
	if (!LucamStreamVideoControl(hCamera, START_DISPLAY, Ctx.hWnd)) return FALSE;
	bIsStream = TRUE;
	return TRUE;
}

BOOL CAM::CLucamDev::StopStream(SCtx& Ctx, USHORT ID) {
	if (hCamera == INVALID_HANDLE_VALUE) return FALSE;
	if (bIsStream) {
		if (!LucamStreamVideoControl(hCamera, STOP_STREAMING, Ctx.hWnd)) {
			LgS.Log(L"Error: Stop stream video FAILED", LOG::CERROR);  return FALSE;
		}
		bIsStream = FALSE;
	}

	return TRUE;
}

BOOL CAM::CLucamDev::GrabOneFrame(IMGL::CIM& Im, int subSample) {
	if (hCamera == INVALID_HANDLE_VALUE) {
		LgS.Log(L"Error: Camera not open", LOG::CERROR);
		return FALSE;
	}

	SCamProp prop;
	if (!GetCamProp(prop)) return FALSE;
	if (!StartFastGrab(prop, subSample)) { return FALSE; }
	if (!FastGrab(Im, prop)) return FALSE;
	if (!StopFastGrab()) return FALSE;

	return TRUE;
}

BOOL CAM::CLucamDev::SaveOneFrame(char* filename, int subSample) {
	IMGL::CIM Im;
	if (GrabOneFrame(Im, subSample)) { Im.Save(CString(filename)); }
	return FALSE;
}

BOOL CAM::CLucamDev::SetProperties() {
	LucamSetProperty(hCamera, LUCAM_PROP_EXPOSURE, Par.Pa.fExposure, 0);
	LucamSetProperty(hCamera, LUCAM_PROP_GAIN, Par.Pa.fGain, 0);
	LucamSetProperty(hCamera, LUCAM_PROP_GAMMA, Par.Pa.fGamma, 0);
	SaveParam();
	return TRUE;
}

void CAM::CLucamDev::DrawCross(BYTE* pData) {
	LUCAM_FRAME_FORMAT* pFF = &CamFrameFormat;
	int wd = pFF->width / pFF->subSampleX, ht = pFF->height / pFF->subSampleY;
	int cx = wd / 2, cy = ht / 2;
	memset(pData + (cy - 1) * wd + cx - 64, 0, 128);
	memset(pData + cy * wd + cx - 64, 0, 128);
	memset(pData + (cy + 1) * wd + cx - 64, 0, 128);

	for (int y = cy - 64; y < cy + 64; y++) {
		for (int i = -1; i <= 1; i++) {
			PBYTE p1 = pData + y * wd + cx + i;
			*p1 = 0;
		}
	}
}

BOOL CAM::SFrame::Set(int w, int h) {
	if ((wd == w) && (ht == h)) return TRUE;
	if (pcFrame) delete[]pcFrame;
	//if (pcFrameRgb) delete[]pcFrameRgb;
	pcFrame = new UCHAR[w * h * sizeof(UCHAR)];
	//pcFrameRgb = new UCHAR[w * h * 3 * sizeof(UCHAR)];
	if (!pcFrame /* || !pcFrameRgb*/) { Clear(); return FALSE; }
	wd = w; ht = h;
	return TRUE;
}

void CAM::SFrame::Clear() {
	if (pcFrame) delete[]pcFrame;
	//if (pcFrameRgb) delete[]pcFrameRgb;
	wd = 0; ht = 0;
}

CAM::SFrame::SFrame() {}

CAM::SFrame::~SFrame() { Clear(); }

#endif