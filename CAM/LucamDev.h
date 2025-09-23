#pragma once

#include "Camera.h"

#define _SIMULUCAM

#ifndef _SIMULUCAM

#include "LucamImg.h"
#include "Lucamapi.h"

//////////////////////////////////////////////////////////////////////////
// Actual camera device [1/1/2020 yuenl]
//////////////////////////////////////////////////////////////////////////

namespace CAM {
	struct SFrame {
		int wd = 0, ht = 0;
		UCHAR* pcFrame = nullptr;;

		BOOL Set(int w, int h);
		void Clear();
		SFrame();
		~SFrame();
	};

	class CLucamDev : public CCamera {
		BOOL bInit = FALSE;

		void CloseHandle();

	protected:
		////////////////////////////////////////////////////////////////////
		// Lucam related
		BOOL bSnapMode = FALSE;
		HANDLE hCamera = INVALID_HANDLE_VALUE;
		FLOAT fFrameRate;
		SFrame Frame; // frame buffers
		LUCAM_SNAPSHOT lsSettings;
		LUCAM_FRAME_FORMAT CamFrameFormat = { 0 };
		CString GetVersionInfo(LUCAM_VERSION* pVersions);
		////////////////////////////////////////////////////////////////////

	public:
		BOOL IsValid();
		short GetSubSample();
		CString GetCameraInfo();
		CString GetCameraFeatureInfo();

	public:
		BOOL AutoExposure();
		BOOL DeinitCapture(USHORT ID);
		BOOL Gamma(BOOL bOn);
		BOOL InitCapture(USHORT ID, short subSampling);
		BOOL SetBrightness(float bright);
		BOOL SetContrast(float contrast);
		BOOL SetExposure(float exposure);
		BOOL SetGamma(float gamma);
		BOOL SetHue(float hu);
		BOOL SetProperties();
		BOOL SetSaturation(float saturate);
		BOOL StartStream(SCtx& Ctx, USHORT ID);
		BOOL StopStream(SCtx& Ctx, USHORT ID);
		BOOL StartStream() { return TRUE; }; // For Basler Cam
		BOOL StopStream() { return TRUE; }; // For Basler Cam
		BOOL GrabOneFrame(IMGL::CIM& Im) { return TRUE; };// For Basler Cam
		BOOL WhiteBalance();
		float GetBrightness();
		float GetContrast();
		float GetExposure();
		float GetGamma();
		float GetHue();
		float GetSaturation();
		BOOL SetGain(float ga);
		float GetGain();
		BOOL AutoGain();
		bool SetTrigger() { return true; }

	public:
		BOOL GetCamProp(SCamProp& prop);
		BOOL SetCamProp(SCamProp& prop);
		BOOL StartFastGrab(SCamProp& prop, int subSample);
		BOOL StopFastGrab();
		BOOL FastGrab(IMGL::CIM& Im, SCamProp& prop);
		BOOL FastGrab(SCamProp& prop);
		BOOL Convert24(IMGL::CIM& Im);

	public:
		bool LucamPropPage();
		bool LucamVideoFormat();

	public:
		BOOL SaveOneFrame(char* filename, int subSample);
		BOOL GrabOneFrame(IMGL::CIM& Im, int subSample);
		void propertyPageDisplay() { return; }

	public:
		HANDLE GetCameraHandle();
		void DrawCross(BYTE* pData);

	public:
		CLucamDev();
		virtual ~CLucamDev();
	};
}

// extern CAM::CLucamDev Lucam;

#endif