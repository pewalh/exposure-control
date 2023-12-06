#pragma once

#include <cstdint>
#include <opencv2/opencv.hpp>


class ExposureCalculator
{
public:
	ExposureCalculator(std::string haarCascadeParamFile);
	~ExposureCalculator();
	
	void getExposureUpDown(cv::Mat& image, int& exposureChange, int& gainChange);

private:
	cv::CascadeClassifier faceCascade;
};

