#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "video_detection.cpp"
#include "utimer.cpp"
#include "args.cpp"
#include "cpu_affinity.cpp"

using namespace cv;
using namespace std;

int main(int argn, char** argv) {
    ios_base::sync_with_stdio(false);

    string filename;
    float detection_percentage = 0.05;
    size_t workers = thread::hardware_concurrency();
    bool set_thread_affinity = false;
    float difference_threshold = 0.3;

    check_arguments(argn, argv, &filename, &difference_threshold, &detection_percentage, &workers, &set_thread_affinity);

    int64_t total_time, read_time, computation_time;

    {
        utimer total_timer(&total_time);
        VideoCapture video(filename);

        if (!video.isOpened()) {
            cout << "Error opening video stream or file" << endl;
            exit(1);
        }

        // Get the first background frame
        Mat background;
        video >> background;

        // Pre-process the background frame
        vector<float> background_blur_grey = blur_grey(background);
        size_t total_pixels = background_blur_grey.size();

        size_t motion_frames = 0;
        size_t total_frames = 0;

        Mat frame;
        for(video >> frame; !frame.empty(); video >> frame) {
            auto changed = is_motion_frame(background_blur_grey, total_pixels, frame, difference_threshold, detection_percentage);
            motion_frames += changed;
            total_frames += 1;
            cout << (changed ? "X" : "_");
        }

        cout << "Total frames: " << total_frames << endl;
        cout << "Number of frames with motion: " << motion_frames << endl;
    }

    cout << total_time << endl; //read_time << computation_time << endl;

    return 0;
}
