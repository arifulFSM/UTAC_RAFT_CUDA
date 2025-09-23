#pragma once

#include "IM/IM.h"
#include "MTH/Point3.h"
#include "PSI/sdef.h"
#include "WLI/Spot.h"

namespace RCP {
	enum MTHD { VIS, PS0, PSI, TW1 };

	struct SStage {
		std::vector<MTH::SPoint2> Pts;

		void New() { Pts.clear(); }

		SStage() {}
		~SStage() {}
		void Serialize(CArchive& ar) {}
	};

	struct SRecipe {
		//////////////////////////////////////////////////////////////////////////
		// Measurement
		bool bPChg = false;
		bool bSmo = false;
		bool bSmoHvy = false;
		bool bUnwrap = false;
		bool bFindPChg = false;
		IMGL::eDIR UnwrapDir = IMGL::LR;
		MTHD Mthd = RCP::PS0;
		//////////////////////////////////////////////////////////////////////////
		// stage
		SStage Stage;
		//////////////////////////////////////////////////////////////////////////
		// algorithm
		short nSmo = 1;
		//////////////////////////////////////////////////////////////////////////
		// sample
		short nLay = 0;
		SSpotCtx SCtx[MAXSPOTCTX]; // 0 = blob, 1 = stripe
		//////////////////////////////////////////////////////////////////////////
		// Operation
		void New() {
			bPChg = bSmo = bSmoHvy = false;
			Mthd = RCP::PS0;
			nSmo = nLay = 1;
			Stage.New();
		}

		SRecipe();
		virtual ~SRecipe();
		void Serialize(CArchive& ar) {}
	};
}

extern RCP::SRecipe Rcp;
