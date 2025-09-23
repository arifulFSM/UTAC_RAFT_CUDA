#pragma once

#include <fstream>

#include "IM.h"
#include "PSI/Frng.h"

namespace WLI {
	struct SIms {
		IMGL::CIM Im;
		//cv::Mat Im;
		float PzPos_um = 0; // unit : um [2/14/2021 FSM]

	public:
		COLORREF GetPixRGB(float x, float y);
		COLORREF GetPixRGB(int x, int y);
		short GetI(int x, int y, IMGL::eCH nChan);
		bool GetDim(int& x, int& y, int& bpp);

	public:
		SIms();
		virtual ~SIms();
		void Serialize(CArchive& ar);
	};
}
