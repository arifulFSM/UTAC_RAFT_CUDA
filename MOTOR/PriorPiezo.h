#pragma once
#include "MOTOR/PriorConfig/dll_adapter.hpp"
#include <string>
#include <list>
#include <iostream>
#include <cstdlib>

#include "MOTOR/PriorConfig/datalink_serial.hpp"
#include "MOTOR/PriorConfig/find_dll.hpp"

namespace MOT {
	class CPriorPiezo {
	public:
		bool isConnected = 0;
		bool Initialize();
		bool MoveR(float val);
		bool MeasureCurPos(float& val);
		void CloseSession();
		DllAdapterStatus isOk;
		std::list<std::string>::iterator strListIter, strListIter2;
		std::string dll;
		DllAdapter dllAdapter;
		DatalinkSerial serialLink;
		bool status;
		void Yld(DWORD tout);
		// docommands
		std::string command;
		std::list<std::string> resultNames;
		std::list<std::string> resultValues;
	};
}