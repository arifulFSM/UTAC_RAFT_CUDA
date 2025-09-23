#include "pch.h"
#include "LOG/LogSys.h"
#include "COM_D.h"

using namespace MOT;

CCOM_D::CCOM_D() {
	//bSimu = false;
	hCom = INVALID_HANDLE_VALUE;
	bOpened = false;
}

CCOM_D::~CCOM_D() {
	Close();
}

int MOT::CCOM_D::IsData() {
	COMSTAT ComStat;
	DWORD dwErrorFlags;
	ClearCommError(hCom, &dwErrorFlags, &ComStat);
	return ComStat.cbInQue;
}

bool MOT::CCOM_D::Close() {
	if (hCom == INVALID_HANDLE_VALUE) {
		bOpened = false; return true;
	}
	CloseHandle(hCom); hCom = INVALID_HANDLE_VALUE;
	bOpened = false;
	return true;
}

bool MOT::CCOM_D::Open(short nPort, int baud) {
	if (hCom != INVALID_HANDLE_VALUE) {
		bOpened = true; return true;
	}
	DCB dcb;
	CString szPort;
	szPort.Format(_T("\\\\.\\COM%d"), nPort);
	hCom = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL /* | FILE_FLAG_OVERLAPPED*/, nullptr);
	if (hCom == INVALID_HANDLE_VALUE) {
		bOpened = false; return false;
	}
	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 0xFFFFFFFF;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts(hCom, &CommTimeOuts);

	dcb.DCBlength = sizeof(DCB);
	GetCommState(hCom, &dcb);
	dcb.BaudRate = baud;
	dcb.ByteSize = nBit;
	//0-4=no,odd,even,mark,space
	switch (tolower(nPa)) {
	case 'n': dcb.Parity = 0; break;
	case 'o': dcb.Parity = 1; break;
	case 'e': dcb.Parity = 2; break;
	case 'm': dcb.Parity = 3; break;
	case 's': dcb.Parity = 4; break;
	}
	if (!SetCommState(hCom, &dcb) || !SetupComm(hCom, 1024, 1024)) {
		CloseHandle(hCom); hCom = INVALID_HANDLE_VALUE;
		LgS.Log(L"Fail to open comm port", LOG::CERROR);
		return false;
	}
	std::wstring str = L"Comm port " + std::to_wstring(nPort) + L" oppened";
	LgS.Log(str.c_str());
	return bOpened = true;
}

int MOT::CCOM_D::Read1(char* buffer, int bufsz, char term, DWORD tout) {
	if (hCom == INVALID_HANDLE_VALUE) return -2;
	char ch;
	int dwTotalRead = 0;
	BOOL bReadStatus;
	DWORD dwBytesRead;
	DWORD tick = GetTickCount();
	while (--bufsz) {
		while (!IsData()) { if ((GetTickCount() - tick) > tout) return -1; Yld(5); }
		bReadStatus = ReadFile(hCom, &ch, 1, &dwBytesRead, nullptr);
		if (!bReadStatus) {
			return -dwTotalRead;
			//if (GetLastError() == ERROR_IO_PENDING) {
			//	WaitForSingleObject(ORead.hEvent, 2000);
			//}
			//else return -dwTotalRead;
		}
		if (ch == term) { buffer[dwTotalRead] = 0; return dwTotalRead; }
		buffer[dwTotalRead++] = ch;
	}
	buffer[dwTotalRead] = 0;
	return -dwTotalRead;
}

int MOT::CCOM_D::Write(const char* buffer, int bufsz) {
	if (hCom == INVALID_HANDLE_VALUE) return 0;
	BOOL bWriteStatus;
	DWORD dwBytesWritten = 0;
	int len = int(strlen(buffer));
	bWriteStatus = WriteFile(hCom, buffer, int(strlen(buffer)), &dwBytesWritten, nullptr);
	if (!bWriteStatus) {
		//if (GetLastError() == ERROR_IO_PENDING) {
		//	WaitForSingleObject(ORead.hEvent, 5000);
		//	return((int)dwBytesWritten);
		//}
		return(dwBytesWritten);
	}
	return dwBytesWritten;
}