#include "pch.h"
#include "CEOPiezo.h"

bool MOT::CEOPiezo::Initialize() {
	handle = EO_InitHandle();
	if (handle == 0) {
		return 0;
	}
	isConnected = true;
	return 1;
}

bool MOT::CEOPiezo::MoveA(float pos) {
	int moveStatus = EO_Move(handle, pos);
	if (moveStatus == EO_SUCCESS) {
		return true;
	}
	return false;
}

bool MOT::CEOPiezo::MeasureCurPos(float& val) {
	double pos;
	int stat = EO_GetCommandPosition(handle, &pos);
	if (stat == EO_SUCCESS) {
		val = pos;
		return true;
	}
	return false;
}

void MOT::CEOPiezo::CloseSession() {
	isConnected = false;
	EO_ReleaseHandle(handle);
}

void MOT::CEOPiezo::Yld(DWORD tout) {
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < tout) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}