#pragma once

#include "COM_D.h"
#include "Stage.h"
#include "Tilt.h"
#include "Piezo.h"
#include "PriorPiezo.h"
#include "CEOPiezo.h"

namespace MOT {
	//#define _SIMUMOTOR
	//#define _SIMUTILT
	//#define _SIMUSTAGE
	class CMotorCtrl {
	public:
		std::unique_ptr<CCOM> ComStage;
		std::unique_ptr<CCOM> ComTilt;

		CStage stage;
		CTilt tilt;
		CPiezo Piezo;
		CPriorPiezo pPiezo;
		CEOPiezo EOPiezo;

	public:
		bool Initialize();
		bool Deinitialize();

		CMotorCtrl();
		virtual ~CMotorCtrl();

	public:
		void MoveR(MOT::MAXIS nAxis, float inc, DWORD tout, bool bCheck = true);
		void MoveA(MOT::MAXIS nAxis, float pos, DWORD tout, bool bCheck = true);
	};
}