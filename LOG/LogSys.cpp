#include "pch.h"
#include <thread>
#include "atltime.h"
#include "LogSys.h"

LOG::CLogSys LgS;

void LOG::CLogSys::Log(const TCHAR* msg, ELOGLEVEL lvl /*= ELOGLEVEL::NORMAL*/) {
	CSingleLock Lock(&CS); Lock.Lock();
	int len = int(wcslen(msg)); if (len >= 128 - 1) len = 128 - 2;
	SLogItem& itm = item[tail];
	wcsncpy(itm.msg, msg, len);
	itm.msg[len + 1] = 0; tail++;
	if (tail >= LOGITEMLEN) tail = 0;
}

void LOG::CLogSys::Write() {
	CSingleLock Lock(&CS); Lock.Lock();
	SLogItem itm = item[head]; head++;
	if (head >= LOGITEMLEN) head = 0;
	Lock.Unlock();

	std::time_t t = std::time(nullptr);
	std::tm* tm = std::localtime(&t);

	if (itoday != tm->tm_mday) {
		itoday = tm->tm_mday;
		Initialize(tm); // create directory only
		logfile1 = path + std::to_wstring(itoday + 1) + _T(".LOG");
		switch (itm.lvl) {
		case ELOGLEVEL::CERROR:
			logfile2 = path + std::to_wstring(itoday + 1) + _T(".ERR");
			break;
		case ELOGLEVEL::WARNING:
			logfile2 = path + std::to_wstring(itoday + 1) + _T(".WRN");
			break;
		case ELOGLEVEL::NOTICE:
			logfile2 = path + std::to_wstring(itoday + 1) + _T(".NOT");
			break;
		default: logfile2 = _T("");  break;
		}
	}
	FILE* fp1 = _wfopen(logfile1.c_str(), _T("ab"));
	if (fp1) {
		fwprintf(fp1, L"%02d/%02d/%02d, %02d:%02d:%02d, %s\n",
			tm->tm_year - 100, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec,
			itm.msg);
		fclose(fp1);
	}
	if (logfile2.length()) {
		FILE* fp2 = _wfopen(logfile2.c_str(), _T("ab"));
		if (fp2) {
			fwprintf(fp2, L"%02d/%02d/%02d, %02d:%02d:%02d, %s\n",
				tm->tm_year - 100, tm->tm_mon + 1, tm->tm_mday,
				tm->tm_hour, tm->tm_min, tm->tm_sec,
				itm.msg);
			fclose(fp2);
		}
	}
}

void LOG::CLogSys::MonitorThread() {
	bRun = true; bStop = false;
	while (bRun) {
		if (head == tail) {
			Sleep(10); continue;
		}
		Write();
	}
	bStop = true;
}

LOG::CLogSys::CLogSys() {
	std::thread thread(&CLogSys::MonitorThread, this);
	thread.detach();
}

LOG::CLogSys::~CLogSys() {
	Deinitialize();
}

void LOG::CLogSys::Initialize(std::tm* tm) {
	wchar_t tmp[32];
	CDosUtil DosUtil;
	path = DosUtil.GetProgramDir();
	path = path + _T("LOG");
	if (_wmkdir(path.c_str())) {}
	wsprintf(tmp, _T("\\%02d%02d\\"), tm->tm_year - 100, tm->tm_mon);
	path += tmp;
	if (_wmkdir(path.c_str())) {}
}

void LOG::CLogSys::Deinitialize() {
	if (bRun) {
		bRun = false; while (!bStop) Sleep(10);
	}
}