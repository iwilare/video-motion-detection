#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include <ff/ff.hpp>

#include "video_detection_main.cpp"
#include "video_detection.cpp"
#include "cpu_affinity.cpp"

using namespace cv;
using namespace std;
using namespace ff;

struct Emitter : ff_node_t<Mat> {
    VideoCapture video;
    Emitter(VideoCapture& video) : video(video) {}
    Mat* svc(Mat*) {
        Mat frame;
        video >> frame;
        return frame.empty() ? EOS : new Mat(frame);
    }
};

struct Collector : ff_node_t<bool> {
    size_t *motion_frames;
    size_t *total_frames;
    Collector(size_t *motion_frames, size_t *total_frames) : motion_frames(motion_frames), total_frames(total_frames) {}
    bool* svc(bool* is_motion_frame) {
        *motion_frames += *is_motion_frame;
        *total_frames += 1;
        cout << (is_motion_frame ? "X" : "_");
        delete is_motion_frame;
        return GO_ON;
    }
};

struct VideoDetectionFastFlow : VideoDetectionMain {
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

        auto worker = [&](Mat* frame, const ff_node* f) -> bool* {
            return new bool(is_motion_frame(background_blur_grey, total_pixels, frame, difference_threshold, detection_percentage));
        };

        size_t farm_workers = n_workers < 1 ? 1 : n_workers - 1;

        vector<unique_ptr<ff_node>> workers(farm_workers);

        for(auto& w : workers)
            w = make_unique<ff_node_F<Mat,bool>>(worker);

        size_t total_frames, motion_frames;
        ff_Farm<void, void> motion_detection_farm(
            move(workers),
            make_unique<Collector>(&total_frames, &motion_frames),
            make_unique<Emitter>(video));

        motion_detection_farm.set_scheduling_ondemand();

        if(motion_detection_farm.run_and_wait_end() < 0) {
            cerr << "Error running FastFlow farm" << endl;
            exit(1);
        }

        return motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionFastFlow().main(argn, argv);
}
