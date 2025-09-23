#pragma once

#include <vector>

// #include "FConst.h"
// #include "Frin4.h"
// #include "Fringe.h"
// #include "FrP4.h"
// #include "Opt.h"
// #include "Fring4Ref.h"
// #include "TimeEvt.h"
// #include "PSI/PsiCalc.h"
#include "IM/IM.h"
//#include "PhMo.h"
// #include "WLI/Pointf.h"

#define HALFPEAK 1

struct SUI;

namespace IMGL {
	struct SImg {
		IMGL::CIM Im;
		float PzPos_um = 0; // unit : um [2/14/2021 FSM]

		inline COLORREF GetPixRGB(float x, float y);
		inline COLORREF GetPixRGB(int x, int y);
		int GetInten(int x, int y, eCH nChan);

		SImg();
		virtual ~SImg();
		void Serialize(CArchive& ar);
	};

	struct SFrP1 {
		short i = 0;	// index [3/20/2021 FSM]
		float x_um = 0;	// x position (PizoPos ...) [3/20/2021 FSM]
		float y = 0;	// y amplitude [3/20/2021 FSM]
	};

	struct SImgStrip {
	public:
		//BOOL CalcStartStops_nm(float stepPz_nm, int idx, int N, std::vector<SFrP1>& vP);

	public:
		short GetHeight();
		short GetWidth();
		BOOL GetDim(int& wd, int& ht);
		//BOOL GenHeightRef();
		//BOOL GenHeightRefH();
		//BOOL GenHeightRefV();

		//////////////////////////////////////////////////////////////////////////
	public:
		CString errStr = L"";
		//CFring4Ref Ref;    // reference [10/14/2021 yuenl]
		//////////////////////////////////////////////////////////////////////////
	public:
		// Height calculations
		//BOOL BGP(IMGL::CIM16& Im16_rad, IMGL::CIM16& Im16_um);
		//BOOL GenHeightPSIMap(int idx, IMGL::CIM16& Im16_rad, IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL CSI(CHNL nChan);
		//BOOL WLPSI(CHNL nChan);
		//BOOL FOM(int idx, IMGL::CIM16& Im16_um, CHNL nChan);
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
	public:
		//int GetBestIndex(CHNL nChan, CAM::SROI& R);
		//int FindMaxPeakIdx(SFrin4& F1, CHNL nChan, int x, int y);
		//int FindMaxVisIdx(CHNL nChan, BOOL bPChg, SFrin4& F1, CAM::SFrD& D, int x, int y);
		//BOOL FindBestROI(CAM::SROI& R, CHNL nChan, float fPad);
		//BOOL CalcWLen(CHNL nChan);
		//BOOL CalcCenterWLenAll(CHNL nChan);
		void Integrate();
		void Integrate(int NFrame);
		//////////////////////////////////////////////////////////////////////////
		//void GetPhaseLineRG(IMGL::CIM16& Im16_rad, IMGL::CIM16& Im16Eq_rad,
		//	CHNL nChan, int y, int wd, float alpha_rad, std::vector<CAM::SFrP1>& Pos);
		//////////////////////////////////////////////////////////////////////////
		//SPhMo GetPhase5NV_rad(int x, int y, float piShift_rad, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase3a(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase3b(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase3c(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase3e(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase3NV_rad(int x, int y, float PShift_rad, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase4a(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase4c(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase4NV(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase5a(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase5b(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase5c(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase6a(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase6b(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase7a(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase7b(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//SPhMo GetPhase8a(int x, int y, eCH nChan, std::vector<SFrP1>& vP);
		//////////////////////////////////////////////////////////////////////////

	public:
		//////////////////////////////////////////////////////////////////////////
		// intermediate [10/25/2020 yuenl]
		BOOL bDump = FALSE;
		//SAlgo algo;
		float wdw = 0.05f;
		float fMax = 0, fMin = 0;
		float cWLen_nm = 0;
		int nIdx = 0;       // current image in strip [11/4/2020 yuenl]
		short PRes = 2;		// 3D Plot resolution
		short NSlice = 1;	// number of slices per z step moved
		short NFrames = 0;
		//////////////////////////////////////////////////////////////////////////

		enum ePKSEL { eINTENSITY, ePOSITION };
		ePKSEL PSel = ePOSITION; // Peak selection preference [11/13/2020 FSM]
		IMGL::CIM16 Im16_rad;    // Top surface, unit is radian [11/7/2020 FSM]
		IMGL::CIM16 Im16Eq_rad;  // Top surface, unit is radian [11/7/2020 FSM]
		IMGL::CIM16 Im16_um;     // Top surface, unit is um [11/7/2020 FSM]
		IMGL::CIM ImBG;
		IMGL::CIM16 Im16A, Im16B, Im16C, Im16D;
		//CAM::SFConst Con;		 // Fringe constants [7/4/2021 FSM]
		SImg* Imgs_dmy = nullptr;
		std::vector<SImg*> Imgs;
		//STimeEvt Capture/*, HightCalculation*/;
		//////////////////////////////////////////////////////////////////////////
		SImgStrip();
		virtual ~SImgStrip();
		void Serialize(CArchive& ar);
		//////////////////////////////////////////////////////////////////////////
		// House keeping
		void ClearImAll();
		void ClearIm16_rad();
		void ClearIm16ABCD();
		//////////////////////////////////////////////////////////////////////////
	private:
		//////////////////////////////////////////////////////////////////////////
		//BOOL GenMap_WL_PSI1(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenMap_WL_PSI2(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenMap_WL_PSI3(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenMap_WL_PSI4(IMGL::CIM16& Im16_um, CHNL nChan, int nPeak);
		//////////////////////////////////////////////////////////////////////////
	public:
		//////////////////////////////////////////////////////////////////////////
		//void LinearizeX();
		//////////////////////////////////////////////////////////////////////////
		//? Major entry point                                                   //
		//BOOL GenHeightMap_CG_CSI_MAXV(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenHeightMap_CG_CSI_MAXP(IMGL::CIM16& Im16_um, CHNL nChan);
		//int Find2PeaksIndex(CAM::SFrin4& F1, CHNL nChan, BOOL bPChg);
		//BOOL GenHeightMap_CSI_CG(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenHeightMap_CSI_PSI_P(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenHeightMap_CSI_PSI_V(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenHeightMap_WL_PSI(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenHeightMap_WL_PSI(IMGL::CIM16& Im16_um, CHNL nChan, int nPeak);
		//BOOL GenHeightMap_TWL1_PSI(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL GenHeightMap_TWL2_PSI(IMGL::CIM16& Im16_um, CHNL nChan);
		//////////////////////////////////////////////////////////////////////////
		//BOOL GenZMap(IMGL::CIM16& Im16_um, CHNL nChan, int pos, BOOL bHorz);
		//////////////////////////////////////////////////////////////////////////
		//BOOL GenPhaseMap(IMGL::CIM16& Im16_rad, CHNL nChan, int idx);
		//BOOL GenPhaseMap(IMGL::CIM16& Im16A, IMGL::CIM16& Im16B, IMGL::CIM16& Im16C, IMGL::CIM16& Im16D);
		//BOOL GenPhaseMapEqv(IMGL::CIM16& Im16_rad, CHNL fr, CHNL to);
		//////////////////////////////////////////////////////////////////////////
		//BOOL AddPhasePointRGB(IMGL::CIM16& Im16, int idx, int x, int y, CHNL nChan, std::vector<SFrP1>& vP);
		//////////////////////////////////////////////////////////////////////////
		//BOOL CollectStripWRGBST(SFrin4& F1, int x, int y, CAM::SROI& R, SIn& In);
		//BOOL CollectStrip(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//BOOL CollectStrip(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL Fr, CHNL To);
		//BOOL CollectStrip(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL Fr, CHNL To, SIn& In);
		//BOOL CollectStripF(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL Fr, CHNL To);
		//BOOL CollectStripZM(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//BOOL CollectStripZMST(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//BOOL CollectStripT3(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL Fr, CHNL To);
		//BOOL CollectStripST(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL Fr, CHNL To);
		//////////////////////////////////////////////////////////////////////////
		//BOOL CollectStrip_WRGB_ST(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripST(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//////////////////////////////////////////////////////////////////////////
		BOOL IdxValid();
		BOOL ValidateIdx();
		//void SetIdxMaxPeak(CHNL nChan, BOOL bPChg);
		//////////////////////////////////////////////////////////////////////////
		float GetStripRange_um();
		//float HeightXY(SPointf* pPt);
		//float* GetBufferX(SPointf* pPt, int& sz);
		//float* GetBufferY(SPointf* pPt, int& sz);
		//float* GetHeightX(SPointf* pPt, float* pBuf, size_t sz);
		//float* GetHeightY(SPointf* pPt, float* pBuf, size_t sz);
		IMGL::CIM* GetIm8();
		IMGL::CIM* GetImg(int idx);
		//int GetNStep();
		//float GetPShift_rad();
		void Add(SImg* pIM);
		void DeallocAll();
		void Dump(char* fname, float* Buf, short sz);
		//void DumpFringeC(char* fname, SFrings& F0, int sz, CHNL nChan);
		//void DumpFringes(char* fname, SFrings& F0, int sz);
		//////////////////////////////////////////////////////////////////////////
		//bool Despike(SPkPt* pBuf, int sz);
		//float* GetFringeData(SFrings& F0, CHNL nChan);
		//float* SplineExpand(float* buf, int sz, int nsz);
		//void CreateStrips_um(float xStart_um, float xStop_um, int nFrames, int wd, int ht);
		//void GaussFilter(SFrings& F0, short wdw, CHNL nChan);
		//void Level(float* pBuf, size_t sz);
		//void LevelAve(float* pBuf, size_t sz);
		//void LevelGauss(float* pBuf, size_t sz, short wdw);
		//void LevelRough(float* pBuf, size_t sz);
		//void LevelStep(float* pBuf, size_t sz);
		//void Reconstruct(IMGL::CIM16& Im16_um);
		//void RemoveZero(float* pBuf, size_t sz);
		//void StatSmooth(float* pBufY, size_t sz);
		//////////////////////////////////////////////////////////////////////////
		//CString CalculateRoughness(SPeakArray* pPkA);
		//CString CalculateStepHeight(SPeakArray* pPkA);
		//////////////////////////////////////////////////////////////////////////
		// PSI section [2/18/2021 FSM]
		//////////////////////////////////////////////////////////////////////////
		//BOOL GetInten3_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan);
		//BOOL GetIntensity_nm(int x, int y, float Step_nm, int idx, float* fI, short N, CHNL nChan);
		//float GetInten3v1_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan);
		//float GetInten4_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan);
		//float GetInten4v1_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan);
		//float GetInten5_nm(int x, int y, float Step_nm, int idx, float* fI, CHNL nChan);
		float Get2PieStepDist_nm();
		float GetStepSize_nm();
		float GetZRange_um();
		float Interpolate(float x0, float x, float x1, float y0, float y1);
		void GetIntenV2(int x, int y, float* fI, short N, eCH nChan, std::vector<SFrP1>& vP);
		//float SplineI(int x, int y, int n, float fPos, CHNL nChan);
		//void Resampling(int x, int y, std::vector<SPointf>& FSpec, CHNL nChan);
		//////////////////////////////////////////////////////////////////////////
		//BOOL GenBlueGreenPhaseMap(IMGL::CIM16& Im16_um, CHNL nChan);
		//BOOL CollectStripNoST(SFrin4& F1, int x, int y, int sz, CHNL nChan);
		//BOOL CollectStripT3(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//BOOL CollectStripNoST(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//BOOL CollectStripST(SFrin4& F1, int x, int y, CAM::SROI& R, CHNL nChan);
		//BOOL CollectStripGBST(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripRGBNoST(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripRGBST(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripRGBT3(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripRGNoST(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripRGST(SFrin4& F1, int x, int y, CAM::SROI& R);
		//BOOL CollectStripRGT3(SFrin4& F1, int x, int y, CAM::SROI& R);
	};
}
