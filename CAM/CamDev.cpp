#include "pch.h"

#include "CAM/LucamDev.h"
#include "BaslerColourCam.h"
#include "BaslerMonoCam.h"

#include "CamDev.h"

using namespace CAM;

// CAM::CCamDev CamDev;

void CAM::CCamDev::Clear(ECAM ID) {
	int iID = int(ID);
	if (pCm[iID]) { delete pCm[iID]; pCm[iID] = nullptr; }
}

CAM::CCamDev::CCamDev() {}

CAM::CCamDev::~CCamDev() {
	Clear(ECAM::PRICAM);
	Clear(ECAM::SECCAM);
}

BOOL CAM::CCamDev::StartDev(CAM::CAMTYPE Type, ECAM ID) {
	//int iID = int(ID);
	Clear(ID);
	switch (Type) {
#ifndef _SIMULUCAM
	case CAM::LUCAM:
		pCm[ID] = new CLucamDev;
		break;
#endif
#ifndef _SIMUBASCOLRCAM
	case CAM::BASCOLRCAM:
		pCm[ID] = new CBaslerColourCam;
		break;
#endif
#ifndef _SIMUBASMONOCAM
	case CAM::BASMONOCAM:
		pCm[ID] = new CBaslerMonoCam;
		break;
#endif
	default:
	case CAM::SIMU:
		pCm[ID] = new CCamera;
		break;
	}
	if (!pCm[ID]) return FALSE;
	pCm[ID]->Initialize(ID);
	return pCm[ID]->InitCapture(pCm[ID]->SCaM.ID, pCm[ID]->subSampling);
}

BOOL CAM::CCamDev::StopDev(ECAM ID) {
	int iID = int(ID);
	if (!pCm[iID]) return TRUE;
	pCm[iID]->DeinitCapture(pCm[iID]->SCaM.ID);
	pCm[iID]->Deinitialize(ID);
	if (pCm[iID] != nullptr) {
		delete pCm[iID]; pCm[iID] = nullptr;
	}
	return TRUE;
}

BOOL CAM::CCamDev::Grab(IMGL::CIM& Im, ECAM ID, int subSample) {
	int iID = int(ID);
	if (!pCm[iID]) { ASSERT(0); return FALSE; }
	if (pCm[iID]->GrabOneFrame(Im, subSample)) return TRUE;
	return FALSE;
}

CCamera* CAM::CCamDev::GetCamera(ECAM ID) {
	int iID = int(ID);
	return pCm[iID];
}

CString CAM::CCamDev::GetCameraInfo(ECAM ID) {
	int iID = int(ID);
	if (!pCm[iID]) return L"Camera not initialized";
	return pCm[iID]->GetCameraInfo();
}

BOOL CAM::CCamDev::ExecuteTrigger(ECAM ID) {
	int iID = int(ID);
	if (!pCm[iID]) { return FALSE; }
	if (pCm[iID]->ExecuteTrigger()) return TRUE;
	return FALSE;
}

BOOL CAM::CCamDev::SetTriggerMode(ECAM ID, bool isTrigOn) {
	int iID = int(ID);
	if (!pCm[iID]) { return FALSE; }
	if (pCm[iID]->SetTriggerMode(isTrigOn)) return TRUE;
	return FALSE;
}

BOOL CAM::CCamDev::GetBitmapImage(IMGL::CIM& Im, ECAM ID) {
	int iID = int(ID);
	if (!pCm[iID]) { return FALSE; }
	if (pCm[iID]->GetBitmapImage(Im)) return TRUE;
	return FALSE;
}