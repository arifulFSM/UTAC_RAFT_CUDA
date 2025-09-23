#pragma once

#include <string>
#include <vector>

#include "MTH/define.h"

#define THRES  127

namespace WLI {
	// everything related to one interferogram
	enum FRP {
		// definition of fringe point
		REDA,  // Amplitude 1
		GRNA,  // Amplitude 2
		BLUA,  // Amplitude 3
		WHTA,  // Amplitude 4
		PHSE,  // E phase
		UPHE,
		PHS1,  // Phase 1
		UPH1,
		PHS2,  // Phase 2
		UPH2,
		ORDR,  // fringe order
		VIS1,  // visibility 1
		VIS2,  // visibility 2
		RSLT,  // result
		TMP1,  // temporary, intermediate
		TMP2,  // temporary, intermediate
		TMP3,  // temporary, intermediate
		ZAXS,  // z value
		LASTF
	};
	struct SIn {
		short i = 0;
		float x = 0, y = 0;
	};
	struct SStat {
		float fave = BADDATA, fmax = BADDATA, fmin = BADDATA;
		int imx = 0, imn = 0, inc = 1;
	};

	struct SZp {
		// Z point data [3/20/2021 FSM]
		static std::string FrpHdr[int(FRP::LASTF)];
		SStat St[FRP::LASTF];
		unsigned short i = 0;

		std::vector<float> zaxs, reda, grna, blua, whta, phs1, phs2, phse,
			uph1, uph2, uphe, ordr, vis1, vis2, rslt, tmp1, tmp2, tmp3;

		int Sz[WLI::LASTF] = { 0 };
		std::vector<float>& Get(WLI::FRP ch);
		float* Get(WLI::FRP Ch, int ist);
		float* Get(WLI::FRP Ch, int ist, int sz);
		int size();
		int size(WLI::FRP Ch);
		void clear();
		void clear(WLI::FRP Ch);
		void resize(WLI::FRP Ch, int sz);

		bool IsAllc(WLI::FRP Ch);
	};
}