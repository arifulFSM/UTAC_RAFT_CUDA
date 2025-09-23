#include <pch.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include "IM/IM.h"

using namespace std;

struct Pixel {
	int x, y;
	Pixel() {};
	Pixel(int x, int y) : x(x), y(y) {}
};

struct _STATS {
	int x1 = -1, y1 = -1, x2 = -1, y2 = -1;
	_STATS() {}
};

class AutoTilt {
public:
	_STATS stats;
	std::vector<std::vector<Pixel>> largest_components, tmp_cmps;
	std::vector<Pixel>tComps;
	enum THRESH { FIRST, SECOND };
	std::vector<std::vector<int>>orgPixelValues;

	AutoTilt();
	void dfs(int x, int y, const std::vector<std::vector<int>>& image, std::vector<Pixel>& component, std::vector<std::vector<bool>>& visited);
	std::vector<std::vector<Pixel>> connectedComponents(const std::vector<std::vector<int>>& image);
	Pixel calculateCenterOfMass(const std::vector<std::vector<int>>& binaryImage);
	void searchingReg(std::vector<std::vector<int>>& OrgPixelValues, int ThresholdR);
	void SmoothData(int nTimes, int windowSize, vector<double>& data);
	int getPeakCount(const vector<double>& yAxisData, int windowSize);
	int GetSmoothingWindowSize(const vector<double>& data);
};

extern AutoTilt AT;