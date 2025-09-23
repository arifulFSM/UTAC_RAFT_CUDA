#pragma once

#include "MTH/Point3.h"
#include "Motor.h"

namespace MOT {
	class CTilt : public CMotor {
	public:
		MTH::SGeo Geo;

	public:
		CTilt();
		virtual ~CTilt();
		int Reset();
		int ResetXY0();
		bool SetZeroPos();
		bool Home(bool bCheck);
		bool GotoTUV(float tdist, float udist, float vdist, DWORD tout, bool bCheck);
		bool UpDownTUV(float incZ, DWORD tout, bool bCheck);
		bool DoTilts(MTH::SPoint3& p, MTH::SPoint3& N, DWORD tout, bool bCheck);

		bool Initialize(CCOM* pCom, bool bSimu = false);
		bool Deinitialize();

		void UpdatePositions();
		void SetN(MTH::SPoint3& N);
	};
}