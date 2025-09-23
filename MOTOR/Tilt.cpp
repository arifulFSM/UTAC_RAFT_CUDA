#include "pch.h"
#include "Tilt.h"
#include <thread>

using namespace MOT;

bool MOT::CTilt::Initialize(CCOM* pCom, bool bSimu) {
	SetCom(pCom);
	for (short i = int(MAXIS::T); i < int(MAXIS::LAST); i++) {
		CMotor::Initialize(MAXIS(i), bSimu);
	}
	return true;
}

bool MOT::CTilt::Deinitialize() {
	for (short i = int(MAXIS::T); i < int(MAXIS::LAST); i++) {
		CMotor::Deinitialize(MAXIS(i));
	}
	return true;
}

void MOT::CTilt::UpdatePositions() {
	float pos;
	CMotor::ReadNow(MAXIS::T, pos);
	CMotor::ReadNow(MAXIS::U, pos);
	CMotor::ReadNow(MAXIS::V, pos);
}

bool MOT::CTilt::UpDownTUV(float incZ, DWORD tout, bool bCheck) {
	char chBuffer[196];
	if (bCheck) {
		LimitCheckMvR(MAXIS::T, incZ); LimitCheckMvR(MAXIS::U, incZ); LimitCheckMvR(MAXIS::V, incZ);
	}
	int iAxis = int(MAXIS::T);
	MOT::SMotPar& M = Mpar[iAxis];
	sprintf(chBuffer, "%dDI%d\r%dDI%d\r%dDI%d\rFL",
		Maxis[int(MAXIS::T)], Real2Dev(MAXIS::T, incZ),
		Maxis[int(MAXIS::U)], Real2Dev(MAXIS::U, incZ),
		Maxis[int(MAXIS::V)], Real2Dev(MAXIS::V, incZ));
	if (!SendBuf(chBuffer)) { M.bDirty = true; return false; }
	if (bSimu) { M.now += incZ; M.bDirty = false; return true; }
	WaitStopR(MAXIS::T, incZ, tout); WaitStopR(MAXIS::U, incZ, tout); WaitStopR(MAXIS::V, incZ, tout);
	return true;
}

bool MOT::CTilt::Home(bool bCheck) {
	SMotPar* p = &CMotor::Mpar[int(MAXIS::T)];
	if (GotoTUV(p->home, (p + 1)->home, (p + 2)->home, 10000, bCheck)) {
		Geo.Zh = 0;
	}
	UpdatePositions();
	return true;
}

bool MOT::CTilt::GotoTUV(float tdist, float udist, float vdist, DWORD tout, bool bCheck) {
	std::thread A(&MOT::CMotor::MoveA, this, MAXIS::T, tdist, 0, bCheck);
	std::thread B(&MOT::CMotor::MoveA, this, MAXIS::U, udist, 0, bCheck);
	std::thread C(&MOT::CMotor::MoveA, this, MAXIS::V, vdist, 0, bCheck);
	A.join(); B.join(); C.join();
	WaitStopA(MAXIS::T, tdist, tout);
	WaitStopA(MAXIS::U, udist, tout);
	WaitStopA(MAXIS::V, vdist, tout);
	return true;
}

MOT::CTilt::CTilt() {}

MOT::CTilt::~CTilt() {}

void MOT::CTilt::SetN(MTH::SPoint3& N) {
	Geo.N = N;
}

bool MOT::CTilt::DoTilts(MTH::SPoint3& p, MTH::SPoint3& N, DWORD tout, bool bCheck) {
	MTH::SAbcD A; A.PlN(p, N);
	Geo.SetVz(Geo.T, A); Geo.SetVz(Geo.U, A); Geo.SetVz(Geo.V, A);
	return GotoTUV(Geo.T.z, Geo.U.z, Geo.V.z, tout, bCheck);
}

int MOT::CTilt::Reset() {
	std::thread T(&MOT::CMotor::Reset, this, MAXIS::T, MOT::CMotor::Mpar[int(MAXIS::T)].resetPos, false);
	std::thread U(&MOT::CMotor::Reset, this, MAXIS::U, MOT::CMotor::Mpar[int(MAXIS::U)].resetPos, false);
	std::thread V(&MOT::CMotor::Reset, this, MAXIS::V, MOT::CMotor::Mpar[int(MAXIS::V)].resetPos, false);
	T.join(); U.join(); V.join();
	UpdatePositions();
	return 0;
}

int MOT::CTilt::ResetXY0() {
	int err = 0;
	std::thread A(&MOT::CMotor::Reset, this, MAXIS::T, MOT::SMotPar::ZEROPOS, false);
	std::thread B(&MOT::CMotor::Reset, this, MAXIS::U, MOT::SMotPar::ZEROPOS, false);
	std::thread C(&MOT::CMotor::Reset, this, MAXIS::V, MOT::SMotPar::ZEROPOS, false);
	A.join(); B.join(); C.join();
	UpdatePositions();
	return err;
}

bool MOT::CTilt::SetZeroPos() {
	bool bOK = true;
	float pos;
	if (CMotor::ReadNow(MAXIS::T, pos)) {
		CMotor::Mpar[int(MAXIS::T)].rstPos = -pos;
		CMotor::Mpar[int(MAXIS::T)].resetPos = SMotPar::RESETPOS;
		devSetAbsPos(MAXIS::T, Real2Dev(MAXIS::T, 0));
		SaveParam(MAXIS::T, section[int(MAXIS::T)].c_str());
		CMotor::ReadNow(MAXIS::T, pos);
	}
	else bOK = false;
	if (CMotor::ReadNow(MAXIS::U, pos)) {
		CMotor::Mpar[int(MAXIS::U)].rstPos = -pos;
		CMotor::Mpar[int(MAXIS::U)].resetPos = SMotPar::RESETPOS;
		devSetAbsPos(MAXIS::U, Real2Dev(MAXIS::U, 0));
		SaveParam(MAXIS::U, section[int(MAXIS::U)].c_str());
		CMotor::ReadNow(MAXIS::U, pos);
	}
	else bOK = false;
	if (CMotor::ReadNow(MAXIS::V, pos)) {
		CMotor::Mpar[int(MAXIS::V)].rstPos = -pos;
		CMotor::Mpar[int(MAXIS::V)].resetPos = SMotPar::RESETPOS;
		devSetAbsPos(MAXIS::V, Real2Dev(MAXIS::V, 0));
		SaveParam(MAXIS::V, section[int(MAXIS::V)].c_str());
		CMotor::ReadNow(MAXIS::V, pos);
	}
	else bOK = false;
	return bOK;
}