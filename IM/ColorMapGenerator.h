#pragma once

#define CRRED(x) (x & 0xFF)
#define CRGREEN(x) ((x >> 8) & 0xFF)
#define CRBLUE(x) ((x >> 16) & 0xFF)

namespace IMGL {
	class CColorMapGenerator {
	public:
		COLORREF Map[256] = { 0 };

	public:
		CColorMapGenerator();
		virtual~CColorMapGenerator();

	public:
		double clamp(double v);
		double interpolate(double val, double y0, double x0, double y1, double x1);
		double base(double val);

		double baseRed(double val);
		double baseGreen(double val);
		double baseBlue(double val);

		double baseRedCB(double val);
		double baseBlueCB(double val);
		double baseGreenCB(double val);

		double baseRedHM(double val);
		double baseGreenHM(double val);
		double baseBlueHM(double val);

	public:
		void GenColorMapHot2Cold();
		void GenColorMapJet();
		void GenColorMapGrey();
		void GenColorMapConstBrightness();
		void GenColorMapHeatMap();
		double baseRedHSV(double val);
		double baseGreenHSV(double val);
		double baseBlueHSV(double val);
		void GenColorMapHSV();
		void Dump(char* fname);
	};
}
