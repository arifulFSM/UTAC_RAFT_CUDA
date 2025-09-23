#include "pch.h"
#include "COM.h"

using namespace MOT;

void MOT::CCOM::Yld(DWORD dur) {
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

CCOM::CCOM() {}

CCOM::~CCOM() {}

bool CCOM::Open(short nPort, int baud) {
	bOpened = true;
	return bOpened;
}

int MOT::CCOM::IsData() {
	return 0;
}

bool CCOM::Close(void) {
	bOpened = false;
	return true;
}

int CCOM::Read1(char* buffer, int bufsz, char term, DWORD tout) {
	return true;
}

int CCOM::Write(const char* buffer, int bufsz) {
	return true;
}

bool MOT::CCOM::IsOpened() {
	return bOpened;
}