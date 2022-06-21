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
        for(video >> frame; !frame.empty(); video >> frame)
            ff_send_out(new Mat(move(frame)));
        return EOS;
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

        atomic<size_t> motion_frames;
        auto worker = [&](Mat* frame, ff_node* self) -> void* {
            bool changed =  is_motion_frame(background_blur_grey, total_pixels, frame, difference_threshold, detection_percentage);
            motion_frames += changed;
            cout << (changed ? "X" : "_");
            delete frame;
            return self->GO_ON;
        };

        size_t farm_workers = n_workers <= 1 ? 1 : n_workers - 1;

        vector<unique_ptr<ff_node>> workers(farm_workers);

        for(auto& w : workers)
            w = make_unique<ff_node_F<Mat,void>>(worker);

        ff_Farm<void, void> motion_detection_farm(move(workers), make_unique<Emitter>(video), nullptr);

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
