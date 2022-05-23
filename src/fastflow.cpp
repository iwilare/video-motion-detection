#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include <ff/ff.hpp>

#include "video_detection.cpp"
#include "utimer.cpp"
#include "args.cpp"
#include "cpu_affinity.cpp"

using namespace cv;
using namespace std;

struct Emitter : ff::ff_node_t<void,Mat> {
    VideoCapture video;
    Emitter(VideoCapture& video) : video(move(video)) {}
    Mat* svc(void*) {
        Mat frame;
        video >> frame;
        return frame.empty() ? EOS : new Mat(frame);
    }
};

struct Collector : ff::ff_node_t<bool,void> {
    size_t *motion_frames;
    size_t *total_frames;
    Collector(size_t *motion_frames, size_t *total_frames) : motion_frames(motion_frames), total_frames(total_frames) {}
    void* svc(bool is_motion_frame) {
        *motion_frames += is_motion_frame;
        *total_frames += 1;
        cout << (is_motion_frame ? "X" : "_");
        return GO_ON;
    }
};

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

        auto worker = [&](auto frame) {
            return new bool(is_motion_frame(background_blur_grey, total_pixels, frame, difference_threshold, detection_percentage));
        };

        size_t farm_workers = max(1, (int)workers-1);
        vector<unique_ptr<ff::ff_node>> workers(farm_workers, make_unique<ff::ff_node_F<Mat,bool>>(worker));

        size_t total_frames, motion_frames;
        ff::ff_Farm<void,void> motion_detection_farm(
            move(workers),
            make_unique<Emitter>(video),
            make_unique<Collector>(&total_frames, &motion_frames)
        );

        motion_detection_farm.set_scheduling_ondemand();

        if(motion_detection_farm.run_and_wait_end()) {
            cerr << "Error running farm" << endl;
            exit(1);
        }

        cout << "Total frames: " << total_frames << endl;
        cout << "Number of frames with motion: " << motion_frames << endl;
    }

    cout << total_time << endl; //read_time << computation_time << endl;

    return 0;
}
