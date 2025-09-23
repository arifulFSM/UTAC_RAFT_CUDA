#pragma once

#include <vector>

#include "IM/IM.h"
#include "IM/Ims.h"
#include "SRC/hightime.h"
#include "Roi.h"
#include "Frng.h"
#include "Recipe.h"

#include <opencv2/opencv.hpp>

namespace WLI {
	struct SAmp {
		int p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;
	};

	enum EWHTMTHD {
		ANALY, PCPTN, R121, R131
	};

	class CStrip {
		std::vector< CHighTime> htime;

	public:
		//////////////////////////////////////////////////////////////////////////
		IMGL::CIM ImBG; // background image at the start of capture
		IMGL::CIM16 Im16um;
		//////////////////////////////////////////////////////////////////////////
		// everything related to entire image group
		std::vector<SIms*> Imgs;  // image database
		std::vector<std::pair<cv::Mat, float>>CVImgs;	// image database
		float xPos = 0, yPos = 0, zPos;
		int wd = 0, ht = 0, bpp = 0;
		int nSteps = 0; // total number of steps moved
		short NSlice = 1;	// number of slices per z step moved
		float wlen_um[5] = { 0 }; //  non persist RGBWE
		float UStep_um = 0, zrange_um = 0;
		//////////////////////////////////////////////////////////////////////////
		//! Color conversion
		EWHTMTHD WhtCalc = ANALY;
		//float sfR = 0.2126f, sfG = 0.7152f, sfB = 0.0722f;
		float sfR = 0.2989f, sfG = 0.5870f, sfB = 0.1440f; // NTSC perception
		//float sfR = 0.3333f, sfG = 0.3334f, sfB = 0.3333f; // Analytic
		//////////////////////////////////////////////////////////////////////////
		bool CollectZCH(SFrng& F, int x, int y, SROI& R, WLI::FRP Ch);
		bool CollectZRG(SFrng& F, int x, int y, SROI& R);
		bool CollectRGBW(SFrng& F, int x, int y, SROI& R);
		bool CollectRGBWCV(SFrng& F, int x, int y, SROI& R);//20250916
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		bool MakeZW(SFrng& F, const SROI& R);
		bool HProfile(SFrng& F, WLI::FRP Ch, int y, SROI& R);
		bool VProfile(SFrng& F, WLI::FRP Ch, int x, SROI& R);

	public:
		bool GenHMapV5(RCP::SRecipe& Rcp);
		bool GenHMapFom(int Idx, std::wstring& log);
		bool GenHMapEnv(std::wstring& log);

	public:
		bool InitCalc();
		bool CVInitCalc();
		void DeallocAll();
		CString GetReport();
		bool GetDim(int& x, int& y, int& b);
		bool GetDimCV(int& x, int& y, int& b);//20250916

	public:
		void ResetTime();
		void AddTime(CHighTime tm);
		CString TimeSpanStr(int ed, int st);
		WLI::SIms* NewImgs(float xpos_um);

	public:
		IMGL::CIM* GetImg(int idx);
		WLI::SIms* GetIms(int idx);
		int size();
		int GetMaxPeakIdx(int x, int y, WLI::FRP nChan, bool bPChg = false);
		float GetZPos(int idx);

	public:
		bool Save16(std::wstring& outfile);
		bool Load(const std::wstring& StripName);
		bool Save(const std::wstring& StripName);
		bool ExportBMP(const TCHAR* fname);
		bool ExpAnalysis(const TCHAR* fname, int x, int y);
		bool Dump(const TCHAR* fname);
		bool Dump(const TCHAR* fname, SFrng& F);
		bool Dump(const TCHAR* fname, SFrng& F, WLI::SPSpar& PsP);
		bool Dump(const TCHAR* fname, SFrng& F, std::vector<SSStat>& SSt);

	public:
		CStrip();
		virtual ~CStrip();
		void Clear();
		void Serialize(CArchive& ar);
	};
}

extern WLI::CStrip Strip;
