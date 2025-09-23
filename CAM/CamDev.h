#pragma once

#include "IM/IM.h"
#include "Camera.h"

namespace CAM {
	enum CAMTYPE {
		SIMU, LUCAM, BASCOLRCAM, BASMONOCAM
	};

	class CCamDev {
	private:
		CAMTYPE CType = CAMTYPE::SIMU;

		void Clear(ECAM ID);

	public:
		CCamera* pCm[ECAM::LASTCAM] = { nullptr }; // pointer only [11/29/2021 yuenl]

	public:
		CCamDev();
		~CCamDev();

		BOOL StartDev(CAM::CAMTYPE Type, ECAM ID);
		BOOL StopDev(ECAM ID);

		CCamera* GetCamera(ECAM ID);
		CString GetCameraInfo(ECAM ID);
		BOOL Grab(IMGL::CIM& Im, ECAM ID, int subSample);
		BOOL ExecuteTrigger(ECAM ID);
		BOOL SetTriggerMode(ECAM ID, bool isTrigOn);
		BOOL GetBitmapImage(IMGL::CIM& Im, ECAM ID);
	};
}

// extern CAM::CCamDev CamDev;
