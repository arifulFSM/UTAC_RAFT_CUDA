#pragma once

namespace CAM {
	enum ECAM { PRICAM, SECCAM, LASTCAM, INVD }; // Primary and secondary camera [6/8/2021 FSM]

	struct SCamProp {
		float fExposure = 1.f;
		float fGain = 1.f;
		float fGamma = 1.f;
		float fGainRed = 1.f;
		float fGainGrn1 = 1.f;
		float fGainGrn2 = 1.f;
		float fGainBlue = 1.f;
		float fBrightness = 0;
		float fContrast = 0;
		float fHue = 0, fSaturate = 0;
		// LucamGetProperty(m_hCamera, LUCAM_PROP_BRIGHTNESS, &fValue, &lFlags))
		USHORT nOffsetH = 0;
		USHORT nOffsetL = 0;
		// camera line-with calibration [11/4/2020 yuenl]
		float uppx = 0, uppy = 0; // um/pixel [11/5/2020 FSM]

		void Serialize(CArchive& ar);
		SCamProp& operator=(SCamProp& co) {
			fExposure = co.fExposure;
			fGain = co.fGain;
			fGamma = co.fGamma;
			fBrightness = co.fBrightness;
			fContrast = co.fContrast;
			nOffsetH = co.nOffsetH;
			nOffsetL = co.nOffsetL;
			uppx = co.uppx;
			uppy = co.uppy;
			fGainRed = co.fGainRed;
			fGainGrn1 = co.fGainGrn1;
			fGainGrn2 = co.fGainGrn2;
			fGainBlue = co.fGainBlue;
			fHue = co.fHue;
			fSaturate = co.fSaturate;
			return*this;
		}
		void LoadParam(short ID);
		void SaveParam(short ID);
	};

#define MAXSUPPORTCAM 2

	struct SCamPar {
		short nID = 0; // installation specific, do not serialize [10/16/2020 FSM]
		short maxCam = MAXSUPPORTCAM;
		short nCamWnd = 100;

		SCamProp Pa; // support 2 camera [6/9/2021 FSM]

		void Serialize(CArchive& ar);
		SCamPar& operator=(SCamPar& co) {
			nID = co.nID;
			nCamWnd = co.nCamWnd;
			Pa = co.Pa;
			return *this;
		}
		void LoadParam(ECAM eID);
		void SaveParam(ECAM eID);
	};
}
