#include "exposure-calculator.h"
#include <opencv2/opencv.hpp>

#include "colors.h"


ExposureCalculator::ExposureCalculator(std::string haarCascadeParamFile) {
    faceCascade.load(haarCascadeParamFile);
}

ExposureCalculator::~ExposureCalculator() {
}


/// <summary>
/// Detect a face in the image and calculate if the exposure should be increased or decreased
/// </summary>
void ExposureCalculator::getExposureUpDown(cv::Mat& image, int& exposureChange, int& gainChange) {

    // Convert image to grayscale
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);


    // Detect faces
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

    if (faces.size() == 0) {
        // no faces detected
        // create a mock face in the center of the image
        int centerX = gray.cols / 2;
        int centerY = gray.rows / 2;
        int faceHeight = gray.rows / 2;
        int faceWidth = faceHeight;
        cv::Rect face(centerX - faceWidth / 2, centerY - faceHeight / 2, faceWidth, faceHeight);
        faces.push_back(face);
    }

    // get the index of the largest face
    int largestFaceIndex = 0;
    int largestFaceArea = 0;
    cv::Rect roi;
    for (int i = 0; i < faces.size(); i++) {
        int area = faces[i].width * faces[i].height;
        if (area > largestFaceArea) {
            largestFaceArea = area;
            largestFaceIndex = i;
            roi = faces[i];
        }
    }

    // shrink the ROI to avoid the edges of the face
    double shrink = 0.4;
    roi.x += roi.width * shrink * 0.5;
    roi.y += roi.height * shrink * 0.5;
    roi.width -= roi.width * shrink;
    roi.height -= roi.height * shrink;



    

    // histogram of pixel values 0-255 within the ROI
    int histogram[256] = { 0 };

    for (int y = roi.y; y < roi.y + roi.height; y++) {
        for (int x = roi.x; x < roi.x + roi.width; x++) {
            // get the gray scale pixel value
            uchar pixel = gray.at<uchar>(y, x);
            histogram[pixel]++;
        }
    }

    // display the histogram
    int hist_w = 256; 
    int hist_h = 200;
    

    cv::Mat histImage(hist_h, hist_w, CV_8UC3);
    // Set all pixels to grayscale 255
    histImage = LIGHTGRAY;

    // find the maximum intensity element from histogram
    int max = histogram[0];
    for (int i = 1; i < 256; i++) {
        if (histogram[i] > max) {
            max = histogram[i];
        }
    }

    



    // get the median pixel value
    int medianPixelValue = 0;
    int pixelCount = 0;
    int medianPixelCount = roi.width * roi.height / 2;
    for (int i = 0; i < 256; i++) {
        pixelCount += histogram[i];
        if (pixelCount >= medianPixelCount) {
            medianPixelValue = i;
            break;
        }
    }

    int wantedVal = 90;
    int noChangeExposureWidth = 30;
    int noChangeGainWidth = 5;


    if (medianPixelValue > (wantedVal + noChangeExposureWidth)) {
        exposureChange = -1; // suggest decreasing exposure
    }
    else if (medianPixelValue < (wantedVal - noChangeExposureWidth)) {
        exposureChange = 1; // suggest increasing exposure
    }
    else {
        exposureChange = 0; // suggest no change
    }
    if (exposureChange == 0) {
        if (medianPixelValue > (wantedVal + noChangeGainWidth)) {
            gainChange = -1; // suggest decreasing gain
        }
        else if (medianPixelValue < (wantedVal - noChangeGainWidth)) {
            gainChange = 1; // suggest increasing gain
        }
        else {
            gainChange = 0; // suggest no change
        }
    }
    else {
        gainChange = 0; // suggest no change
    }
    
    
    // draw the intensity line for histogram
    for (int i = 0; i < 256; i++) {
        int val = (int)(((double)histogram[i] / (double)max) * (double)hist_h);
        cv::line(histImage, cv::Point(i, hist_h), cv::Point(i, hist_h - val), BLUE, 1, cv::LINE_4);
    }

    // draw the median line and the wanted value line
    cv::line(histImage, cv::Point(medianPixelValue, 0), cv::Point(medianPixelValue, hist_h), RED, 1, cv::LINE_4);
    cv::line(histImage, cv::Point(wantedVal, 0), cv::Point(wantedVal, hist_h), GREEN, 1, cv::LINE_4);


    // display histogram
    cv::imshow("Histogram", histImage);

    
    // Draw the ROI rect for the detected faces
    for (int i = 0; i < faces.size(); i++) {
        cv::Rect face = faces[i];
        cv::Scalar roiColor = (i == largestFaceIndex) ? PRIMARY_ROI_COLOR : SECONDARY_ROI_COLOR;
        cv::rectangle(image, roi, roiColor, 2, cv::LINE_AA);
    }
    
}
