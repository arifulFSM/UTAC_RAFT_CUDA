enum EUpdateHint {
	UpdateHint_All = 0
	, UpdateHint_Image = 1
	, UpdateHint_Feature = 2
	, UpdateHint_DeviceList = 4
};

#include "pch.h"
#include "resource.h"
#include "../CAM/Camera.h"
#include<pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#include<pylon/PylonGUI.h>
#endif
#include <pylon/BaslerUniversalInstantCamera.h>
#include "CameraProp.h"

#ifndef _SIMUBASMONOCAM

//////////////////////////////////////////////////////////////////////////
// Actual camera device [1/1/2020 yuenl]
//////////////////////////////////////////////////////////////////////////

namespace CAM {
	class CBaslerMonoCam : public CCamera
		, public Pylon::CImageEventHandler             // Allows you to get informed about received images and grab errors.
		, public Pylon::CConfigurationEventHandler     // Allows you to get informed about device removal.
		, public Pylon::CCameraEventHandler {
		BOOL bInit = FALSE;

		void CloseHandle();

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
		BOOL ExecuteTrigger() { return TRUE; }
		BOOL SetTriggerMode(bool isTrigOn) { return TRUE; }
		BOOL GetBitmapImage(IMGL::CIM& Im) { return TRUE; }
		void propertyPageDisplay();

	public:
		HANDLE GetCameraHandle();
		void DrawCross(BYTE* pData);

	public:
		CBaslerMonoCam();
		virtual ~CBaslerMonoCam();

	public:
		Pylon::CBaslerUniversalInstantCamera m_camera;
		Pylon::DeviceInfoList_t m_devices;
		// The grab result retrieved from the camera
		Pylon::CGrabResultPtr m_ptrGrabResult;
		// The grab result as a windows DIB to be displayed on the screen
		Pylon::CPylonBitmapImage m_bitmapImage, tmp_bitmapimage;
		Pylon::CAcquireSingleFrameConfiguration m_singleConfiguration;
		Pylon::CAcquireContinuousConfiguration m_continuousConfiguration;
		// Protects members.
		mutable CCriticalSection m_MemberLock;
		BOOL StartStream();
		BOOL StopStream();
		BOOL GrabOneFrame(IMGL::CIM& Im);
		void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult);
		void Yld(int _tOut);
		bool isImgCapSuccess = false;
		IMGL::CIM ImgStream;
		SCtx Ctx;
		void DisplayImage();
	};
}

// extern CAM::CBaslerMonoCam BasMono;

#endif