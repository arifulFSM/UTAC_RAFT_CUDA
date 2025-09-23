#pragma once

#include <string>
#include <vector>
#include "SRC/DOSUtil.h"

#ifndef LOGITEMLEN
#define LOGITEMLEN 2048
#define MSGITEMLEN 128
#endif

namespace LOG {
	enum ELOGLEVEL {
		CERROR, WARNING, NOTICE, NORMAL
	};

	struct SLogItem {
		TCHAR msg[MSGITEMLEN];
		ELOGLEVEL lvl = NORMAL;
		const SLogItem& operator=(const SLogItem& co) {
			lvl = co.lvl;
			memcpy(msg, co.msg, MSGITEMLEN * sizeof(TCHAR));
		}
	};
	class CLogSys {
		CCriticalSection CS;
		int head = 0, tail = 0; // circular buffer with NO buffer overrun protection
		SLogItem item[LOGITEMLEN];

	public:
		bool bRun = false, bStop = false;
		void MonitorThread();

	private:
		ELOGLEVEL lvl = NORMAL;
		FILE* fLog1 = nullptr, * fLog2 = nullptr;
		short itoday = -1;
		std::wstring path, logfile1, logfile2;

	public:
		CLogSys();
		~CLogSys();

	public:
		void Initialize(std::tm* tm);
		void Deinitialize();
		void Write();
		void Log(const TCHAR* msg, ELOGLEVEL lvl = ELOGLEVEL::NORMAL);
	};
}

extern LOG::CLogSys LgS;
