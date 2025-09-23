#include "pch.h"
#include "SRC/DOSUtil.h"
#include "LOG/LogSys.h"
#include "MotorCtrl.h"

using namespace MOT;

bool MOT::CMotorCtrl::Initialize() {
	//////////////////////////////////////////////////////////////////////
	for (int i = 0; i < int(MAXIS::LAST); i++) {
		CMotor::LoadParam(MAXIS(i), CMotor::section[i].c_str());
		CMotor::SaveParam(MAXIS(i), CMotor::section[i].c_str());
	}

	short port1 = 1, port2 = 2;
	std::wstring sec = _T("Ports");
	port1 = DosUtil.ReadCfgINI(sec.c_str(), _T("Tilt"), port1);
	DosUtil.WriteCfgINI(sec.c_str(), _T("Tilt"), port1);
	port2 = DosUtil.ReadCfgINI(sec.c_str(), _T("Stage"), port2);
	DosUtil.WriteCfgINI(sec.c_str(), _T("Stage"), port2);
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
#ifdef _SIMUMOTOR
	ComTilt.reset(new CCOM);
	ComStage.reset(new CCOM);
#else

#ifdef _SIMUTILT
	ComTilt.reset(new CCOM);
#else
	ComTilt.reset(new CCOM_D);
#endif

#ifdef _SIMUSTAGE
	ComStage.reset(new CCOM);
#else
	ComStage.reset(new CCOM_D);
#endif
#endif
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	bool bSimu = false;
#ifdef _SIMUTILT
	bSimu = true;
#endif // _SIMUTILT

	bool ret = true;
	if (ComTilt->Open(port1)) {
		LgS.Log(L"Starting Tilt ComMonitor");
		tilt.ID = L"Tilt";
		if (!bSimu) tilt.StartMonitorThread();
		tilt.Initialize(ComTilt.get(), bSimu);
	}
	else {
		ret = false;
		std::wstring str = L"Failed to open tilt comm port #" + std::to_wstring(port1);
		LgS.Log(str.c_str(), LOG::CERROR);
	}
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	bSimu = false;
#ifdef _SIMUSTAGE
	bSimu = true;
#endif // _SIMUSTAGE

	if (ComStage->Open(port2)) {
		LgS.Log(L"Starting Stage ComMonitor");
		stage.ID = L"Stage";
		if (!bSimu) stage.StartMonitorThread();
		stage.Initialize(ComStage.get(), bSimu);
	}
	else {
		ret = false;
		std::wstring str = L"Failed to open Stage comm port #" + std::to_wstring(port2);
		LgS.Log(str.c_str(), LOG::CERROR);
	}
	//////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	if (Piezo.Initialize()) {
		Piezo.GetPos_um();
	}
	else LgS.Log(L"Piezo failed initialization", LOG::CERROR);
	//////////////////////////////////////////////////////////////////////
	return ret;
}

bool MOT::CMotorCtrl::Deinitialize() {
	tilt.bRun = false; stage.bRun = false;
	while (!tilt.bFrnSrchStop) Sleep(10); while (!stage.bFrnSrchStop) Sleep(10);
	stage.Deinitialize(); tilt.Deinitialize();
	ComTilt->Close(); ComTilt.reset(nullptr);
	ComStage->Close(); ComStage.reset(nullptr);

	Piezo.Deinitialize();

	for (int i = 0; i < int(MAXIS::LAST); i++) {
		CMotor::SaveParam(MAXIS(i), CMotor::section[i].c_str());
	}
	return true;
}

MOT::CMotorCtrl::CMotorCtrl() {}

MOT::CMotorCtrl::~CMotorCtrl() {}

void MOT::CMotorCtrl::MoveR(MOT::MAXIS nAxis, float inc, DWORD tout, bool bCheck) {
	switch (nAxis) {
	case MAXIS::X:
	case MAXIS::Y:
	case MAXIS::Z:
		stage.MoveR(nAxis, inc, tout, bCheck);
		break;
	case MAXIS::T:
	case MAXIS::U:
	case MAXIS::V:
		tilt.MoveR(nAxis, inc, tout, bCheck);
		break;
	}
}

void MOT::CMotorCtrl::MoveA(MOT::MAXIS nAxis, float pos, DWORD tout, bool bCheck) {
	switch (nAxis) {
	case MAXIS::X:
	case MAXIS::Y:
	case MAXIS::Z:
		stage.MoveA(nAxis, pos, tout, bCheck);
		break;
	case MAXIS::T:
	case MAXIS::U:
	case MAXIS::V:
		tilt.MoveA(nAxis, pos, tout, bCheck);
		break;
	}
}