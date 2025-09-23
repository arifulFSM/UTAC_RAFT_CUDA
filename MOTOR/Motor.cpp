#include "pch.h"
#include <string>
#include <thread>
#include "Motor.h"

using namespace MOT;

//20220406 / yukchiu
#define		SI_COMPORT			(1)
#define		DELIMITOR			('\r')
#define		MOTOR_ERROR_BASE	(0x10000)

#define		MIN_SPEED			(0.025f)
#define		MAX_SPEED			(50.0f)

#define		MIN_VELOC			(0.025f)
#define		MAX_VELOC			(30.0f)

const char chRevisionLevel[] = "RV";
const char chRequestStatus[] = "RS";
const char chMicrostepResolution[] = "MR";
const char chAcknowledgement[] = "00";
const char chStop[] = "ST";
const char chDistance[] = "DI";
const char chFeedToLength[] = "FL";
const char chFeedToPos[] = "FP";
const char chSetAbsPos[] = "SP";
const char chVelocity[] = "VE";
const char chJogSpeed[] = "JS";
const char chPowerUpCurrent[] = "PC";
const char chChangeCurrent[] = "CC";
const char chSetAcceleration[] = "AC";
const char chSetDeceleration[] = "DE";
const char chSetOutput[] = "SO";
const char chInputStatus[] = "IS";
const char chDefineLimits[] = "DL";
const char chInputHigh[] = "1";
const char chJogStart[] = "CJ";
const char chJogStop[] = "SJ";
const char chAnalogInput[] = "RA";
const char chSetPMValue[] = "PM";
const char chJogAcceleration[] = "JA";

MOT::SMotPar CMotor::Mpar[short(MAXIS::LAST)];
short CMotor::Maxis[short(MAXIS::LAST)] = { 0 };
std::wstring CMotor::section[int(MOT::MAXIS::LAST)];

bool MOT::CMotor::SetCom(CCOM* p) {
	CSingleLock Lock(&CS); Lock.Lock();
	pCom = p;
	return true;
}

void MOT::CMotor::ComInMonitor() {
	std::wstring str = ID + L": ComInMonitor thread started";
	LgS.Log(str.c_str());
	bRun = true; bFrnSrchStop = false;
	while (bRun) {
		if (!pCom->IsData()) { Sleep(5); continue; }
		bool bOK = false;
		for (int i = 0; i < MAXQUE; i++) {
			if (!Que[i].bActive) {
				CSingleLock Lock(&CS2); Lock.Lock();
				if (pCom->Read1(Que[i].msg, MAXQUEBUF, '\r', 10000) > 0) {
					Que[i].bActive = true;
					bOK = true;
					std::wstring wstr(MAXQUEBUF + 1, L'#');
					mbstowcs(&wstr[0], Que[i].msg, strlen(Que[i].msg));
					str = ID + std::wstring(L": recv ") + wstr;
					LgS.Log(str.c_str());
				}
				else {
					str = ID + L": ComInMonitor timeout";
					LgS.Log(str.c_str(), LOG::CERROR);
				}
				Lock.Unlock();
				break;
			}
		}
		if (!bOK) {
			str = ID + L": ComInMonitor out of queue space";
			LgS.Log(str.c_str(), LOG::CERROR);
		}
	}
	bFrnSrchStop = true;
}

void MOT::CMotor::StartMonitorThread() {
	std::thread thread(&CMotor::ComInMonitor, this);
	thread.detach();
}

void MOT::CMotor::RecvBuf() {
	std::wstring str;
	for (int i = 0; i < MAXQUE; i++) {
		if (Que[i].bActive) {
			std::wstring wstr(MAXQUEBUF + 1, L'#');
			mbstowcs(&wstr[0], Que[i].msg, strlen(Que[i].msg));
			str = ID + L": ** " + wstr;
			LgS.Log(str.c_str());
		}
	}
	str = ID + L": RecvBuf done";
	LgS.Log(str.c_str());
}

void MOT::CMotor::LoadParam(MAXIS nAxis, const TCHAR* section) {
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];
	Maxis[iAxis] = DosUtil.ReadCfgINI(section, _T("ID"), iAxis + 1);
	M.acc = DosUtil.ReadCfgINI(section, _T("Acceleration"), 25.f);
	M.vel = DosUtil.ReadCfgINI(section, _T("Velocity"), 4.f);
	M.current = DosUtil.ReadCfgINI(section, _T("Current"), 0.6f);
	M.defLimit = DosUtil.ReadCfgINI(section, _T("defLimit"), 2);
	M.polarity = DosUtil.ReadCfgINI(section, _T("Polarity"), 1);
	M.min = DosUtil.ReadCfgINI(section, _T("MinPos"), 0.0f);
	M.max = DosUtil.ReadCfgINI(section, _T("MaxPos"), 0.0f);
	M.home = DosUtil.ReadCfgINI(section, _T("HomePos"), 0.0f);
	M.SPMM = DosUtil.ReadCfgINI(section, _T("SPMM"), 0.0f);
	M.rstPos = DosUtil.ReadCfgINI(section, _T("RstPos"), 0.0f);
	M.resetPos = SMotPar::ERESETPOS(DosUtil.ReadCfgINI(section, _T("ResetPos"), float(SMotPar::ZEROPOS)));
}

void MOT::CMotor::SaveParam(MAXIS nAxis, const TCHAR* section) {
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];
	DosUtil.WriteCfgINI(section, _T("ID"), Maxis[iAxis]);
	DosUtil.WriteCfgINI(section, _T("Acceleration"), M.acc);
	DosUtil.WriteCfgINI(section, _T("Velocity"), M.vel);
	DosUtil.WriteCfgINI(section, _T("Current"), M.current);
	DosUtil.WriteCfgINI(section, _T("defLimit"), M.defLimit);
	DosUtil.WriteCfgINI(section, _T("Polarity"), M.polarity);
	DosUtil.WriteCfgINI(section, _T("MinPos"), M.min);
	DosUtil.WriteCfgINI(section, _T("MaxPos"), M.max);
	DosUtil.WriteCfgINI(section, _T("HomePos"), M.home);
	DosUtil.WriteCfgINI(section, _T("SPMM"), M.SPMM);
	DosUtil.WriteCfgINI(section, _T("RstPos"), M.rstPos);
	DosUtil.WriteCfgINI(section, _T("ResetPos"), M.resetPos);
}

bool MOT::CMotor::WaitStopA(MAXIS nAxis, float lpos, DWORD tout) {
	int iAxis = int(nAxis);
	if (!WaitStop1(nAxis, tout)) {
		Mpar[iAxis].bDirty = true; return false;
	}
	ReadNow(nAxis, lpos);
	//////////////////////////////////////////////////////////////////////////
	if (bSimu) { Mpar[iAxis].now = lpos; }
	//////////////////////////////////////////////////////////////////////////
	return true;
}

bool MOT::CMotor::WaitStopR(MAXIS nAxis, float dist, DWORD tout) {
	int iAxis = int(nAxis);
	if (!WaitStop1(nAxis, tout)) {
		Mpar[iAxis].bDirty = true; return false;
	}
	ReadNow(nAxis, dist);
	//////////////////////////////////////////////////////////////////////////
	if (bSimu) { Mpar[iAxis].now += dist; }
	//////////////////////////////////////////////////////////////////////////
	return true;
}

int MOT::CMotor::Real2Dev(MAXIS id, float pos) {
	if (Mpar[int(id)].polarity) pos = -pos;
	return int(Mpar[int(id)].SPMM * pos);
}

float MOT::CMotor::Dev2Real(MAXIS id, int pos) {
	if (Mpar[int(id)].polarity) pos = -pos;
	return pos / Mpar[int(id)].SPMM;
}

bool MOT::CMotor::Initialize(MAXIS id, bool bSimu) {
	this->bSimu = bSimu;
	int i = int(id);
	MOT::SMotPar& M = Mpar[i];
	short OK = 0;
	if (!SetPowerModeA(id, 2)) {
		if (!SetPowerModeA(id, 2)) {
			if (!SetPowerModeA(id, 2)) {
				std::wstring s((const TCHAR*)&section[i][0], sizeof(wchar_t) / sizeof(char) * section[i].size());
				Log(L"Cannot PM axis " + s, LOG::CERROR);
				OK = 1 << i;
			}
		}
	}
	SetCurrent(id, M.current);
	DefineLimits(id, M.defLimit);
	SetSpeed(id, M.vel);
	SetAcceleration(id, M.acc);
	SetMicroRes(id, 8);
	if (OK != 0) return false;
	return true;
}

bool MOT::CMotor::Deinitialize(MAXIS i) {
	//for (int i = 0; i < LASTM; i++) {
	SetCurrent(i, 0);
	//}
	return true;
}

MOT::CMotor::CMotor() {
	// hard coded, v.sure no change needed for its intended use/modal
	Maxis[int(MAXIS::X)] = 1;
	Maxis[int(MAXIS::Y)] = 2;
	Maxis[int(MAXIS::Z)] = 3;
	Maxis[int(MAXIS::T)] = 3;
	Maxis[int(MAXIS::U)] = 1;
	Maxis[int(MAXIS::V)] = 2;
	RstPar[int(MAXIS::X)].SetPh(-360.f, 4.f, -10.f);
	RstPar[int(MAXIS::Y)] = RstPar[int(MAXIS::X)];
	RstPar[int(MAXIS::Z)].SetPh(-50.f, 2.f, -5.f);
	RstPar[int(MAXIS::T)].SetPh(-50.f, 1.f, -5.f);
	RstPar[int(MAXIS::U)] = RstPar[int(MAXIS::T)];
	RstPar[int(MAXIS::V)] = RstPar[int(MAXIS::T)];
	section[int(MAXIS::X)] = _T("XMOTOR");
	section[int(MAXIS::Y)] = _T("YMOTOR");
	section[int(MAXIS::Z)] = _T("ZMOTOR");
	section[int(MAXIS::T)] = _T("T1MOTOR");
	section[int(MAXIS::U)] = _T("T2MOTOR");
	section[int(MAXIS::V)] = _T("T3MOTOR");
}

MOT::CMotor::~CMotor() {}

// void MOT::CMotor::Log(const std::wstring& msg, CLogSys::ELOGLEVEL lvl = CLogSys::NORMAL) {
// 	//TODO: to be completed
// }

bool MOT::CMotor::ReadNow(MAXIS nAxis, float& pos) {
	char chBuffer[32];
	short iAxis = Maxis[int(nAxis)];
	MOT::SMotPar& M = Mpar[int(nAxis)];

	//////////////////////////////////////////////////////////////////////////
	if (bSimu) { M.bDirty = false; return true; }
	//////////////////////////////////////////////////////////////////////////

	sprintf(chBuffer, "%d%s", iAxis, chSetAbsPos);
	if (!SendBuf(chBuffer)) { M.bDirty = true; return false; }
	Yld(20);
	char chLookFor[16];
	memcpy(chLookFor, chBuffer, 3); chLookFor[3] = 0;
	int nInput = Receive(chLookFor, chBuffer, sizeof(chBuffer), 10000);
	if (nInput <= 0) { M.bDirty = true; return false; }
	pos = Dev2Real(nAxis, (atoi(&chBuffer[4])));
	M.now = pos;
	M.bDirty = false;
	return true;
}

bool MOT::CMotor::MoveA(MAXIS nAxis, float lpos, DWORD tout, bool bCheck) {
	char chBuffer[64];
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];

	//////////////////////////////////////////////////////////////////////////
	if (bSimu) { M.now = lpos; M.bDirty = false; return true; }
	//////////////////////////////////////////////////////////////////////////

	if (bCheck) LimitCheckMvA(nAxis, lpos);
	sprintf(chBuffer, "%d%s%d\r%d%s",
		Maxis[iAxis], chDistance, Real2Dev(nAxis, lpos),
		Maxis[iAxis], chFeedToPos);
	if (!SendBuf(chBuffer)) { M.bDirty = true; return false; }
	if (tout > 0) return WaitStopA(nAxis, lpos, tout);
	else M.bDirty = true;
	return true;
}

bool MOT::CMotor::MoveR(MAXIS nAxis, float dist, DWORD tout, bool bCheck) {
	char chBuffer[64];
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];

	//////////////////////////////////////////////////////////////////////////
	if (bSimu) { M.now += dist; M.bDirty = false; }
	//////////////////////////////////////////////////////////////////////////

	if (bCheck) LimitCheckMvR(nAxis, dist);
	sprintf(chBuffer, "%d%s%d\r%d%s",
		Maxis[iAxis], chDistance, Real2Dev(nAxis, dist),
		Maxis[iAxis], chFeedToLength);
	if (!SendBuf(chBuffer)) { M.bDirty = true; return false; }
	if (tout > 0) return WaitStopR(nAxis, dist, tout);
	else M.bDirty = true;
	return true;
}

bool MOT::CMotor::Reset(MAXIS nAxis, MOT::SMotPar::ERESETPOS ResetPos, bool bCheck) {
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];
	SetSpeed(nAxis, M.vel / 4.0f);
	if (MoveR(nAxis, RstPar[iAxis].ph1, 120000, bCheck)) {
		SetSpeed(nAxis, M.vel / 8.f);
		if (MoveR(nAxis, RstPar[iAxis].ph2, 60000, bCheck)) {
			SetSpeed(nAxis, M.vel / 12.f);
			if (MoveR(nAxis, RstPar[iAxis].ph3, 60000, bCheck)) {
				SetSpeed(nAxis, M.vel);
				float lpos;
				switch (ResetPos) {
				case SMotPar::MINPOS: lpos = M.min; break;
				case SMotPar::HOMPOS: lpos = M.home; break;
				case SMotPar::MAXPOS: lpos = M.max; break;
				case SMotPar::RESETPOS: lpos = M.rstPos; break;
				default: lpos = 0; break;
				}
				devSetAbsPos(nAxis, Real2Dev(nAxis, lpos));
				ReadNow(nAxis, lpos);
				if (bSimu) { M.now = lpos; }
				return true;
			}
		}
	}
	SetSpeed(nAxis, M.vel);
	std::wstring s((const TCHAR*)&section[iAxis][0], sizeof(wchar_t) / sizeof(char) * section[iAxis].size());
	Log(L"Failed to reset axis " + s, LOG::CERROR);
	return false;
}

bool MOT::CMotor::SendBuf(const char* buf) {
	CSingleLock Lock(&CS); Lock.Lock();
	if (pCom != nullptr) {
		if (pCom->Write(buf, int(strlen(buf)))) {
			pCom->Write("\r", 1);
			std::wstring wstr(strlen(buf) + 1, L'#');
			mbstowcs(&wstr[0], buf, strlen(buf));
			std::wstring str = ID + L": send " + wstr;
			LgS.Log(str.c_str());
			return true;
		}
	}
	return false;
}

void MOT::CMotor::LimitCheckMvA(MAXIS nAxis, float& lpos) {
	std::wstring msg;
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];
	float pos = lpos;
	if (pos < M.min) {
		lpos = M.min;
		msg = ID + L": Would hit low limit of axis " + std::to_wstring(iAxis + 1) + L". Position changed to " + std::to_wstring(lpos);
		Log(msg, LOG::WARNING);
		AfxMessageBox(L"Reached low limit of axis", MB_ICONINFORMATION);
		//ASSERT(0);
	}
	else if (pos > M.max) {
		lpos = M.max;
		msg = ID + L": Would hit high limit of axis " + std::to_wstring(iAxis + 1) + L". Position changed to " + std::to_wstring(lpos);
		Log(msg, LOG::WARNING);
		AfxMessageBox(L"Reached high limit of axis", MB_ICONINFORMATION);
		//ASSERT(0);
	}
}

void MOT::CMotor::LimitCheckMvR(MAXIS nAxis, float& dist) {
	std::wstring msg;
	int iAxis = int(nAxis);
	MOT::SMotPar& M = Mpar[iAxis];
	float v = (M.now + dist);
	if (v < M.min) {
		dist = -(M.now - M.min);
		msg = L"Would hit low limit of axis " + std::to_wstring(iAxis) + L". Position changed to " + std::to_wstring(dist);
		Log(msg, LOG::WARNING);
		AfxMessageBox(L"Reached low limit of axis", MB_ICONINFORMATION);
		//ASSERT(0);
	}
	else if (v > M.max) {
		dist = M.max - M.now;
		msg = L"Would hit high limit of axis " + std::to_wstring(iAxis) + L". Position changed to " + std::to_wstring(dist);
		Log(msg, LOG::WARNING);
		AfxMessageBox(L"Reached high limit of axis", MB_ICONINFORMATION);
		//ASSERT(0);
	}
}

void MOT::CMotor::Yld(DWORD dur) {
	MSG msg;
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < dur) {
		//? User message will be removed too
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

int MOT::CMotor::Receive(char* Lookfor1, char* Lookfor2, char* chBuffer, int nSize, DWORD tout) {
	std::wstring str;
	bool bOK = false;
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < tout) {
		for (int i = 0; i < MAXQUE; i++) {
			if (Que[i].bActive) {
				if (strstr(Que[i].msg, Lookfor1)) {
					if (nSize > MAXQUEBUF) nSize = MAXQUEBUF;
					CSingleLock Lock(&CS2); Lock.Lock();
					memcpy(chBuffer, Que[i].msg, nSize);
					Que[i].bActive = false;
					bOK = true;
					//str = ID + ": ** " + std::string(chBuffer); LgS.Log(str.c_str());
					Lock.Unlock();
					return int(strlen(chBuffer));
				}
				if (strstr(Que[i].msg, Lookfor2)) {
					if (nSize > MAXQUEBUF) nSize = MAXQUEBUF;
					CSingleLock Lock(&CS2); Lock.Lock();
					memcpy(chBuffer, Que[i].msg, nSize);
					Que[i].bActive = false;
					bOK = true;
					//str = ID + ": ** " + std::string(chBuffer); LgS.Log(str.c_str());
					Lock.Unlock();
					return int(strlen(chBuffer));
				}
			}
		}
		Yld(10);
	}
	str = ID + L": ** Recieve timeout";
	LgS.Log(str.c_str(), LOG::CERROR);
	return 0;
}

int MOT::CMotor::Receive(char* Lookfor, char* chBuffer, int nSize, DWORD tout) {
	std::wstring str;
	bool bOK = false;
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < tout) {
		for (int i = 0; i < MAXQUE; i++) {
			if (Que[i].bActive) {
				if (strstr(Que[i].msg, Lookfor)) {
					if (nSize > MAXQUEBUF) nSize = MAXQUEBUF;
					CSingleLock Lock(&CS2); Lock.Lock();
					memcpy(chBuffer, Que[i].msg, nSize);
					Que[i].bActive = false;
					bOK = true;
					//str = ID + ": ** " + std::string(chBuffer); LgS.Log(str.c_str());
					Lock.Unlock();
					return int(strlen(chBuffer));
				}
			}
		}
		Yld(10);
	}
	str = ID + L": ** Receive timeout";
	LgS.Log(str.c_str(), LOG::CERROR);
	return 0;
}

bool MOT::CMotor::WaitStop(MAXIS nAxis, DWORD tout) {
	//////////////////////////////////////////////////////////////////////////
	if (bSimu) return true;
	//////////////////////////////////////////////////////////////////////////

	short iAxis = Maxis[int(nAxis)];
	char chBuffer[48];
	sprintf(chBuffer, "%d%s", iAxis, chRequestStatus);
	if (!SendBuf(chBuffer)) return false;
	Yld(20);
	char chLookFor1[4], chLookFor2[4];
	sprintf(chLookFor1, "%dR", iAxis);
	sprintf(chLookFor2, "%dM", iAxis);
	//memcpy(chLookFor, chBuffer, 3); chLookFor[3] = 0;
	int nInput = Receive(chLookFor1, chLookFor2, chBuffer, sizeof(chBuffer), tout);
	if (nInput <= 0) { return false; }
	if (strchr(chBuffer, 'R') != nullptr) {
		//float pos; //ReadNow(nAxis, pos);
		return true;
	}
	return false;
}

bool MOT::CMotor::StopAll() {
	return SendBuf("ST");
}

bool MOT::CMotor::JogZEnable(bool bEn) {
	char chBuffer[92];
	if (bEn) {
		sprintf(chBuffer, "%dJE", Maxis[int(MAXIS::Z)]);
		return SendBuf(chBuffer);
	}
	sprintf(chBuffer, "%dJD", Maxis[int(MAXIS::Z)]);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::JogZSpeed(float spd) {
	char chBuffer[92];
	sprintf(chBuffer, "%dJS%.3f", Maxis[int(MAXIS::Z)], spd);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::JogZCommence() {
	char chBuffer[92];
	sprintf(chBuffer, "%dCJ", Maxis[int(MAXIS::Z)]);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::SetPowerModeA(MAXIS nAxis, int PMVal) {
	//////////////////////////////////////////////////////////////////////////
	if (bSimu) return true;
	//////////////////////////////////////////////////////////////////////////

	char chBuffer[64];
	short iAxis = Maxis[int(nAxis)];
	sprintf(chBuffer, "%d%s%d\r%d%s",
		iAxis, chSetPMValue, PMVal, iAxis, chSetPMValue
	);
	if (SendBuf(chBuffer)) {
		Yld(20);
		char chLookFor[16];
		sprintf(chLookFor, "%d%s", iAxis, chSetPMValue);
		if (Receive(chLookFor, chBuffer, 32, 10000) > 0) {
			if (chBuffer[4] == '2') return true;
		}
	}
	return false;
}

bool MOT::CMotor::SetAcceleration(MAXIS nAxis, float fAcc) {
	if (fAcc < MIN_VELOC) fAcc = MIN_VELOC;
	if (fAcc > MAX_VELOC) fAcc = MAX_VELOC;
	char chBuffer[92];
	sprintf(chBuffer, "%d%s%.2f\r%d%s%.2f\r%d%s%.2f",
		Maxis[int(nAxis)], chSetAcceleration, fAcc,
		Maxis[int(nAxis)], chSetDeceleration, fAcc,
		Maxis[int(nAxis)], chJogAcceleration, fAcc);
	//if (!Send(chBuffer)) return false;
	return SendBuf(chBuffer);
}

bool MOT::CMotor::SetSpeed(MAXIS nAxis, float fSpeed) {
	char chBuffer[32];
	if (fSpeed < MIN_SPEED) fSpeed = MIN_SPEED;
	if (fSpeed > MAX_SPEED) fSpeed = MAX_SPEED;
	sprintf(chBuffer, "%d%s%.3f", Maxis[int(nAxis)], chVelocity, fSpeed);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::SetOutput(MAXIS nAxis, int nChan, bool bOnOff) {
	if ((nChan < 1) || (nChan > 3)) return false;
	char chBuffer[32];
	sprintf(chBuffer, "%d%s%d%s", Maxis[int(nAxis)], chSetOutput, nChan, bOnOff ? "H" : "L");
	return SendBuf(chBuffer);
}

bool MOT::CMotor::SetCurrent(MAXIS nAxis, float fCurrent) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s%.3f", Maxis[int(nAxis)], chChangeCurrent, fCurrent);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::SetDistance(MAXIS nAxis, int nDistance) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s%i", Maxis[int(nAxis)], chDistance, nDistance);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::SetMicroRes(MAXIS nAxis, short MicroRes) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s%d", Maxis[int(nAxis)], chMicrostepResolution, MicroRes);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::devSetAbsPos(MAXIS nAxis, int lPos) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s%d", Maxis[int(nAxis)], chSetAbsPos, lPos);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::GetInput(MAXIS nAxis, int nChan) {
	//////////////////////////////////////////////////////////////////////////
	if (bSimu) return true;
	//////////////////////////////////////////////////////////////////////////

	char chBuffer[32];
	if (nChan < 1) return false;
	if (nChan > 8) return false;
	short iAxis = Maxis[int(nAxis)];
	sprintf(chBuffer, "%d%s", iAxis, chInputStatus);
	if (!SendBuf(chBuffer)) return false;
	Yld(20);
	char chLookFor[16];
	memcpy(chLookFor, chBuffer, 3); chLookFor[3] = 0;
	if (Receive(chLookFor, chBuffer, sizeof(chBuffer), 10000) == 12) {
		return (*(chBuffer + 12 - nChan) == '1');
	}
	return false;
}

bool MOT::CMotor::devGetCurPos(MAXIS nAxis, float* pfPos) {
	if (Mpar[int(nAxis)].bDirty) {
		if (ReadNow(nAxis, *pfPos)) return true;
		return false;
	}
	else *pfPos = Mpar[int(nAxis)].now;
	return true;
}

bool MOT::CMotor::FeedToPosition(MAXIS nAxis) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s", Maxis[int(nAxis)], chFeedToPos);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::FeedToLength(MAXIS nAxis) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s", Maxis[int(nAxis)], chFeedToLength);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::Stop(MAXIS nAxis) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s", Maxis[int(nAxis)], chStop);
	return SendBuf(chBuffer);
}

bool MOT::CMotor::WaitStop1(MAXIS nAxis1, DWORD tout) {
	DWORD tick = GetTickCount();
	while (!WaitStop(nAxis1, tout)) {
		if ((GetTickCount() - tick) > tout) {
			Log(ID + L": Axis " + std::to_wstring(int(nAxis1) + 1) + L" timeout!", LOG::WARNING);
			return false;
		}
		Yld(200);
	}
	return true;
}

bool MOT::CMotor::DefineLimits(MAXIS nAxis, int nData) {
	char chBuffer[32];
	sprintf(chBuffer, "%d%s%d", Maxis[int(nAxis)], chDefineLimits, nData);
	return SendBuf(chBuffer);
}

float MOT::CMotor::GetAnalogInput(MAXIS nAxis) {
	//////////////////////////////////////////////////////////////////////////
	if (bSimu) return 0;
	//////////////////////////////////////////////////////////////////////////

	char chBuffer[32];
	short iAxis = Maxis[int(nAxis)];
	sprintf(chBuffer, "%d%s", iAxis, chAnalogInput);
	if (!SendBuf(chBuffer)) return 0;
	Yld(20);
	char chLookFor[16];
	memcpy(chLookFor, chBuffer, 3); chLookFor[3] = 0;
	int n = Receive(chLookFor, chBuffer, sizeof(chBuffer), 10000);
	if (n >= 8) {
		for (int i = 0; i < 4; i++) {
			chBuffer[i] = chBuffer[i + 4];
		}
		chBuffer[4] = 0;
		return float(atof(chBuffer));
	}
	else {
		sprintf(chBuffer, "%d%s", iAxis, chAnalogInput);
		if (!SendBuf(chBuffer)) return 0;
		Yld(20);
		n = Receive(chLookFor, chBuffer, sizeof(chBuffer), 10000);
		if ((n >= 8) && (n <= 9)) {
			for (int i = 0; i < 4; i++) {
				chBuffer[i] = chBuffer[i + 4];
			}
			chBuffer[4] = 0;
			return float(atof(chBuffer));
		}
		return 0;
	}
}

float MOT::CMotor::GetSpeed(MAXIS nAxis) {
	char chBuffer[32], chTemp[32];
	short iAxis = Maxis[int(nAxis)];
	sprintf(chBuffer, "%dVE", iAxis);
	if (!SendBuf(chBuffer)) return 0;
	Yld(20);
	char chLookFor[16];
	memcpy(chLookFor, chBuffer, 3); chLookFor[3] = 0;
	int n = Receive(chLookFor, chBuffer, sizeof(chBuffer), 10000);
	if (n > 4) {
		for (int i = 0; i < (n - 4); i++) {
			chTemp[i] = chBuffer[i + 4];
		}
		chTemp[n - 4] = 0;
		return float(atof(chTemp));
	}
	ASSERT(0);
	return 0;
}