#include <iostream>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cstdint>

#include <opencv2/opencv.hpp>

#include "timer.h"
#include "exposure-calculator.h"
#include "colors.h"



#define DEFAULT_SETTINGS_FILE "./settings.yaml"
#define APP_NAME "exposure-control"
#define VERSION "0.0.1"



int main(int argc, char** argv) {
    
    // print version number and github link
    std::cout << APP_NAME << " v" << VERSION << std::endl;
    std::cout << "https://github.com/pewalh/exposure-control" << std::endl << std::endl;


    Timer timer;
    timer.start();


    // Parse command line arguments
    std::string settingsFile = DEFAULT_SETTINGS_FILE;
    bool showWindow = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
			std::cout << "Options:" << std::endl;
			std::cout << "  -h, --help                  Show this help message and exit" << std::endl;
			std::cout << "  -s, --settings <filename>   Specify path to settings file (default: \"" << DEFAULT_SETTINGS_FILE << "\")" << std::endl;
            std::cout << "  -n, --no-window             Disable window with processing info" << std::endl;         
			return 0;
		}
		else if (arg == "-s" || arg == "--settings") {
			if (i + 1 < argc) {
				std::string filename = argv[++i];
				std::cout << "Settings file: " << filename << std::endl;
				if (!std::filesystem::exists(filename)) {
					std::cerr << "Error: Settings file does not exist" << std::endl;
					return -1;
				}
			}
			else {
				std::cerr << "Error: Settings file must be specified together with -s/--settings" << std::endl;
				return -1;
			}
		}
        else if (arg == "-n" || arg == "--no-window") {
			showWindow = false;
		}
        
		else {
			std::cerr << "Error: Unknown argument '" << arg << "'" << std::endl;
			return -1;
		}
	}



    // Create a VideoCapture object to access the webcam
    cv::VideoCapture cap(0);

    // Check if the webcam is opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not connect to camera" << std::endl;
        return -1;
    }

    // Read yaml file with default camera settings

    double exposure;
    double gain;
    double contrast;
    double sharpness;
    double frameTimeMs;
    cv::FileStorage fs;
    try {
        cv::FileStorage fs = cv::FileStorage("./settings.yaml", cv::FileStorage::READ);
    	if (!fs.isOpened()) {
        		std::cerr << "Error: Could not open settings.yaml" << std::endl;
        		return -1;
   		}
    	fs["exposure"] >> exposure;
    	fs["gain"] >> gain;
    	fs["contrast"] >> contrast;
        fs["sharpness"] >> sharpness;
        fs["frameTimeMs"] >> frameTimeMs;
    	fs.release();
    }
    catch (const std::exception& e) {
    	std::cerr << "Error while reading settings.yaml: " << e.what() << std::endl; 
    }
    
    double defaultGain = gain;
    // Set default camera settings
    cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0); // Disable auto exposure
    cap.set(cv::CAP_PROP_EXPOSURE, exposure); // Set exposure exponent, e.g. -4 => 2^-4 = 1/16 seconds
    cap.set(cv::CAP_PROP_GAIN, gain);
    cap.set(cv::CAP_PROP_CONTRAST, contrast);
    cap.set(cv::CAP_PROP_SHARPNESS, sharpness);



    cv::Mat frame;
    char key = -1;




    cap >> frame;
    // If the frame is empty, break the loop
    if (frame.empty())
    {
        std::cerr << "Error: Could not capture fram from camera" << std::endl;
        cap.release();
        return -1;
    }

    if (showWindow) {
		// Create a window to display the frame
		cv::namedWindow(APP_NAME, cv::WINDOW_AUTOSIZE);
	}

    ExposureCalculator exposureCalculator;

    // starting time
    uint64_t time_ms = timer.getElapsedMilli();
    
    int returnCode = 0;
    while (true) {
    
        // Break the loop if the window has been closed
        if (showWindow && (cv::getWindowProperty(APP_NAME, cv::WND_PROP_VISIBLE) < 1)) {
            break;
        }


        // Capture frame-by-frame
        cap >> frame;

        // If the frame is empty, break the loop
        if (frame.empty()) {
            std::cerr << "Error: Could not capture fram from camera" << std::endl;
            returnCode = -1;
            break;
        }

        // mirror the frame
        cv::flip(frame, frame, 1);


        int exposureChangeSuggestion;
        int gainChangeSuggestion;
        exposureCalculator.getExposureUpDown(frame, exposureChangeSuggestion, gainChangeSuggestion);
        // print the current exposure in the top left corner
        cv::putText(frame, "Exposure: " + std::to_string((int)exposure), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, TEXTCOLOR, 1, cv::LINE_AA);
        cv::putText(frame, "Gain: " + std::to_string((int)gain), cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, TEXTCOLOR, 1, cv::LINE_AA);

        // Display the resulting frame mirrorred
        if (showWindow) {
            
            cv::imshow(APP_NAME, frame);
            key = cv::waitKey(5); // allow window to redraw
        }
        

        bool setGain = false;
        bool setExposure = false;

        

        // adjust exposure and gain
        if (exposureChangeSuggestion != 0) {
            exposure += exposureChangeSuggestion;
            if (exposure < -11) {
                exposure = -11;
            }
            else if (exposure > -2) {
                exposure = -2;
            }
            else {
                setExposure = true;
            }
        }

        if (gainChangeSuggestion != 0) {
            gain += gainChangeSuggestion;
            if (gain < 0) {
                gain = 0;
            }
            else if (gain > 255) {
                gain = 255;
            }
            else {
                setGain = true;
            }
        }
        

        // balance exposure and gain
        if (gain > 226 && exposure < -2) {
            exposure++;
            gain = defaultGain;
            setExposure = true;
            setGain = true;
        }
        else if (gain < 30 && exposure > -11) {
            exposure--;
            gain = defaultGain;
            setExposure = true;
            setGain = true;
        }


        if (setExposure) {
            cap.set(cv::CAP_PROP_EXPOSURE, exposure);
        }
        if (setGain) {
            cap.set(cv::CAP_PROP_GAIN, gain);
        }

        // now just loop frames until frameTimeMs milliseconds has passed since t0, without changing exposure
        uint64_t t0 = time_ms;
        while (true) {
            
            // Break the loop if the window has been closed
            if (showWindow && (cv::getWindowProperty(APP_NAME, cv::WND_PROP_VISIBLE) < 1)) {
                break;
            }
            cap >> frame;
            cv::flip(frame, frame, 1);
            int _exp, _gain;
            exposureCalculator.getExposureUpDown(frame, _exp, _gain);
            cv::putText(frame, "Exposure: " + std::to_string((int)exposure), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, TEXTCOLOR, 1, cv::LINE_AA);
            cv::putText(frame, "Gain: " + std::to_string((int)gain), cv::Point(10, 40), cv::FONT_HERSHEY_SIMPLEX, 0.5, TEXTCOLOR, 1, cv::LINE_AA);
            if (showWindow) {
                cv::imshow(APP_NAME, frame);
            }

            cv::waitKey(5);
            // sleep for 50 ms
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            time_ms = timer.getElapsedMilli();
            if ((time_ms - t0) >= frameTimeMs)
                break;
            
        }


    }

    // When everything done, release the video capture object and close all windows
    cap.release();
    cv::destroyAllWindows();

    return returnCode;
}
