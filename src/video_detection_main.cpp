#include <string>
#include <fstream>
#include <algorithm>
#include <thread>
#include <opencv2/opencv.hpp>

#include "args.cpp"
#include "utimer.cpp"

using namespace cv;
using namespace std;

struct VideoDetectionMain {
    virtual size_t logic(VideoCapture video,
                         float detection_percentage,
                         float difference_threshold,
                         size_t n_workers,
                         bool set_thread_affinity) = 0;
    int main(int argn, char** argv) {
        ios_base::sync_with_stdio(false);

        string filename;
        float detection_percentage = 0.05;
        float difference_threshold = 0.3;
        size_t n_workers = thread::hardware_concurrency();
        bool set_thread_affinity = false;

        get_arguments(argn, argv, &filename, &difference_threshold, &detection_percentage, &n_workers, &set_thread_affinity);

        int64_t total_time;

        {
            utimer total_timer(&total_time);
            VideoCapture video(filename);

            if (!video.isOpened()) {
                cout << "Error opening video stream or file" << endl;
                exit(1);
            }

            cout << "Number of frames with motion: " << logic(video, detection_percentage, difference_threshold, n_workers, set_thread_affinity) << endl;
        }

        cout << total_time << endl; //read_time << computation_time << endl;

        return 0;
    }
};
