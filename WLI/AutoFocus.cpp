#include "pch.h"
#include "AutoFocus.h"
#include "BaslerCamDev.h"
#include "Dev.h"
#include <iostream>
#include <fstream>
#include <utility>
//#include "MoSetup.h"
//#include "MOTOR/Motor.h"

CAutoFocus::CAutoFocus() {
	calibZPos = -1;
	tiltZ = -1;
}

void CAutoFocus::CalcCoeffs(float& a, float& b, float& c) {
	//197.56628
	float z1, z2, z3;
	Dev.MC.get()->tilt.devGetCurPos(MOT::MAXIS::T, &z1);
	Dev.MC.get()->tilt.devGetCurPos(MOT::MAXIS::U, &z2);
	Dev.MC.get()->tilt.devGetCurPos(MOT::MAXIS::V, &z3);

	/*z2 = z2 + 3.35f;
	z3 = z3 - 3.0f;*/

	PointInfo TPos, UPos, VPos;
	//TPos = { -197.56628, -197.56628, z1 };	//x-, y-	//1
	//UPos = { 197.56628 , -197.56628, z2 };	//x+, y-	//2
	//VPos = { 197.56628 , 197.56628, z3 };	//x+, y+	//3

	TPos = { -139.7, -139.7, -z1 };	//x-, y-	//1
	UPos = { 139.7 , -139.7, -z2 };	//x+, y-	//2
	VPos = { 139.7 , 139.7, -z3 };	//x+, y+	//3

	//TPos = { 10, 0, 23.491 };	//x-, y-	//1
	//UPos = { 10 , 10, 23.326 };	//x+, y-	//2
	//VPos = { 0 , 10, 23.302 };	//x+, y+	//3

	float D = TPos.x * (UPos.y - VPos.y) - TPos.y * (UPos.x - VPos.x) + (UPos.x * VPos.y - VPos.x * UPos.y);
	float Da = TPos.z * (UPos.y - VPos.y) - TPos.y * (UPos.z - VPos.z) + (VPos.y * UPos.z - UPos.y * VPos.z);
	float Db = TPos.x * (UPos.z - VPos.z) - TPos.z * (UPos.x - VPos.x) + (UPos.x * VPos.z - VPos.x * UPos.z);
	float Dc = TPos.x * (UPos.y * VPos.z - VPos.y * UPos.z) - TPos.y * (UPos.x * VPos.z - VPos.x * UPos.z) + TPos.z * (UPos.x * VPos.y - VPos.x * UPos.y);

	a = Da / D;
	b = Db / D;
	c = Dc / D;
}

//double CAutoFocus::AutoFocus() {
//	float a, b, c;
//	CalcCoeffs(a, b, c);
//	float x, y, z;
//	Dev.MC.get()->stage.devGetCurPos(MOT::MAXIS::X, &x);
//	Dev.MC.get()->stage.devGetCurPos(MOT::MAXIS::Y, &y);
//	Dev.MC.get()->stage.devGetCurPos(MOT::MAXIS::Z, &z);
//
//	float base = a * x + b * y + c;
//	z = ((oPtZ - z) + (oPtBase - base)) * 2;	//value of z in mm
//	Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, z, 20000, true);
//
//	return AutoFocus(-1, 150, 10, FALSE);
//}

float CAutoFocus::AutoFocus(int direction, double Range, double StepSize, BOOL multiDirection = FALSE) {
	//gBCam.StopGrabbing();
	std::ofstream myfile, myfile2;
	myfile.open("E:/SmoothScore.csv");
	//myfile2.open("F:/TEST/Score.csv");
	if (multiDirection) {
		Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, (direction * -1 * Range) / (2 * 1000.0f), 20000, true);
	}
	int StepCount = Range / StepSize;
	double MaxSharpness = -1, focusStepPosition = 0, Score, ScoreTemp;
	bool focusStart = false;
	std::vector<std::pair<double, int>>Sharpness;
	int pos = 0;
	CAM::ECAM eID = CAM::PRICAM;

	Dev.Cam.SetTriggerMode(CAM::PRICAM, true);
	Dev.MC.get()->stage.SetupZJog(0.12f, true);
	Dev.MC.get()->stage.StartZJog();

	for (int Step = 0; Step < 1000; Step++) {
		//gBCam.isScanActive = TRUE;
		//gBCam.SendSWTrigger();
		//if (!gBCam.SnapOneFrame()) {
		//	//2ND TRY
		//	gBCam.isScanActive = TRUE;
		//	gBCam.SendSWTrigger();
		//	int trial = 0;
		//	while (!gBCam.SnapOneFrame() && trial < 6) {
		//		trial++;
		//		gBCam.SendSWTrigger();
		//	}
		//	if (trial >= 6) {
		//		AfxMessageBox(L"Image capture failed!!!");
		//		continue;
		//	}
		//}
		//gBCam.isScanActive = FALSE;
		//BitmapImage.Attach(gBCam.m_bitmapImage);
		//CString temp;
		//temp.Format(L"F:/TEST/%d.BMP", Step);
		//BitmapImage.Save(temp);

		/*while (!Dev.Cam.Grab(BitmapImage, eID, Dev.Cam.pCm[eID]->subSampling)) {
		}*/
		Dev.Cam.ExecuteTrigger(CAM::PRICAM);
		while (!Dev.Cam.GetBitmapImage(BitmapImage, CAM::PRICAM));

		wd = BitmapImage.GetWidth();
		ht = BitmapImage.GetHeight();
		//Sharpness.push_back({ calculateSharpness(), pos });
		ScoreTemp = calculateSharpness();
		if (Step == 0) {
			Score = ScoreTemp;
		}
		else if (ScoreTemp >= Score * 5.0) {
			focusStart = true;
			Score = ScoreTemp;
			//Dev.MC.get()->stage.StopZJog();
			//return 0.0;
		}
		else if (focusStart && ScoreTemp < Score) {
			BitmapImage.Detach();
			Dev.Cam.SetTriggerMode(CAM::PRICAM, false);
			Dev.MC.get()->stage.StopZJog();
			return 0.010;
		}
		//myfile << Sharpness[pos].first << "\n";
		pos++;
		BitmapImage.Detach();
		//Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, direction * StepSize / 1000.0f, 20000, true);
		//Dev.MC.get()->stage.UpdatePositions();
	}
	Dev.Cam.SetTriggerMode(CAM::PRICAM, false);
	Dev.MC.get()->stage.StopZJog();
	return 0.0;
	StepCount = pos;
	double* SmoothData;
	SmoothData = new double[pos];
	SmoothData[0] = Sharpness[0].first;
	SmoothData[pos - 1] = Sharpness[pos - 1].first;
	double sum = 0.0;
	for (int i = 1; i < pos - 1; i++) {
		/*sum = 0.0;
		for (int j = i - 1; j <= i + 1; j++) {
			sum += Sharpness[j].first;
		}
		SmoothData[i] = sum / 3.0;
		myfile << SmoothData[i] << "\n";*/
		SmoothData[i] = Sharpness[i].first;
	}

	double* cumLeft, * cumRight;
	cumLeft = new double[pos];
	cumRight = new double[pos];
	cumLeft[0] = 0;
	cumRight[pos - 1] = 0;

	BOOL isPositive = TRUE;
	for (int i = 1; i < pos; i++) {
		if (SmoothData[i] >= SmoothData[i - 1]) {
			if (isPositive) {
				cumLeft[i] = cumLeft[i - 1] + (SmoothData[i] - SmoothData[i - 1]);
			}
			else {
				cumLeft[i] = (SmoothData[i] - SmoothData[i - 1]);
				isPositive = TRUE;
			}
		}
		else {
			if (!isPositive) {
				cumLeft[i] = cumLeft[i - 1] + (SmoothData[i] - SmoothData[i - 1]);
			}
			else {
				cumLeft[i] = (SmoothData[i] - SmoothData[i - 1]);
				isPositive = FALSE;
			}
		}
	}

	isPositive = TRUE;
	for (int i = pos - 1; i >= 0; i--) {
		if (SmoothData[i] >= SmoothData[i + 1]) {
			if (isPositive) {
				cumRight[i] = cumRight[i + 1] + (SmoothData[i] - SmoothData[i + 1]);
			}
			else {
				cumRight[i] = (SmoothData[i] - SmoothData[i + 1]);
				isPositive = TRUE;
			}
		}
		else {
			if (!isPositive) {
				cumRight[i] = cumRight[i + 1] + (SmoothData[i] - SmoothData[i + 1]);
			}
			else {
				cumRight[i] = (SmoothData[i] - SmoothData[i + 1]);
				isPositive = FALSE;
			}
		}
	}
	/*for (int i = 1; i < pos; i++) {
		if (SmoothData[i] > SmoothData[i - 1]) {
			cumLeft[i] = (SmoothData[i] - SmoothData[i - 1]);
		}
		else {
			cumLeft[i] = 0;
		}
	}

	for (int i = pos - 2; i >= 0; i--) {
		if (SmoothData[i] > SmoothData[i + 1]) {
			cumRight[i] = (SmoothData[i] - SmoothData[i + 1]);
		}
		else {
			cumRight[i] = 0;
		}
	}*/

	BOOL isPositiveMax = FALSE;
	int maxi;
	for (int i = 1; i < pos - 1; i++) {
		if (cumLeft[i] > 0 && cumRight[i] > 0) {
			if (std::min(cumLeft[i], cumRight[i]) > MaxSharpness) {
				MaxSharpness = std::min(cumLeft[i], cumRight[i]);
				focusStepPosition = Sharpness[i].second;
				maxi = i;
				isPositiveMax = TRUE;
			}
		}
		else if (cumLeft[i] < 0 && cumRight[i] < 0) {
			if (abs(std::max(cumLeft[i], cumRight[i])) > MaxSharpness) {
				MaxSharpness = abs(std::max(cumLeft[i], cumRight[i]));
				focusStepPosition = Sharpness[i].second;
				maxi = i;
				isPositiveMax = FALSE;
			}
		}
	}

	//Dev.MC
	/*float offset = 0.0;
	if ((maxi - 1) >= 0 && (maxi + 1) < Sharpness.size() && abs(Sharpness[maxi - 1].first) < abs(Sharpness[maxi + 1].first)) {
		if ((abs(Sharpness[maxi].first - Sharpness[maxi + 1].first)) > (abs(Sharpness[maxi + 1].first) - abs(Sharpness[maxi + 2].first))) {
			focusStepPosition = maxi + 1;
		}
		else {
			offset += 0.005;
		}
	}
	else if ((maxi - 1) >= 0 && (maxi + 1) < Sharpness.size()) {
	}*/
	direction *= -1;	//reverse direction
	myfile.close();
	//myfile2.close();
	//delete[] SmoothData;
	delete[] cumLeft;
	delete[] cumRight;
	//Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, direction * StepSize * (StepCount - focusStepPosition) / 1000.0f, 20000, true);
	/*if (isPositiveMax) {
		if (Sharpness[focusStepPosition - 1].first > Sharpness[focusStepPosition].first && Sharpness[focusStepPosition - 1].first > Sharpness[focusStepPosition + 1].first) {
			focusStepPosition -= 1;
		}
		else if (Sharpness[focusStepPosition + 1].first > Sharpness[focusStepPosition - 1].first && Sharpness[focusStepPosition + 1].first > Sharpness[focusStepPosition].first) {
			focusStepPosition += 1;
		}
	}
	else {
		if (Sharpness[focusStepPosition - 1].first < Sharpness[focusStepPosition].first && Sharpness[focusStepPosition - 1].first < Sharpness[focusStepPosition + 1].first) {
			focusStepPosition -= 1;
		}
		else if (Sharpness[focusStepPosition + 1].first < Sharpness[focusStepPosition - 1].first && Sharpness[focusStepPosition + 1].first < Sharpness[focusStepPosition].first) {
			focusStepPosition += 1;
		}
	}*/
	return direction * StepSize * (StepCount - focusStepPosition) / 1000.0;
}

float CAutoFocus::CalibrateFocus() {
	int Step = 5;
	Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, (-1 * Step) / 1000.0f, 20000, true);
	std::vector<float>Score;
	CAM::ECAM eID = CAM::PRICAM;
	for (int i = 0; i < 3; i++) {
		while (!Dev.Cam.Grab(BitmapImage, eID, Dev.Cam.pCm[eID]->subSampling)) {
		}
		wd = BitmapImage.GetWidth();
		ht = BitmapImage.GetHeight();
		Score.push_back(calculateSharpness());
		if (i < 2) {
			Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, Step / 1000.0f, 20000, true);
		}
	}

	float maxScore = -10000;
	int maxi;
	for (int i = 0; i < Score.size(); i++) {
		if (maxScore < Score[i]) {
			maxScore = Score[i];
			maxi = i;
		}
	}
	float position = -1 * (3 - maxi - 1) / 1000.0;
	return position;
}

double CAutoFocus::calculateSharpness() {
	int skip = 2;
	double* buf = new double[(ht / skip) * (wd / skip)];

	RGB2GRAY(buf, skip);
	wd /= skip;
	ht /= skip;

	double* magnitiude = new double[wd * ht];
	Sobel(buf, magnitiude);
	double standardDev = calculateStandardDeviation(magnitiude);

	delete[] buf;
	delete[] magnitiude;

	return standardDev;
}

void CAutoFocus::RGB2GRAY(double* buf, int skip) {
	double coeffs[] = { 0.299, 0.587, 0.114 };

	COLORREF color;
	for (int i = 0; i < ht / skip; i++) {
		for (int j = 0; j < wd / skip; j++) {
			color = BitmapImage.GetPixel(j * skip, i * skip);
			buf[(i * (wd / skip)) + j] = GetRValue(color) * coeffs[0] + GetGValue(color) * coeffs[1] + GetBValue(color) * coeffs[2];
		}
	}
}

void CAutoFocus::Sobel(double* buf, double* magnitiude) {
	int kernelX[3][3] = { {-1,0,1}, {-2,0,2}, {-1,0,1} };
	int kernelY[3][3] = { {-1,-2,-1}, {0,0,0}, {1,2,1} };
	double gX = 0.0, gY = 0.0;

	for (int y = 1; y < ht - 1; y++) {
		for (int x = 1; x < wd - 1; x++) {
			gX = 0.0;
			gY = 0.0;
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					gX += kernelX[i + 1][j + 1] * buf[(y + i) * wd + (x + j)];
					gY += kernelY[i + 1][j + 1] * buf[(y + i) * wd + (x + j)];
				}
			}
			magnitiude[y * wd + x] = (gX * gX) + (gY * gY);
		}
	}
}

double CAutoFocus::calculateStandardDeviation(double* magnitiude) {
	double mean = calculateMean(magnitiude);

	double* squaredDifference = new double[ht * wd];
	double diff;
	for (int i = 1; i < ht - 1; i++) {
		for (int j = 1; j < wd - 1; j++) {
			diff = magnitiude[i * wd + j] - mean;
			squaredDifference[i * wd + j] = diff * diff;
		}
	}

	double meanSquaredDiff = calculateMean(squaredDifference);

	delete[] squaredDifference;
	return meanSquaredDiff;
};

double CAutoFocus::calculateMean(double* array) {
	double sum = 0.0;
	for (int i = 1; i < ht - 1; i++) {
		for (int j = 1; j < wd - 1; j++) {
			sum += array[i * wd + j];
		}
	}
	return sum / ((ht - 2) * (wd - 2));
};

void CAutoFocus::CenteringFringe() {
	bool centeringDone = false;
	double coeffs[] = { 0.299, 0.587, 0.114 };
	bool isUp = false, isDown = false;
	int numberOfFringe = 0;
	CAM::ECAM eID = CAM::PRICAM;
	COLORREF color;
	double val;
	//std::ofstream myfile;
	//myfile.open("E:/center.csv");
	double maxVal, minVal, maxVariation;
	int step = 0, maxVarPos;
	while (!centeringDone && step < 25) {
		while (!Dev.Cam.Grab(BitmapImage, eID, Dev.Cam.pCm[eID]->subSampling)) {
		}

		wd = BitmapImage.GetWidth();
		ht = BitmapImage.GetHeight();
		int i = ht / 2;
		maxVariation = -1;
		maxVarPos = -1;
		for (int j = 0; j < wd - 15; j++) {
			maxVal = 0;
			minVal = 256;
			/*color = BitmapImage.GetPixel(j - 1, i);
			val1 = GetRValue(color) * coeffs[0] + GetGValue(color) * coeffs[1] + GetBValue(color) * coeffs[2];*/
			//color = BitmapImage.GetPixel(j - 1, i);
			//myfile << val1 << "\n";
			for (int k = j; k < j + 15; k++) {
				color = BitmapImage.GetPixel(k, i);
				val = GetRValue(color) * coeffs[0] + GetGValue(color) * coeffs[1] + GetBValue(color) * coeffs[2];
				maxVal = std::max(maxVal, val);
				minVal = std::min(minVal, val);
				if (maxVariation < (maxVal - minVal)) {
					maxVariation = maxVal - minVal;
					maxVarPos = j;
				}
			}
			//myfile << maxVal << "," << minVal << "\n";
		}

		if (((wd / 2 - 15) <= maxVarPos && wd / 2 >= maxVarPos) || ((wd / 2 + 15) >= maxVarPos && wd / 2 <= maxVarPos)) {
			centeringDone = true;
			//break;
		}
		else if ((wd / 2) > maxVarPos) {
			Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, 1 / 1000.0f, 20000, true);
			//break;
		}
		else if ((wd / 2) < maxVarPos) {
			Dev.MC.get()->stage.MoveR(MOT::MAXIS::Z, -1 / 1000.0f, 20000, true);
			//break;
		}

		//centeringDone = true;
		//myfile.close();
		step++;
	}
	//myfile.close();
}