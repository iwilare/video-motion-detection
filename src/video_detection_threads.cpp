#include <string>
#include <fstream>
#include <algorithm>
#include <atomic>
#include <opencv2/opencv.hpp>

#include "shared_queue.cpp"

#include "video_detection_main.cpp"
#include "video_detection.cpp"
#include "cpu_affinity.cpp"

using namespace cv;
using namespace std;

struct VideoDetectionThreads : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 float detection_percentage,
                 float difference_threshold,
                 size_t n_workers,
                 bool set_thread_affinity) {
        // Get the first background frame
        Mat background;
        video >> background;

        // Pre-process the background frame
        vector<float> background_blur_grey = blur_grey(&background);
        size_t total_pixels = background_blur_grey.size();

        atomic<size_t> motion_frames(0);
        shared_queue<Mat> queue;

        size_t thread_workers = n_workers < 1 ? 1 : n_workers - 1;

        auto worker = [&]() {
            while(true) {
                auto frame_opt = queue.pop();
                if(frame_opt) {
                    auto frame = frame_opt.value();
                    auto changed = is_motion_frame(background_blur_grey, total_pixels, &frame, difference_threshold, detection_percentage);
                    cout << (changed ? "X" : "_");
                    motion_frames += changed ? 1 : 0;
                } else
                    return;
            }
        };

        vector<thread> workers(thread_workers);

        for(size_t i = 0; i < thread_workers; i++) {
            workers[i] = thread(worker);
            if(set_thread_affinity)
                pin_thread_to_cpu(i, workers[i]);
        }

        for(size_t i = 0; i < thread_workers; i++)
            workers[i].join();

        return motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionThreads().main(argn, argv);
}
