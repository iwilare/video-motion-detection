#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "lib/main.cpp"
#include "lib/video_detection.cpp"

using namespace cv;
using namespace std;

struct VideoDetectionOmp : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 vector<float> background_blur_grey,
                 float detection_percentage,
                 float difference_threshold,
                 size_t n_workers) {
        size_t motion_frames = 0;

        #pragma omp parallel num_threads(n_workers)
        #pragma omp single // Only the emitter reads frames
        while(true) {
            Mat frame; // Ensure that frame is directly in the scope of the task
            video >> frame;
            if(frame.empty())
                break;

            // Copy the frame locally
            #pragma omp task firstprivate(frame)
            if(is_motion_frame(background_blur_grey, frame, difference_threshold, detection_percentage))
                #pragma omp atomic
                motion_frames++;
        }

        return motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionOmp().main(argn, argv);
}
