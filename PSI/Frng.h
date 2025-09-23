#pragma once

#include <vector>

#include "Roi.h"
#include "sdef.h"
#include "PSpar.h"
#include "IM/IM.h"
#include "MTH/Stat.h"
#include "MTH/Define.h"

#ifndef PIE
#define PIE     3.1415926f
#define PIE2    6.283185f
#endif

namespace WLI {
	struct SInc {
		int inc = 0;
		int inc2 = 0;
		float sdStep = 0;
	};

	struct SFrng {
	private:
		bool bZeroed = false;
		float FakeIntensity(float z, float z0, float dz, float lc, float lm);
		inline float Interpolate(float x0, float x, float x1, float y0, float y1);

	private:
		//bool _PhaseRG5(int ix, WLI::FRP Ch1, WLI::FRP Ch2, SInc& InC1, SInc& InC2);
		bool _Visi5(int ix, SInc& InC, WLI::FRP Ch);
		float _Phase5(int ix, SInc& InC, WLI::FRP Ch1, WLI::FRP Ch2);
		//float Vis2_5(int ix, int inc, float sin_dStep, WLI::FRP Ch);

	public:
		float TW1Z(float phEU, float phG, WLI::SPSpar& PsP);
		float TW1Zex(int sz, float phEU, float phG, float& Ord, WLI::SPSpar& PsP);
		//void Phase5(WLI::FRP Ch, std::vector<SSStat>& SSt, SROI& R);
		//void PhaseRGE5U(WLI::FRP fCh1, WLI::FRP fCh2, SInc& InC1, SInc& InC2, SROI& R);
		void Visi5(WLI::FRP Ch, SInc& InC, const SROI& R);

		float TWM5(std::vector<SInc>& InC, float lmG1, float lmG2, float lmG, float lmE, const SROI& R);
		//float VIS5(float sin_dStep, int inc, SROI& R, WLI::FRP Ch);

	public:
		//bool PhaseU5(WLI::SPSpar& PsP, SROI& R);
		bool PhasePV5(WLI::FRP Ch, WLI::FRP Ph, WLI::SPSpar& PsP, const SROI& Ra);
		//bool PhasePVE5ex(WLI::SPSpar& PsP, SROI& R);
		bool VisiV5(WLI::FRP Ch, WLI::SPSpar& PsP, const SROI& R);
		bool PeakTW1ex(int idx, WLI::SPSpar& PsP, const SROI& R, int sz);
		float PeakGrad(WLI::FRP Ch, int st, int ed, int sz);
		float PeakPhas(WLI::FRP Ch, int st, int ed, bool bPChg, int sz);
		float PeakPSI5(WLI::FRP Ch, int idx, int inc, float PSsin, int sz);
		float PhaseI5(WLI::SPSpar& PsP, int idx, float& PhG);

	public:
		//////////////////////////////////////////////////////////////////////////
		// Attributes
		SZp Z;
		//////////////////////////////////////////////////////////////////////////

	protected:
		void resize(WLI::FRP Ch, int n);

	public: // diagnostics
		void dump(FILE* fp);
		void dump(FILE* fp, WLI::FRP idx, const SROI& R);
		void dump(const char* fname);
		void dump(const char* fname, WLI::FRP idx, const SROI& R);

	public:
		//bool ZeroMean(IMGL::eCH nCh, SROI& R);
		float DStep();
		float EqvWL(float l1, float l2);
		//float TWLI(int idx) { return 0; }
		float Wavelength(WLI::FRP Ch, short nQ);
		float XatY0(WLI::FRP Ch, int imx, int inc);
		int BestMax(IMGL::eCH nCh);
		//int FindNearestIdx(float fix, SROI& R);
		bool Smooth(WLI::FRP Ch, int n, int wdw, const SROI& R);
		void Smooth5(WLI::FRP Ch, int n, const SROI& R);
		//int Smooth9(WLI::FRP Ch, int n, SROI& R);
		//void AddANoise(WLI::FRP Ch, float pct);
		void MakeWhite(const SROI& R, bool bBg, float ave);
		void FakData(float len, int div, float z0, float dz, int gain, int offset);
		//void LSQ(WLI::FRP Ch, SROI& R);
		//void Max(float& fmax, int& imax, WLI::FRP Ch, SROI& R);
		//void MaxMin(WLI::FRP Ch, SROI& R);
		//void Scale(WLI::FRP Ch, float sf, SROI& R);
		//void Stati(SSStat& SSt, WLI::FRP Ch1);
		void Stats(SSStat& SSt, float DStep, float wavelen);
		//void Unwrap(WLI::FRP Ch, int idx, SROI& R);
		//void Unwrap(WLI::FRP Ch, SROI& R);
		//void UnwrapEx(WLI::FRP Ch, SROI& R);
		void MaxMin(WLI::FRP Ch, const SROI& R, int sz, bool bAve);

	public:
		SFrng();
		SFrng(int sz);
		SFrng(std::vector<WLI::FRP> args, int sz);
		~SFrng() {}
		void resize(int sz);
		void resize(std::vector<WLI::FRP> args, int sz);

	public: // maintenance
		void Reset();
		//void Clear();
		//void Clear(WLI::FRP Ch);
	};
}