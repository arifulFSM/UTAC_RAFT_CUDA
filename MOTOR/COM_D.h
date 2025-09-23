#pragma once

#include "COM.h"

namespace MOT {
	class CCOM_D : public CCOM {
		HANDLE hCom = INVALID_HANDLE_VALUE;
		//OVERLAPPED ORead, OWrite;

	public:
		CCOM_D();
		virtual ~CCOM_D();

	public:
		int IsData();
		bool Close();
		bool Open(short nPort, int baud = 9600);
		int Write(const char* buffer, int bufsz);
		int Read1(char* buffer, int bufsz, char term, DWORD tout);
	};
}