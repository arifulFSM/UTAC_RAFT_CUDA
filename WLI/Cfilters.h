#pragma once
//#include<opencv2/opencv.hpp>

//using namespace cv;
class Cfilters {
	//Mat convertVectorToMat(std::vector<std::vector<float>>& img);
	//std::vector<std::vector<float>> convertMatToVector(Mat& imgMat);
	float calculateMostFreqVal(std::vector<std::vector<float>> img);
	float calculateStandardDeviation(std::vector<std::vector<float>>& img);
public:
	void ApplySmoothing(std::vector<std::vector<float>>& img);
	void removeOutliers(std::vector<std::vector<float>>& img);
	void removeOutliers(std::vector<float>& HeightData, int wd, int ht);
};
