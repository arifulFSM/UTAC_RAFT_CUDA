#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#include "eo-drive.h"

#pragma comment(lib, "EO-Drive.lib")

namespace MOT {
	class CEOPiezo {
	public:
		int handle;
		bool isConnected = false;
		bool Initialize();
		bool MoveA(float val);
		bool MeasureCurPos(float& val);
		void CloseSession();
		void Yld(DWORD tout);
	};
}