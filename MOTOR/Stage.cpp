#include "pch.h"

#include <thread>
#include "Stage.h"
#include "MTH/Ang.h"
#include "SRC/DOSUtil.h"

using namespace MOT;

// load param/ save param
// id is in ini file

bool MOT::CStage::Initialize(CCOM* pCom, bool bSimu) {
	SetCom(pCom);
	for (int i = 0; i < int(MAXIS::T); i++) {
		CMotor::Initialize(MAXIS(i), bSimu);
	}
	return true;
}

bool MOT::CStage::Deinitialize() {
	for (int i = 0; i < int(MAXIS::T); i++) {
		CMotor::Deinitialize(MAXIS(i));
	}
	return true;
}

MOT::CStage::CStage() {}

MOT::CStage::~CStage() {}

void MOT::CStage::Polar2Cart(float R, float A, float& X, float& Y) {
	A = A * PIE2 / 360.f;
	X = R * cos(A);
	Y = R * sin(A);
	if (fabsf(X) < 1e-4f) X = 0;
	if (fabsf(Y) < 1e-4f) Y = 0;
}

void MOT::CStage::Cart2Polar(float X, float Y, float& R, float& A) {
	R = sqrt(X * X + Y * Y);
	A = float((atan2(Y, X) + PIE) * 360.f / PIE2);
	if (fabsf(A) < 1e-4f) A = 0;
	if (fabsf(R) < 1e-4f) R = 0;
}

bool MOT::CStage::Home(bool bCheck) {
	GotoXY(MOT::CMotor::Mpar[int(MAXIS::X)].home, MOT::CMotor::Mpar[int(MAXIS::Y)].home, 10000, bCheck);
	return true;
}

bool MOT::CStage::HomeZ() {
	MoveA(MAXIS::Z, MOT::CMotor::Mpar[int(MAXIS::Z)].home, 2000, true);
	return true;
}

bool MOT::CStage::GotoXY(float xdist, float ydist, DWORD tout, bool bCheck) {
	std::thread A(&MOT::CMotor::MoveA, this, MAXIS::X, xdist, 0, bCheck);
	std::thread B(&MOT::CMotor::MoveA, this, MAXIS::Y, ydist, 0, bCheck);
	A.join(); B.join();
	WaitStopA(MAXIS::X, xdist, tout);
	WaitStopA(MAXIS::Y, ydist, tout);
	return true;
}

bool MOT::CStage::GotoRA(float radius, float angle, DWORD tout, bool bCheck) {
	angle = angle * PIE2 / 360.f;
	float x = radius * cos(angle);
	float y = radius * sin(angle);
	std::thread A(&MOT::CMotor::MoveA, this, MAXIS::X, x, 0, bCheck);
	std::thread B(&MOT::CMotor::MoveA, this, MAXIS::Y, y, 0, bCheck);
	A.join(); B.join();
	WaitStopA(MAXIS::X, x, tout);
	WaitStopA(MAXIS::Y, y, tout);
	return true;
}

int MOT::CStage::Reset() {
	std::thread A(&MOT::CMotor::Reset, this, MAXIS::X, MOT::CMotor::Mpar[int(MAXIS::X)].resetPos, false);
	std::thread B(&MOT::CMotor::Reset, this, MAXIS::Y, MOT::CMotor::Mpar[int(MAXIS::Y)].resetPos, false);
	std::thread C(&MOT::CMotor::Reset, this, MAXIS::Z, MOT::CMotor::Mpar[int(MAXIS::Z)].resetPos, false);
	A.join(); B.join(); C.join();
	UpdatePositions();
	return 0;
}

int MOT::CStage::ResetXY() {
	std::thread A(&MOT::CMotor::Reset, this, MAXIS::X, MOT::CMotor::Mpar[int(MAXIS::X)].resetPos, false);
	std::thread B(&MOT::CMotor::Reset, this, MAXIS::Y, MOT::CMotor::Mpar[int(MAXIS::Y)].resetPos, false);
	A.join(); B.join();
	UpdatePositions();
	return 0;
}

int MOT::CStage::ResetXY0() {
	std::thread A(&MOT::CMotor::Reset, this, MAXIS::X, MOT::SMotPar::ZEROPOS, false);
	std::thread B(&MOT::CMotor::Reset, this, MAXIS::Y, MOT::SMotPar::ZEROPOS, false);
	A.join(); B.join();
	UpdatePositions();
	return 0;
}

int MOT::CStage::ResetZ() {
	MOT::CMotor::Reset(MAXIS::Z, MOT::CMotor::Mpar[int(MAXIS::Z)].resetPos, false);
	float pos;
	CMotor::ReadNow(MAXIS::Z, pos);
	return 0;
}

bool MOT::CStage::SetZeroPos() {
	bool bOK = true;
	float pos;
	if (CMotor::ReadNow(MAXIS::X, pos)) {
		CMotor::Mpar[int(MAXIS::X)].rstPos = -pos;
		CMotor::Mpar[int(MAXIS::X)].resetPos = SMotPar::RESETPOS;
		devSetAbsPos(MAXIS::X, Real2Dev(MAXIS::X, 0));
		SaveParam(MAXIS::X, _T("XMOTOR"));
		CMotor::ReadNow(MAXIS::X, pos);
	}
	else bOK = false;
	if (CMotor::ReadNow(MAXIS::Y, pos)) {
		CMotor::Mpar[int(MAXIS::Y)].rstPos = -pos;
		CMotor::Mpar[int(MAXIS::Y)].resetPos = SMotPar::RESETPOS;
		devSetAbsPos(MAXIS::Y, Real2Dev(MAXIS::Y, 0));
		SaveParam(MAXIS::Y, _T("YMOTOR"));
		CMotor::ReadNow(MAXIS::Y, pos);
	}
	else bOK = false;
	return bOK;
}

void MOT::CStage::LED(bool bOn) {
	SetOutput(MAXIS::X, 1, !bOn);
}

void MOT::CStage::Laser(bool bOn) {
	SetOutput(MAXIS::X, 2, !bOn);
}

// 12052023 - START

void MOT::CStage::SHUTTER(bool bOn) {
	SetOutput(MAXIS::Y, 1, bOn);
}

// 12052023 - END

void MOT::CStage::UpdatePositions() {
	float pos;
	CMotor::ReadNow(MAXIS::X, pos);
	CMotor::ReadNow(MAXIS::Y, pos);
	CMotor::ReadNow(MAXIS::Z, pos);
}

bool MOT::CStage::SetupZJog(float speed, bool bJogUp) {
	// JE JD
	if (!JogZEnable(true)) return false;
	// JS
	spd = GetSpeed(MAXIS::Z);
	if (!JogZSpeed(speed)) return false;
	// DI: down(1), up(-1)
	if (bJogUp) SetDistance(MAXIS::Z, -1);
	else SetDistance(MAXIS::Z, 1);
	return true;
}

bool MOT::CStage::StartZJog() {
	// CJ
	return JogZCommence();
}

bool MOT::CStage::StopZJog() {
	// ST
	if (!Stop(MAXIS::Z)) return false;
	// JD
	if (!JogZEnable(false)) return false;
	SetSpeed(MAXIS::Z, spd);
	return true;
}