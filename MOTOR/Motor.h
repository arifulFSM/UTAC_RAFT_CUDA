#pragma once

#include <string>

#include "COM.h"
#include "LOG/LogSys.h"
#include "SRC/DOSUtil.h"

#ifndef MAXQUE
#define MAXQUE 12
#define MAXQUEBUF 64
#endif

namespace MOT {
	struct SMotPar {
		enum ERESETPOS {
			ZEROPOS, MINPOS, HOMPOS, MAXPOS, RESETPOS
		}resetPos = ZEROPOS;
		bool polarity = true;
		float SPMM = 0;
		float now = 0; // current motor position
		bool bDirty = true;
		float min = 0, max = 0, home = 0;
		float acc = 0, vel = 0;
		float current = 0; // motor current
		int defLimit = 0;
		float rstPos = 0;
	};
	struct SMotRst {
		// motor reset sequence table
		float ph1 = 0, ph2 = 0, ph3 = 0;
		void SetPh(float a, float b, float c) {
			ph1 = a; ph2 = b; ph3 = c;
		}
	};

	enum class MAXIS {
		X, Y, Z, T, U, V, LAST
	};

	struct SQueItem {
		bool bActive = false;
		char msg[MAXQUEBUF] = { 0 }; //? must be in ASCII
	};

	class CMotor {
		SMotRst RstPar[int(MAXIS::LAST)];
		void Log(const std::wstring& msg, LOG::ELOGLEVEL lvl = LOG::ELOGLEVEL::NORMAL) {
			LgS.Log(msg.c_str(), lvl);
		}

	protected:
		bool SendBuf(const char* buf);
		void LimitCheckMvA(MAXIS nAxis, float& lpos);
		void LimitCheckMvR(MAXIS nAxis, float& dist);

	public:
		bool bSimu = true;
		std::wstring ID = L"";

	public:
		CCriticalSection CS, CS2;
		static SMotPar Mpar[short(MAXIS::LAST)];
		static short Maxis[short(MAXIS::LAST)];
		static std::wstring section[short(MAXIS::LAST)];

	protected:
		CCOM* pCom = nullptr; //? pointer only, do not delete

		bool SetCom(CCOM* p);
		bool WaitStop(MAXIS nAxis, DWORD tout);
		int Receive(char* Lookfor, char* chBuffer, int nSize, DWORD tout);
		int Receive(char* Lookfor1, char* Lookfor2, char* chBuffer, int nSize, DWORD tout);

	protected:
		bool ReadNow(MAXIS nAxis, float& pos);

	public:
		SQueItem Que[MAXQUE];
		bool bRun = false, bFrnSrchStop = true;
		void ComInMonitor();
		void StartMonitorThread();
		void RecvBuf();
		void Yld(DWORD dur);

	public:
		bool MoveA(MAXIS nAxis, float lpos, DWORD tout, bool bCheck = true);
		bool MoveR(MAXIS nAxis, float dist, DWORD tout, bool bCheck = true);
		bool Reset(MAXIS nAxis, MOT::SMotPar::ERESETPOS ResetPos, bool bCheck = false);
		bool WaitStopA(MAXIS nAxis, float lpos, DWORD tout);
		bool WaitStopR(MAXIS nAxis, float dist, DWORD tout);

	public:
		int Real2Dev(MAXIS id, float pos);
		float Dev2Real(MAXIS id, int pos);
		bool Initialize(MAXIS i, bool bSimu = false);
		bool Deinitialize(MAXIS i);

		static void LoadParam(MAXIS nAxis, const TCHAR* section);
		static void SaveParam(MAXIS nAxis, const TCHAR* section);

		CMotor();
		virtual ~CMotor();

		//20220406 / yukchiu
		bool StopAll();
		bool JogZEnable(bool bEn);
		bool JogZSpeed(float spd);
		bool JogZCommence();
		bool SetMicroRes(MAXIS nAxis, short MicroRes);
		bool SetPowerModeA(MAXIS nAxis, int PMVal);
		bool SetCurrent(MAXIS nAxis, float fCurrent);
		bool SetAcceleration(MAXIS nAxis, float fAcc);
		bool SetSpeed(MAXIS nAxis, float fSpeed);
		bool SetOutput(MAXIS nAxis, int nChan, bool bOnOff);
		bool SetDistance(MAXIS nAxis, int nDistance);

		bool devSetAbsPos(MAXIS nAxis, int lPos);
		bool GetInput(MAXIS nAxis, int nChan);
		bool devGetCurPos(MAXIS nAxis, float* pfPos);
		bool FeedToPosition(MAXIS nAxis);
		bool FeedToLength(MAXIS nAxis);
		bool Stop(MAXIS nAxis);
		bool WaitStop1(MAXIS nAxis1, DWORD tout);
		//bool WaitStop2(MAXIS nAxis1, MAXIS nAxis2, DWORD tout);
		bool DefineLimits(MAXIS nAxis, int nData);
		float GetAnalogInput(MAXIS nAxis);
		float GetSpeed(MAXIS nAxis);
	};
}