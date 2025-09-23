#pragma once
#include "IM/IM.h"

struct PointInfo {
	float x;
	float y;
	float z;
};
class CAutoFocus {
private:
	IMGL::CIM BitmapImage;
	int wd;
	int ht;
	double calculateSharpness();
	void RGB2GRAY(double* buf, int skip);
	void Sobel(double* buf, double* magnitiude);
	double calculateStandardDeviation(double* magnitiude);
	double calculateMean(double* array);

public:
	CAutoFocus();
	float calibZPos;
	float tiltZ;
	void CalcCoeffs(float& a, float& b, float& c);
	float AutoFocus(int dir, double Range, double StepSize, BOOL multiDirection);
	double AutoFocus();
	float CalibrateFocus();
	void CenteringFringe();
};
