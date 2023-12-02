#pragma once

#include <cstdint>
#include <opencv2/opencv.hpp>


class ExposureCalculator
{
public:
	ExposureCalculator(float saturationLimit = 0.05f);
	~ExposureCalculator();
	
	int getExposureUpDown(cv::Mat& image);

private:
	float saturationLimit;
};

