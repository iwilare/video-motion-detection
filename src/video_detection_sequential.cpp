#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "video_detection_main.cpp"
#include "video_detection.cpp"
#include "cpu_affinity.cpp"

using namespace cv;
using namespace std;

struct VideoDetectionSequential : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 float detection_percentage,
                 float difference_threshold,
                 size_t workers,
                 bool set_thread_affinity) {
        // Get the first background frame
        Mat background;
        video >> background;

        // Pre-process the background frame
        vector<float> background_blur_grey = blur_grey(&background);
        size_t total_pixels = background_blur_grey.size();

        size_t motion_frames = 0;
        Mat frame;
        for(video >> frame; !frame.empty(); video >> frame) {
            auto changed = is_motion_frame(background_blur_grey, total_pixels, &frame, difference_threshold, detection_percentage);
            motion_frames += changed;
            cout << (changed ? "X" : "_");
        }

        return motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionSequential().main(argn, argv);
}
