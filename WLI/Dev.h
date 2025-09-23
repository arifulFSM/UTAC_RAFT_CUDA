#pragma once

#include <memory>
#include <string>

#include "MOTOR/Piezo.h"
#include "CAM/CamDev.h"
#include "MOTOR/MotorCtrl.h"

#define INCTUV 5.f
#define INCUD  5.f

class CDev {
	CAM::CAMTYPE Type1 = CAM::SIMU, Type2 = CAM::SIMU;

	void Log(const std::string& msg, LOG::ELOGLEVEL lvl = LOG::NORMAL) {}

	void LoadParam();
	void SaveParam();

public:
	CAM::CCamDev Cam;
	std::unique_ptr<MOT::CMotorCtrl> MC;

public:
	CDev();
	virtual ~CDev();

public:
	bool Initialize();
	bool Deinitialize();

public:
	MOT::CStage& GetStage();
	MOT::CTilt& GetTilt();
	MOT::CPiezo& GetPiezo();
};

extern CDev Dev;
