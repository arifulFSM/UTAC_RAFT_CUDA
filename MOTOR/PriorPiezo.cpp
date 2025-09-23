#include "pch.h"
#include "PriorPiezo.h"

bool MOT::CPriorPiezo::Initialize() {
	status = FindDll(dll);
	if (!status) {
		AfxMessageBox(L"ERROR: Could not find controller interface DLL");
		return 0;
	}

	isOk = dllAdapter.Init(dll);
	if (isOk != DLL_ADAPTER_STATUS_SUCCESS) {
		AfxMessageBox(L"ERROR: Controller interface DLL is not valid");
		return 0;
	}

	const char* portNum = "\\\\.\\COM17";

	bool openedOk = serialLink.Open(portNum);

	if (!openedOk) {
		AfxMessageBox(L"Could not open COM Port.");
		return 0;
	}

	isOk = dllAdapter.OpenSessionDatalink(DatalinkSerial::Read, DatalinkSerial::Write, (void*)&serialLink, 0);

	if (isOk != DLL_ADAPTER_STATUS_SUCCESS) {
		AfxMessageBox(L"Failed to open datalink");
	}

	//measureCurPos();
	//command = "stage.mode.analogue-command.get 1";
	//isOk = dllAdapter.DoCommand(command, resultNames, resultValues);
	//AfxMessageBox(L"Controller connected");
	isConnected = 1;
	return 1;
}

bool MOT::CPriorPiezo::MoveR(float val) {
	val *= 1e6; // Convert To um
	command = "stage.position.stepped-command.increment 1 " + std::to_string(val); // move-cmd, channel num, pos
	isOk = dllAdapter.DoCommand(command, resultNames, resultValues);
	//Yld(5);
	if (isOk == DLL_ADAPTER_STATUS_SUCCESS) {
		return true;
	}
	else {
		return false;
	}
}

bool MOT::CPriorPiezo::MeasureCurPos(float& val) {
	command = "stage.position.measured.get 1";
	isOk = dllAdapter.DoCommand(command, resultNames, resultValues);
	if (isOk == DLL_ADAPTER_STATUS_SUCCESS) {
		val = atof(resultValues.begin()->c_str()) * 1.0e-6; // convert pm to um
		return true;
	}
	else {
		return false;
	}
}

void MOT::CPriorPiezo::CloseSession() {
	dllAdapter.CloseSession();
	serialLink.Close();
	isConnected = 0;
}

void MOT::CPriorPiezo::Yld(DWORD tout) {
	DWORD tick = GetTickCount();
	while ((GetTickCount() - tick) < tout) {
		MSG msg;
		// User message will be removed too
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) return;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}