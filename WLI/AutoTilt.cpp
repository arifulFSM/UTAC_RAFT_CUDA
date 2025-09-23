#include <pch.h>
#include "AutoTilt.h"

AutoTilt AT;

AutoTilt::AutoTilt() {}

// Function to perform depth-first search for connected component labeling
void AutoTilt::dfs(int x, int y, const std::vector<std::vector<int>>& image, std::vector<Pixel>& component, std::vector<std::vector<bool>>& visited) {
	if (x < 0 || x >= image.size() || y < 0 || y >= image[0].size() || visited[x][y] || image[x][y] == 0) {
		return;
	}

	visited[x][y] = true;
	component.push_back(Pixel(x, y));

	// Check neighboring pixels
	dfs(x - 1, y + 0, image, component, visited);
	dfs(x + 1, y + 0, image, component, visited);
	dfs(x + 0, y - 1, image, component, visited);
	dfs(x + 0, y + 1, image, component, visited);
	dfs(x - 1, y - 1, image, component, visited);
	dfs(x + 1, y + 1, image, component, visited);
	dfs(x + 1, y - 1, image, component, visited);
	dfs(x - 1, y + 1, image, component, visited);
}

// Function to perform connected component labeling
std::vector<std::vector<Pixel>> AutoTilt::connectedComponents(const std::vector<std::vector<int>>& image) {
	int rows = image.size();
	int cols = image[0].size();
	std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
	std::vector<std::vector<Pixel>> components;

	for (int x = 0; x < rows; ++x) {
		for (int y = 0; y < cols; ++y) {
			if (image[x][y] == 1 && !visited[x][y]) {
				std::vector<Pixel> component;
				dfs(x, y, image, component, visited);
				components.push_back(component);
			}
		}
	}

	return components;
}

// Function to calculate the center of mass of a binary image
Pixel AutoTilt::calculateCenterOfMass(const std::vector<std::vector<int>>& binaryImage) {
	int totalX = 0, totalY = 0, totalPixels = 0;
	for (int x = 0; x < binaryImage.size(); ++x) {
		for (int y = 0; y < binaryImage[0].size(); ++y) {
			if (binaryImage[x][y] == 1) {
				totalX += (x * orgPixelValues[x][y]);
				totalY += (y * orgPixelValues[x][y]);
				totalPixels += orgPixelValues[x][y];
			}
		}
	}

	if (totalPixels == 0) {
		// Avoid division by zero
		return Pixel(-1, -1);
	}

	int centerX = totalX / totalPixels;
	int centerY = totalY / totalPixels;

	return Pixel(centerX, centerY);
}

void AutoTilt::searchingReg(std::vector<std::vector<int>>& PixelValues, int ThresholdR) {
	int rows = PixelValues.size();
	if (!rows) return;
	int cols = PixelValues[0].size();
	int Thresh;

	(ThresholdR) ? Thresh = 27 : Thresh = 30; // Original Working
	//Thresh = 15;

	// Perform Binary Thresholding
	std::vector<std::vector<int>>imagePixelVal;
	for (int x = 0; x < rows; x++) {
		vector<int>rows;
		for (int y = 0; y < cols; y++) {
			(PixelValues[x][y] > Thresh) ? rows.push_back(1) : rows.push_back(0);
		}
		imagePixelVal.push_back(rows);
	}

	// Perform connected component labeling
	std::vector<std::vector<Pixel>> components = connectedComponents(imagePixelVal);

	// Sort components based on size in descending order
	std::sort(components.begin(), components.end(), [](const auto& a, const auto& b) {
		return a.size() > b.size();
		});

	// Extract the two largest components

	if (ThresholdR == 0) largest_components.push_back(components[0]);
	else {
		if (components[0].size() > (largest_components[0].size() / 3)) { // Original Working
			//if (components[0].size() > 5){
			largest_components.push_back(components[0]);
		}
	}

	// Create an output image with the two largest components

	std::vector<std::vector<int>> output_image1(rows, std::vector<int>(cols, 0));
	std::vector<std::vector<int>> output_image2(rows, std::vector<int>(cols, 0));
	bool flag = 0;
	tComps.clear();
	for (const auto& component : largest_components) {
		for (const auto& pixel : component) {
			if (ThresholdR == 0) {
				if (orgPixelValues[pixel.x][pixel.y] > Thresh + 100) {
					output_image1[pixel.x][pixel.y] = 1;
					tComps.push_back(pixel);
				}
				PixelValues[pixel.x][pixel.y] = -1;
			}
			else if (ThresholdR == 1 && !flag) {
				flag = 1;
				break;
			}
			else {
				if (orgPixelValues[pixel.x][pixel.y] > Thresh + 8) {
					tComps.push_back(pixel);
					output_image2[pixel.x][pixel.y] = 1;
				}
				PixelValues[pixel.x][pixel.y] = -2;
			}
		}
	}

	tmp_cmps.push_back(tComps);

	// Calculate the center of mass
	Pixel centerOfMass1, centerOfMass2;
	if (ThresholdR == 0)  centerOfMass1 = calculateCenterOfMass(output_image1);
	else centerOfMass2 = calculateCenterOfMass(output_image2);

	// Display the result
	if (ThresholdR == 0) {
		stats.x1 = centerOfMass1.x;
		stats.y1 = centerOfMass1.y;
	}
	else {
		stats.x2 = centerOfMass2.x;
		stats.y2 = centerOfMass2.y;
	}
}

void AutoTilt::SmoothData(int nTimes, int windowSize, vector<double>& data) {
	int idx = 1, cnt = 0;
	int sz = data.size();
	int ws = windowSize / 2;
	while (idx++ <= nTimes) {
		for (int j = ws; j < sz - ws; j++) {
			double val = 0.0;
			cnt = 0;
			for (int i = j - ws; i < j + ws; i++) {
				val += data[i];
				cnt++;
			}
			data[j] = val / cnt;
		}
	}
}

int AutoTilt::GetSmoothingWindowSize(const vector<double>& data) {
	vector<double>copyData;
	int sz = data.size();
	int windowSize = 100, nTime = 2, step = 5; // Need to set this value from config file
	int score, idx = 1;
	int LSize = (windowSize / step);
	while (idx++ <= LSize) {
		copyData.clear();
		for (auto X : data) copyData.push_back(X);
		SmoothData(nTime, windowSize, copyData);
		long long sum = 0;
		//ofstream myFile("E:\\FSMB\\RAFT-WLI\\FringStripeAnalysis\\WSCurveData.csv");
		for (int i = 0; i < sz; i++) {
			sum += abs(data[i] - copyData[i]);
			//myFile << data[i] << "," << copyData[i] << endl;
		}
		//myFile.close();
		if (sum < 5000) break; // Need to set this value from config file
		windowSize -= step;
	}
	return windowSize;
}

int AutoTilt::getPeakCount(const vector<double>& yAxisData, int windowSize) {
	// Calculate the first derivative using central differencing
	int totPeakCnt = 0;
	for (size_t i = windowSize; i < yAxisData.size() - windowSize; ++i) {
		double dy1 = (yAxisData[i + 1] - yAxisData[i]);
		double dy2 = (yAxisData[i + 2] - yAxisData[i + 1]);
		if (dy1 >= 0 && dy2 <= 0) totPeakCnt++;
	}

	return totPeakCnt;
}