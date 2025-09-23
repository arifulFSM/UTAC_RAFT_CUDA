#include "pch.h"

#include "Dev.h"
#include "CAM/CamDev.h"
#include "LOG/LogSys.h"

CDev Dev;

void CDev::LoadParam() {
	CDosUtil DosUtil;  CString sec;
	sec = _T("Camera");
	Type1 = CAM::CAMTYPE(DosUtil.ReadCfgINI(sec, _T("Type1"), int(Type1)));
	Type2 = CAM::CAMTYPE(DosUtil.ReadCfgINI(sec, _T("Type2"), int(Type2)));
}

void CDev::SaveParam() {
	CDosUtil DosUtil;  CString sec;
	sec = _T("Camera");
	DosUtil.WriteCfgINI(sec, _T("Type1"), int(Type1));
	DosUtil.WriteCfgINI(sec, _T("Type2"), int(Type2));
}

CDev::CDev() {}

CDev::~CDev() {}

bool CDev::Initialize() {
	std::time_t t = std::time(nullptr);
	std::tm* tm = std::localtime(&t);
	LgS.Initialize(tm);
	LgS.Log(L"Log system started");
	LoadParam(); SaveParam();
	BOOL statPriCam = Cam.StartDev(Type1, CAM::PRICAM);
	CString str = L"";
	str.Format(L"PRICAM STATUS: %d [0: Not Open | 1 : Open]", statPriCam);
	LgS.Log(str);
	BOOL statSecCam = Cam.StartDev(Type2, CAM::SECCAM);
	str.Format(L"SECCAM STATUS: %d [0: Not Open | 1 : Open]", statSecCam);
	LgS.Log(str);
	MC.reset(new MOT::CMotorCtrl());
	if (!MC->Initialize()) {
		Log("Fail to initialize motion controller", LOG::CERROR);
		return false;
	}
	return true;
}

bool CDev::Deinitialize() {
	Cam.StopDev(CAM::PRICAM);
	Cam.StopDev(CAM::SECCAM);

	MC->Deinitialize();
	SaveParam();
	LgS.Log(L"Log system stopped");
	LgS.Deinitialize();
	return true;
}

MOT::CStage& CDev::GetStage() {
	return MC->stage;
}

MOT::CTilt& CDev::GetTilt() {
	return MC->tilt;
}

MOT::CPiezo& CDev::GetPiezo() {
	return MC->Piezo;
}