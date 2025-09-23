#pragma once
#include "Motor.h"

namespace MOT {
	class CStage : public CMotor {
		float spd = 0;
	public:
		// load param/ save param
		// id is in ini file
		bool Initialize(CCOM* pCom, bool bSimu = false);
		bool Deinitialize();

		CStage();
		virtual ~CStage();

	public:
		void Polar2Cart(float R, float A, float& X, float& Y);
		void Cart2Polar(float X, float Y, float& R, float& A);

	public:
		bool Home(bool bCheck);
		bool HomeZ();
		bool GotoXY(float xdist, float ydist, DWORD tout, bool bCheck);
		bool GotoRA(float radius, float angle, DWORD tout, bool bCheck);
		int Reset();
		int ResetXY();
		int ResetXY0();
		int ResetZ();
		bool SetZeroPos();
		void LED(bool bOn);
		void Laser(bool bOn);
		void SHUTTER(bool bOn); // 12052023
		void UpdatePositions();

		bool SetupZJog(float speed, bool bJogUp);
		bool StartZJog();
		bool StopZJog();
	};
}