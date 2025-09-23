#pragma once

#include "BaslerCameraDlg.h"
#include "../SRC/DOSUtil.h"
#include "../PSI/Strip.h"

class CBaslerCamDev : public Pylon::CImageEventHandler             // Allows you to get informed about received images and grab errors.
	, public Pylon::CConfigurationEventHandler     // Allows you to get informed about device removal.
	, public Pylon::CCameraEventHandler {
public:
	void StartContGrabbing();
	Pylon::CPylonBitmapImage m_bitmapImage;
	void SetBaslerCamProp();
	void StopGrabbing();
	void SendSWTrigger();
	float getbCamFPS();
	double resultingFPS();
	void SaveImages();
	int ImWidth, ImHeight, ImFrameRate;
	void UpdateImageProp();
	double exposureTime = 500;
	bool isScanActive = 0;
};

extern CBaslerCamDev gBCam;