#include "pch.h"

#include "Piezo.h"

MOT::CPiezoB::CPiezoB() {}

MOT::CPiezoB::~CPiezoB() {}

int MOT::CPiezoB::GetError(int ID) { return iError; }

float MOT::CPiezoB::GetPos_um() {
	return NowPos_um;
}

bool MOT::CPiezoB::Step(float dist_um) {
	NowPos_um += dist_um;
	return true;
}

bool MOT::CPiezoB::Wait(DWORD tout) { return true; }

void MOT::CPiezoB::Yld() {
	MSG msg;
	//? User message will be removed too
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) return;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool MOT::CPiezoB::Goto(float pos_um, bool bWait) {
	NowPos_um = pos_um;
	return true;
}

bool MOT::CPiezoB::IsMoving(DWORD tout) {
	return false;
}

bool MOT::CPiezoB::Reset() {
	return true;
}

bool MOT::CPiezoB::AutoZero() {
	NowPos_um = PZHOMEPOS;
	return true;
}

bool MOT::CPiezoB::Initialize() {
	return true;
}

bool MOT::CPiezoB::Deinitialize() {
	return true;
}

#ifndef _SIMUPIEZO
#include "Pi_gcs2_dll.h"
#pragma comment(lib,"PI_GCS2_DLL_x64.lib")

#define		DELIMITOR			('\n')

MOT::CPiezoPI::CPiezoPI() {}

MOT::CPiezoPI::~CPiezoPI() {}

// BOOL MOT::CPiezoPI::command(char* cmd, USHORT len) {
// 	pSCom->ClearRx(); // clear receive buffer [10/17/2020 FSM]
// 	if (execute(cmd)) {
// 		if (pSCom->ReadLine(cmd, len) > 0) return TRUE;
// 	}
// 	*cmd = 0;
// 	return FALSE;
// }
//
// BOOL MOT::CPiezoPI::execute(char* cmd) {
// 	if (pSCom->SendData(cmd, int(strlen(cmd))) > 0) return TRUE;
// 	return FALSE;
// }
//
// MOT::CPiezoPI::CPiezoPI() {}
//
// MOT::CPiezoPI::~CPiezoPI() {}

float MOT::CPiezoPI::GetPos_um() {
	// return unit is um
	if (!PI_qPOS(iID, szAxes, &dPos_um[0])) {
		GetError(iID); return -999.0f;
	}
	return NowPos_um = float(dPos_um[0]);
}

bool MOT::CPiezoPI::Step(float dist_um) {
	// dist unit is um
	float pos = NowPos_um + dist_um;
	Goto(pos, true);
	return false;
}

bool MOT::CPiezoPI::Goto(float pos_um, bool bWait) {
	// pos: unit is um
	dPos_um[0] = pos_um;
	if (!PI_MOV(iID, szAxes, &dPos_um[0])) {
		GetError(iID); return false;
	}
	if (bWait) {
		Wait(2000); NowPos_um = float(dPos_um[0]);
	}
	return true;
}

bool MOT::CPiezoPI::IsMoving(DWORD tout) {
	////////////////////////////////////////
	// Read the moving state of the axes. //
	////////////////////////////////////////

	// if 'axes' = NULL or 'axis' is empty a general moving state of all axes is returned in 'bIsMoving[0]'
	// if 'bIsMoving[0]' = TRUE at least one axis of the controller is still moving.
	// if 'bIsMoving[0]' = FALSE no axis of the controller is moving.

	// if 'axes != NULL and 'axis' is not empty the moving state of every axis in 'axes' is returned in
	// the array bIsMoving.
	if (!PI_IsMoving(int(ID), NULL, bIsMoving)) {
		PI_GetError(int(ID)); return FALSE;
	}
	return bIsMoving[int(ID)];
}

bool MOT::CPiezoPI::Wait(DWORD tout) {
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < tout) {
		if (IsMoving(0)) { Yld(); continue; }
		return true;
	}
	return false;
}

bool MOT::CPiezoPI::Reset() {
	return true;
}

bool MOT::CPiezoPI::AutoZero() {
	double Array[8] = { 0 };
	BOOL bUseDefault = TRUE;
	if (PI_ATZ(int(ID), szAxes, Array, &bUseDefault)) { return true; }
	return false;
}

bool MOT::CPiezoPI::Initialize() {
	char szBuffer[256];
	int iBufferSize = 255;
	int Id = PI_EnumerateUSB(szBuffer, iBufferSize, "");
	if (Id < 0) { GetError(Id); return false; }
	Id = PI_ConnectUSB(szBuffer);
	//ID = PI_ConnectRS232(Par.nHub, 57600);
	if (Id < 0) { GetError(Id); iID = -1; return false; }
	if (!PI_qSAI(Id, szAxes, 17)) {
		GetError(Id); PI_CloseConnection(Id); iID = -1; return false;
	}
	bFlags[0] = TRUE; // servo on for the axis in the string 'axes'.

	if (!PI_SVO(Id, szAxes, bFlags)) {
		GetError(Id); PI_CloseConnection(Id); iID = -1; return false;
	}
	iID = Id;
	NowPos_um = PZHOMEPOS;
	return true;
}

bool MOT::CPiezoPI::Deinitialize() {
	return true;
}

int MOT::CPiezoPI::GetError(int ID) {
	char szErrorMesage[1024];
	iError = PI_GetError(int(ID));
	PI_TranslateError(iError, szErrorMesage, 1024);
	printf("ERROR %d: %s\n", iError, szErrorMesage);
	return iError;
}

#endif

bool MOT::CPiezo::Initialize() {
	if (Pz.get() != nullptr) return true;

#ifdef _SIMUPIEZO
	Pz.reset(new CPiezoB);
#else
	Pz.reset(new CPiezoPI);
#endif
	Pz->Initialize();
	return true;
}

bool MOT::CPiezo::Deinitialize() { Pz->Deinitialize(); return true; }

MOT::CPiezoB* MOT::CPiezo::Get() { return Pz.get(); }

float MOT::CPiezo::GetPos_um() { return Pz->GetPos_um(); }

bool MOT::CPiezo::WaitStop(DWORD tout) {
	return Pz->Wait(tout);
}

bool MOT::CPiezo::Goto(float pos_um, bool bWait) { return Pz->Goto(pos_um, bWait); }

void MOT::CPiezo::Yld(DWORD tout) {
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < tout) {
		Pz->Yld();
	}
}

MOT::CPiezo::CPiezo() {}

MOT::CPiezo::~CPiezo() {}