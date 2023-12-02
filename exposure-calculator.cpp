#include "exposure-calculator.h"
#include <opencv2/opencv.hpp>

#include "colors.h"


ExposureCalculator::ExposureCalculator(float saturationLimit) {
    this->saturationLimit = saturationLimit;
}

ExposureCalculator::~ExposureCalculator() {
}


/// <summary>
/// Detect a face in the image and calculate if the exposure should be increased or decreased
/// </summary>
int ExposureCalculator::getExposureUpDown(cv::Mat& image) {

    // Convert image to grayscale
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // Load face cascade
    cv::CascadeClassifier faceCascade;
    faceCascade.load("./haarcascade_frontalface_default.xml");

    // Detect faces
    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(gray, faces, 1.1, 3, 0, cv::Size(30, 30));

    // get the index of the largest face
    int largestFaceIndex = 0;
    int largestFaceArea = 0;
    for (int i = 0; i < faces.size(); i++) {
        int area = faces[i].width * faces[i].height;
        if (area > largestFaceArea) {
            largestFaceArea = area;
            largestFaceIndex = i;
        }
    }


    int nOverSaturatedPixels = 0;
    int nUnderSaturatedPixels = 0;
    int nPixels = gray.rows * gray.cols;
    for (int i = 0; i < gray.rows; i++) {
        for (int j = 0; j < gray.cols; j++) {
            // get the gray scale pixel value
            uchar pixel = gray.at<uchar>(i, j);
                            
            if (pixel == 0) {
                nUnderSaturatedPixels++;
            }
            else if (pixel == 255) {
                nOverSaturatedPixels++;
            }
        }
    }
        
    double pctOverSaturated = (double)nOverSaturatedPixels / (double)nPixels;
    double pctUnderSaturated = (double)nUnderSaturatedPixels / (double)nPixels;

    int exposureChangeSuggestion = 0;
    std::string exposureChangeText = "No change";
    if ((pctOverSaturated > pctUnderSaturated) && (pctOverSaturated > saturationLimit)) {
        exposureChangeSuggestion = -1; // suggest decreasing exposure
        exposureChangeText = "Decrease";
    }
    else if ((pctUnderSaturated > pctOverSaturated) && (pctUnderSaturated > saturationLimit)) {
        exposureChangeSuggestion = 1; // suggest increasing exposure
        exposureChangeText = "Increase";
    }
    
    
    // Draw the ROI rect for the detected faces
    for (int i = 0; i < faces.size(); i++) {
        cv::Rect face = faces[i];
        cv::Scalar roiColor = (i == largestFaceIndex) ? PRIMARY_ROI_COLOR : SECONDARY_ROI_COLOR;
        cv::rectangle(image, face, roiColor, 2, cv::LINE_AA);
        // draw the exposure change text in lower left corner of the ROI
        if (i == largestFaceIndex) {            
            cv::putText(image, exposureChangeText, cv::Point(face.x+5, face.y + face.height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, TEXTCOLOR, 1, cv::LINE_AA);
        }
    }
    
    return exposureChangeSuggestion;
}
