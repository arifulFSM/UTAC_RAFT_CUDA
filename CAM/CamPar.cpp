#include "pch.h"
#include "SRC\DOSUtil.h"

#include "CamPar.h"

void CAM::SCamPar::Serialize(CArchive& ar) {
	USHORT magic = 3;
	short maxCam = MAXSUPPORTCAM;
	if (ar.IsStoring()) {
		ar << magic;
		ar << nCamWnd;
		Pa.Serialize(ar);
	}
	else {
		ar >> magic;
		ar >> nCamWnd;
		Pa.Serialize(ar);
	}
}

void CAM::SCamPar::LoadParam(ECAM eID) {
	int iID = int(eID);
	Pa.LoadParam(iID + 1);
}

void CAM::SCamPar::SaveParam(ECAM eID) {
	int iID = int(eID);
	Pa.SaveParam(iID + 1);
}

void CAM::SCamProp::Serialize(CArchive& ar) {
	USHORT magic = 2;
	if (ar.IsStoring()) {
		ar << magic;
		ar << fExposure;
		ar << fGain;
		ar << fGamma;
		ar << nOffsetH;
		ar << nOffsetL;
		ar << fBrightness;
		ar << fContrast;
		ar << uppx;
		ar << uppy;
	}
	else {
		ar >> magic;
		ar >> fExposure;
		ar >> fGain;
		ar >> fGamma;
		ar >> nOffsetH;
		ar >> nOffsetL;
		ar >> fBrightness;
		ar >> fContrast;
		ar >> uppx;
		ar >> uppy;
	}
}

void CAM::SCamProp::LoadParam(short ID) {
	ID++;
	CDosUtil DosUtil;
	CString sec;
	if (ID == 1) sec = _T("LucamProp1"); else sec = _T("LucamProp2");
	fExposure = DosUtil.ReadCfgINI(sec, _T("Exposure"), fExposure);
	fGain = DosUtil.ReadCfgINI(sec, _T("Gain"), fGain);
	fGamma = DosUtil.ReadCfgINI(sec, _T("Gamma"), fGamma);
	uppx = DosUtil.ReadCfgINI(sec, _T("UPPX"), uppx);
	uppy = DosUtil.ReadCfgINI(sec, _T("UPPY"), uppy);
}

void CAM::SCamProp::SaveParam(short ID) {
	ID++;
	CDosUtil DosUtil;
	CString sec;
	if (ID == 1) sec = _T("LucamProp1"); else sec = _T("LucamProp2");
	DosUtil.WriteCfgINI(sec, _T("Exposure"), fExposure);
	DosUtil.WriteCfgINI(sec, _T("Gain"), fGain);
	DosUtil.WriteCfgINI(sec, _T("Gamma"), fGamma);
	DosUtil.WriteCfgINI(sec, _T("UPPX"), uppx);
	DosUtil.WriteCfgINI(sec, _T("UPPY"), uppy);
}