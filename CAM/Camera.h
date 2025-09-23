#pragma once

#include "IM\IM.h"
#include "CamPar.h"
#include "LucamImg.h"

namespace CAM {
	//#define _SIMULUCAM

		//////////////////////////////////////////////////////////////////////////
		// all Camera devices base class [1/1/2020 yuenl]
		//////////////////////////////////////////////////////////////////////////
	struct SCam {
		short ID = 0; // current camera ID [6/9/2021 FSM]
		CString name = L"";
		void LoadParam(int n);
		void SaveParam(int n);
	};

	struct SCtx {
		HWND hWnd = 0;
		CRect rc;
	};

	class CCamera {
	protected:
		float fFrameRate = 15;

	public:
		BOOL bIsStream = FALSE;
		USHORT subSampling = 2;
		BOOL bCameraReversed = FALSE;
		BOOL bGrab = FALSE, bStore = FALSE;
		int Wd = 256, Ht = 256, Bpp = 24;

		SCamPar Par; // support multiple camera [6/9/2021 FSM]
		SCam SCaM;
		ECAM eID = ECAM::INVD;

	public:
		virtual BOOL IsValid();
		virtual short GetSubSample() { return 1; }
		virtual CString GetCameraInfo();
		virtual CString GetCameraFeatureInfo();

	public:
		void LoadParam();
		void SaveParam();
		void Initialize(ECAM ID);
		void Deinitialize(ECAM ID);

	public:
		virtual BOOL AutoExposure();
		virtual BOOL DeinitCapture(USHORT ID);
		virtual BOOL Gamma(BOOL bOn);
		virtual BOOL InitCapture(USHORT ID, short subSampling);

		virtual BOOL SetBrightness(float bright);
		virtual BOOL SetContrast(float contrast);
		virtual BOOL SetExposure(float exposure);
		virtual BOOL SetGamma(float gamma);
		virtual BOOL SetHue(float hu);
		virtual BOOL SetProperties();
		virtual BOOL SetSaturation(float saturate);
		virtual BOOL StartStream(SCtx& Ctx, USHORT ID);
		virtual BOOL StopStream(SCtx& Ctx, USHORT ID);
		virtual BOOL StartStream(); // For Basler Cam
		virtual BOOL StopStream(); // For Basler Cam
		virtual BOOL GrabOneFrame(IMGL::CIM& Im); // For Basler Cam
		virtual BOOL WhiteBalance();
		virtual float GetBrightness();
		virtual float GetContrast();
		virtual float GetExposure();
		virtual float GetGamma();
		virtual float GetHue();
		virtual float GetSaturation();
		virtual BOOL SetGain(float ga);
		virtual float GetGain();
		virtual BOOL AutoGain();
		virtual bool SetTrigger() { return true; }

	public:
		// camera specific
		virtual BOOL GetCamProp(SCamProp& prop);
		virtual BOOL SetCamProp(SCamProp& prop);
		virtual BOOL StartFastGrab(SCamProp& prop, int subSample);
		virtual BOOL StopFastGrab();
		virtual BOOL FastGrab(IMGL::CIM& Im, SCamProp& prop);
		virtual BOOL FastGrab(SCamProp& prop);
		virtual BOOL Convert24(IMGL::CIM& Im);

	public:
		virtual bool LucamPropPage();
		virtual bool LucamVideoFormat();

	public:
		virtual BOOL GrabOneFrame(IMGL::CIM& Im, int subSample);
		virtual BOOL ExecuteTrigger() { return TRUE; }
		virtual BOOL SetTriggerMode(bool isTrigOn) { return TRUE; }
		virtual BOOL SaveOneFrame(char* filename, int subSample);
		virtual void propertyPageDisplay() { return; }
		virtual BOOL GetBitmapImage(IMGL::CIM& Im) { return TRUE; }

	public:
		CCamera();
		virtual ~CCamera();
	};
}

#ifdef _SIMULUCAM
extern CAM::CCamera Lucam;
#endif