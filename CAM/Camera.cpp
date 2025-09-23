#include "pch.h"

#include "SRC\DOSUtil.h"
#include "Camera.h"

using namespace CAM;

#ifdef _SIMULUCAM
CAM::CCamera Lucam;
#endif

BOOL CAM::CCamera::IsValid() { return true; }

CString CAM::CCamera::GetCameraInfo() {
	return L"Simulation camera";
}

CString CAM::CCamera::GetCameraFeatureInfo() {
	return L"No feature";
}

void CCamera::LoadParam() {
	CDosUtil DosUtil;  CString sec;
	sec = _T("Camera");
	subSampling = DosUtil.ReadCfgINI(sec, _T("SubSampling"), subSampling);

	int iID = int(eID);
	Par.Pa.LoadParam(iID);
	SCaM.LoadParam(iID);
}

void CCamera::SaveParam() {
	CDosUtil DosUtil; CString sec;
	sec = _T("Camera");
	DosUtil.WriteCfgINI(sec, _T("SubSampling"), subSampling);

	int iID = int(eID);
	Par.Pa.SaveParam(iID);
	SCaM.SaveParam(iID);
}

void CAM::CCamera::Initialize(ECAM ID) {
	eID = ID;
	LoadParam(); SaveParam();
}

BOOL CCamera::GrabOneFrame(IMGL::CIM& Im, int subSample) {
	if (!Im.Create(Wd, Ht, Bpp)) return FALSE;
	return TRUE;
}

BOOL CCamera::SaveOneFrame(char* filename, int subSample) {
	return TRUE;
}

BOOL CAM::CCamera::InitCapture(USHORT ID, short subSampling) {
	return TRUE;
}

BOOL CAM::CCamera::DeinitCapture(USHORT ID) {
	return 0;
}

BOOL CAM::CCamera::WhiteBalance() {
	return TRUE;
}

BOOL CAM::CCamera::Gamma(BOOL bOn) {
	return 0;
}

float CAM::CCamera::GetGamma() {
	return 0.0f;
}

float CAM::CCamera::GetHue() {
	return 0.0f;
}

BOOL CAM::CCamera::SetHue(float hu) {
	return 0;
}

BOOL CAM::CCamera::SetGamma(float gamma) {
	return 0;
}

void CAM::CCamera::Deinitialize(ECAM ID) {
	SaveParam();
}

BOOL CAM::CCamera::AutoExposure() {
	return 0;
}

BOOL CAM::CCamera::SetExposure(float exposure) {
	return 0;
}

float CAM::CCamera::GetSaturation() {
	return 0.0f;
}

BOOL CAM::CCamera::SetGain(float ga) {
	return 0;
}

float CAM::CCamera::GetGain() {
	return 0.0f;
}

BOOL CAM::CCamera::AutoGain() {
	return 0;
}

// camera specific

BOOL CAM::CCamera::GetCamProp(SCamProp& prop) { return TRUE; }

BOOL CAM::CCamera::SetCamProp(SCamProp& prop) { return TRUE; }

BOOL CAM::CCamera::StartFastGrab(SCamProp& prop, int subSample) { return TRUE; }

BOOL CAM::CCamera::StopFastGrab() { return TRUE; }

BOOL CAM::CCamera::FastGrab(IMGL::CIM& Im, SCamProp& prop) { return TRUE; }

BOOL CAM::CCamera::FastGrab(SCamProp& prop) { return TRUE; }

BOOL CAM::CCamera::Convert24(IMGL::CIM& Im) { return TRUE; }

bool CAM::CCamera::LucamPropPage() { return 1; }

bool CAM::CCamera::LucamVideoFormat() { return 1; }

BOOL CAM::CCamera::SetBrightness(float bright) { return 0; }

BOOL CAM::CCamera::SetContrast(float contrast) { return 0; }

float CAM::CCamera::GetBrightness() { return 0.0f; }

float CAM::CCamera::GetContrast() { return 0.0f; }

BOOL CAM::CCamera::SetSaturation(float saturate) { return 0; }

BOOL CCamera::SetProperties() { return TRUE; }

float CAM::CCamera::GetExposure() { return 0.0f; }

BOOL CAM::CCamera::StartStream(SCtx& Ctx, USHORT ID) { return TRUE; }

BOOL CAM::CCamera::StartStream() { return TRUE; };

BOOL CAM::CCamera::StopStream(SCtx& Ctx, USHORT ID) { return TRUE; }

BOOL CAM::CCamera::StopStream() { return TRUE; };

BOOL CAM::CCamera::GrabOneFrame(IMGL::CIM& Im) { return TRUE; };

CCamera::CCamera() {}

CCamera::~CCamera() {}

void CAM::SCam::LoadParam(int n) {
	n++;
	CDosUtil DosUtil;  CString sec, str;
	sec = _T("Camera");
	str.Format(_T("ID%d"), n);
	ID = DosUtil.ReadCfgINI(sec, str, ID);
	str.Format(_T("Name%d"), n);
	name = DosUtil.ReadCfgINI(sec, str.GetBuffer(), name.GetBuffer()).c_str();
}

void CAM::SCam::SaveParam(int n) {
	n++;
	CDosUtil DosUtil;  CString sec, str;
	sec = _T("Camera");
	str.Format(_T("ID%d"), n);
	DosUtil.WriteCfgINI(sec, str.GetBuffer(), ID);
	str.Format(_T("Name%d"), n);
	DosUtil.WriteCfgINI(sec, str.GetBuffer(), name);
}