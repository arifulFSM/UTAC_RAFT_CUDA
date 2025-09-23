#include "pch.h"
#include "Cfilters.h"
//#include <opencv2/opencv.hpp>
#include <algorithm>
#include <math.h>
#include <map>
#include <unordered_map>

//using namespace cv;

void Cfilters::ApplySmoothing(std::vector<std::vector<float>>& img) {
	int rows = img.size();
	int cols = img[0].size();

	std::vector<std::vector<float>> temp(img); // Make a copy of the original data to preserve original values during calculation

	for (int i = 1; i < rows - 1; ++i) // Start from 1 and end at rows-1 to avoid accessing out of bounds
	{
		for (int j = 1; j < cols - 1; ++j) // Same for columns
		{
			float sum = 0;
			// Sum the values of the current pixel and its neighbors
			for (int ki = -1; ki <= 1; ++ki) {
				for (int kj = -1; kj <= 1; ++kj) {
					sum += temp[i + ki][j + kj];
				}
			}
			// Assign the average value back to the original data
			img[i][j] = sum / 9; // 9 is the number of values in a 3x3 kernel
		}
	}
}

//Mat Cfilters::convertVectorToMat(std::vector<std::vector<float>>& img) {
//	int rows = img.size();
//	int cols = img[0].size();
//
//	Mat mat(rows, cols, CV_32F);
//
//	// Copy data from vector to Mat
//	for (int i = 0; i < rows; ++i) {
//		// Get pointer to the beginning of the i-th row in the Mat
//		float* ptr = mat.ptr<float>(i);
//
//		// Copy data from vector to Mat row
//		for (int j = 0; j < cols; ++j) {
//			ptr[j] = img[i][j];
//		}
//	}
//
//	return mat;
//}
//
//std::vector<std::vector<float>> Cfilters::convertMatToVector(Mat& imgMat) {
//	int rows = imgMat.rows;
//	int cols = imgMat.cols;
//
//	// Create a vector<vector<float>> with the same size as the Mat
//	std::vector<std::vector<float>> img(rows, std::vector<float>(cols));
//
//	// Copy data from Mat to vector
//	for (int i = 0; i < rows; ++i) {
//		const float* ptr = imgMat.ptr<float>(i);
//		for (int j = 0; j < cols; ++j) {
//			img[i][j] = ptr[j];
//		}
//	}
//
//	return img;
//}

float Cfilters::calculateMostFreqVal(std::vector<std::vector<float>> img) {
	int rows = img.size();
	int cols = img[0].size();

	std::unordered_map<int, int>freq;
	int key;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			float value = round(img[row][col] * 1000.00f);
			key = (int)value;
			freq[key]++;
		}
	}
	int mxCnt = -1000, mxVal;
	for (auto x : freq) {
		if (mxCnt < x.second) {
			mxCnt = x.second;
			mxVal = x.first;
		}
	}

	float freqVal = (float)mxVal;
	freqVal /= 1000.00f;

	return freqVal;
}

float Cfilters::calculateStandardDeviation(std::vector<std::vector<float>>& img) {
	// Flatten the 2D vector into a 1D vector
	std::vector<float> flatData;
	for (const auto& row : img) {
		flatData.insert(flatData.end(), row.begin(), row.end());
	}

	// Calculate mean
	float sum = 0.0;
	for (const float value : flatData) {
		sum += value;
	}
	float mean = sum / flatData.size();

	// Calculate variance
	float variance = 0.0;
	for (const float value : flatData) {
		variance += (value - mean) * (value - mean);
	}
	variance /= flatData.size();

	// Return standard deviation (square root of variance)
	return std::sqrt(variance);
	//return variance;
}

void Cfilters::removeOutliers(std::vector<std::vector<float>>& img) {
	ApplySmoothing(img);
	int rows = img.size();
	int cols = img[0].size();

	std::vector<std::vector<float>> temp(img); // Make a copy of the original data to preserve original values during calculation

	float freqVal = calculateMostFreqVal(img);// we will replace spikes with this value
	float threshold = calculateStandardDeviation(img);
	for (int i = 0; i < rows; ++i) // Start from 1 and end at rows-1 to avoid accessing out of bounds
	{
		for (int j = 0; j < cols; ++j) // Same for columns
		{
			if (abs(img[i][j] - freqVal) > threshold * 2)img[i][j] = freqVal;
		}
	}
}

void Cfilters::removeOutliers(std::vector<float>& HeightData, int wd, int ht) {
	std::vector<std::vector<float>> img;
	for (int i = 0; i < ht - 1; i++) {
		std::vector<float>data;
		for (int j = 0; j < wd - 1; j++) {
			data.push_back(HeightData[i * (wd - 1) + j]);
		}
		img.push_back(data);
	}
	removeOutliers(img);
	for (int i = 0; i < ht - 1; i++) {
		for (int j = 0; j < wd - 1; j++) {
			HeightData[i * (wd - 1) + j] = img[i][j];
		}
	}
}