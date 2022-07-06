#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "lib/main.cpp"
#include "lib/video_detection.cpp"

using namespace cv;
using namespace std;

struct VideoDetectionSequential : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 vector<float> background_blur_grey,
                 float detection_percentage,
                 float difference_threshold,
                 size_t workers) {
        size_t motion_frames = 0;

        Mat frame;
        for(video >> frame; !frame.empty(); video >> frame)
            if(is_motion_frame(background_blur_grey, frame, difference_threshold, detection_percentage))
                motion_frames++;

        return motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionSequential().main(argn, argv);
}
