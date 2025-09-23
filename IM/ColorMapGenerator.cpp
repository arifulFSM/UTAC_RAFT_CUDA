#include "pch.h"
#include "ColorMapGenerator.h"

IMGL::CColorMapGenerator::CColorMapGenerator() {
	GenColorMapJet();
}

IMGL::CColorMapGenerator::~CColorMapGenerator() {}

double IMGL::CColorMapGenerator::clamp(double v) {
	const double t = v < 0 ? 0 : v;
	return t > 1.0 ? 1.0 : t;
}

double IMGL::CColorMapGenerator::interpolate(double val, double y0, double x0, double y1, double x1) {
	return (val - x0) * (y1 - y0) / (x1 - x0) + y0;
}

double IMGL::CColorMapGenerator::base(double val) {
	if (val <= -0.75) return 0;
	else if (val <= -0.25) return interpolate(val, 0.0, -0.75, 1.0, -0.25);
	else if (val <= 0.25) return 1.0;
	else if (val <= 0.75) return interpolate(val, 1.0, 0.25, 0.0, 0.75);
	else return 0.0;
}

double IMGL::CColorMapGenerator::baseRed(double val) {
	double a = 0.33;
	if (val <= 0) return 0;
	else if (val <= a) return interpolate(val, 0.0, 0, 1.0, a);
	else return 1.0;
}

double IMGL::CColorMapGenerator::baseGreen(double val) {
	double a = 0.33;
	if (val <= -a) return interpolate(val, 0.0, -1.0, 1.0, -a);
	else if (val <= a) return 1.0;
	else return interpolate(val, 1.0, a, 0, 1.0);
}

double IMGL::CColorMapGenerator::baseBlue(double val) {
	double a = 0.33;
	if (val <= -a) return 1.0;
	else if (val <= 0) return interpolate(val, 1.0, -a, 0, 0);
	else return 0;
}

void IMGL::CColorMapGenerator::GenColorMapHot2Cold() {
	float sf = 2.0f / 255.0f;
	for (USHORT i = 0; i < 256; i++) {
		float a = i * sf - 1.0f;
		short r = short(baseRed(a) * 255);
		short g = short(baseGreen(a) * 255);
		short b = short(baseBlue(a) * 255);
		Map[i] = RGB(r, g, b);
	}
}

void IMGL::CColorMapGenerator::GenColorMapJet() {
	float sf = 2.0f / 255.0f;
	for (USHORT i = 0; i < 256; i++) {
		float a = i * sf - 1.0f;
		short r = short(base(a - 0.5) * 255);
		short g = short(base(a) * 255);
		short b = short(base(a + 0.5) * 255);
		Map[i] = RGB(r, g, b);
	}
}

void IMGL::CColorMapGenerator::GenColorMapGrey() {
	for (USHORT i = 0; i < 256; i++) {
		Map[i] = RGB(i, i, i);
	}
}

double IMGL::CColorMapGenerator::baseRedCB(double val) {
	double a = 0.333;
	if (val <= -a) return interpolate(val, 1.0, -1.0, 0, -a);
	else if (val <= a) return 0;
	else return interpolate(val, 0.0, a, 1.0, 1.0);
}

double IMGL::CColorMapGenerator::baseGreenCB(double val) {
	double a = 0.333;
	if (val <= -a) return interpolate(val, 0.0, -1.0, 1.0, -a);
	else if (val <= a) return interpolate(val, 1.0, -a, 0, a);
	else return 0;
}

double IMGL::CColorMapGenerator::baseBlueCB(double val) {
	double a = 0.333;
	if (val <= -a) return 0.0;
	else if (val <= a) return interpolate(val, 0.0, -a, 1.0, a);
	else return interpolate(val, 1.0, a, 0.0, 1.0);;
}

void IMGL::CColorMapGenerator::GenColorMapConstBrightness() {
	float sf = 2.0f / 255.0f;
	for (USHORT i = 0; i < 256; i++) {
		float a = i * sf - 1.0f;
		short r = short(baseRedCB(a) * 255);
		short g = short(baseGreenCB(a) * 255);
		short b = short(baseBlueCB(a) * 255);
		Map[i] = RGB(r, g, b);
	}
}

double IMGL::CColorMapGenerator::baseRedHM(double val) {
	if (val <= 0) return 0;
	else return interpolate(val, 0, 0, 1.0, 1.0);
}

double IMGL::CColorMapGenerator::baseGreenHM(double val) {
	if (val <= 0) return interpolate(val, 0.0, -1.0, 1.0, 0);
	else return interpolate(val, 1.0, 0, 0, 1.0);
}

double IMGL::CColorMapGenerator::baseBlueHM(double val) {
	if (val <= 0) return interpolate(val, 1.0, -1.0, 0, 0);
	else return 0;
}

void IMGL::CColorMapGenerator::GenColorMapHeatMap() {
	float sf = 2.0f / 255.0f;
	for (USHORT i = 0; i < 256; i++) {
		float a = i * sf - 1.0f;
		short r = short(baseRedHM(a) * 255);
		short g = short(baseGreenHM(a) * 255);
		short b = short(baseBlueHM(a) * 255);
		Map[i] = RGB(r, g, b);
	}
}

void IMGL::CColorMapGenerator::Dump(char* fname) {
	FILE* fp = fopen(fname, "wb");
	if (fp) {
		fprintf(fp, "No,Red,Green,Blue\n");
		for (USHORT i = 0; i < 256; i++) {
			COLORREF cr = Map[i];
			fprintf(fp, "%d,%d,%d,%d\n", i + 1, cr & 0xff, (cr >> 8) & 0xff, (cr >> 16) & 0xff);
		}
		fclose(fp);
	}
}

double IMGL::CColorMapGenerator::baseRedHSV(double val) {
	double a = 0.75, b = 0.25;
	if (val <= -a) return 1.0;
	else if (val <= -b) return interpolate(val, 1.0, -a, 0, -b);
	else if (val <= b) return 0;
	else if (val <= a) return interpolate(val, 0, b, 1.0, a);
	else return 1.0;
}

double IMGL::CColorMapGenerator::baseGreenHSV(double val) {
	double a = 0.75, b = 0.25;
	if (val <= -a) return interpolate(val, 0, -1.0, 1.0, -a);
	else if (val <= 0) return 1.0;
	else if (val <= b) return interpolate(val, 1.0, 0, 0, b);
	else return 0;
}

double IMGL::CColorMapGenerator::baseBlueHSV(double val) {
	double a = 0.75, b = 0.25;
	if (val <= -b) return 0;
	if (val <= 0) return interpolate(val, 0, -b, 1.0, 0);
	else if (val <= a) return 1.0;
	else return interpolate(val, 1.0, a, 0, 1.0);
}

void IMGL::CColorMapGenerator::GenColorMapHSV() {
	float sf = 2.0f / 255.0f;
	for (USHORT i = 0; i < 256; i++) {
		float a = i * sf - 1.0f;
		short r = short(baseRedHSV(a) * 255);
		short g = short(baseGreenHSV(a) * 255);
		short b = short(baseBlueHSV(a) * 255);
		Map[i] = RGB(r, g, b);
	}
}