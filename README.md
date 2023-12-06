# exposure-control
Optimize exposure on the face for webcams that can be controlled by OpenCV.

The app tries to find a face using OpenCV and then adjusts the exposure and gain to 
optimize visibility of the face. If several faces are found, the one with the largest
area is used.


## Setup
`exposure-control` uses OpenCV to find faces and control the camera. To use it, you need to install OpenCV and set the environment variable `OPENCV_DIR` to the installation directory.
1. Download and install OpenCV libraries ([opencv doc](https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html))
2. Set the environment variable `OPENCV_DIR` (all caps) and add the bin folder to `PATH` ([opencv doc](https://docs.opencv.org/4.x/d3/d52/tutorial_windows_install.html#tutorial_windows_install_path))
3. Download and set haar cascade file
    * Download xml file from [here](https://github.com/opencv/opencv/tree/master/data/haarcascades)
    * Set `haarCascadeParamFile` in `settings.yaml` appropriately

