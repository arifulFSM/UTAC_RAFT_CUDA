#pragma once

#include "MTH/Ang.h"
#include "sdef.h"

namespace WLI {
	struct SPSpar {
		float UStep_um = 0;
		float WL1_um = 0, WL2_um = 0, WL3_um = 0, WLE_um = 0;
		float PS[FRP::WHTA + 1] = { 0 };
		float PSrd[FRP::WHTA + 1] = { 0 };
		float PSsin[FRP::WHTA + 1] = { 0 };

		short Inc[FRP::WHTA + 1]={0};
		float sf1 = 0, sf2 = 0, wnum = 0, dwnum = 0;

		void SetConst(float wlR, float wlG, float wlW, float UStep);
		void Set(WLI::FRP Ch, float wl, float us);
		void dump(FILE* fp);
		float Get(WLI::SPSpar& PsP, WLI::FRP Ch);
		CString Report();

	private:
		void Set(float wlr, float wlg, float wlw, float us);
	};
}
