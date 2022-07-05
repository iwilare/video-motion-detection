#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include <ff/ff.hpp>

#include "lib/main.cpp"
#include "lib/video_detection.cpp"

using namespace cv;
using namespace std;
using namespace ff;

struct Emitter : ff_monode_t<Mat> {
    VideoCapture video;
    Emitter(VideoCapture&& video) : video(video) {}
    Mat* svc(Mat*) {
        Mat frame;
        for(video >> frame; !frame.empty(); video >> frame)
            ff_send_out(new Mat(move(frame)));
        return EOS;
    }
};

struct Worker : ff_node_t<Mat,void> {
    /*
        Global information
    */
    // Shared structures, by reference
    atomic<size_t>* global_motion_frames;
    // Copied information: we keep a local copy of the background frame
    vector<float> background_blur_grey;
    float difference_threshold;
    float detection_percentage;

    /*
        Local information
    */
    size_t local_motion_frames;

    // Base constructor with all global information required to compute the motion frame
    Worker(atomic<size_t>* global_motion_frames,
           vector<float> background_blur_grey,
           float difference_threshold,
           float detection_percentage) :
               global_motion_frames(global_motion_frames), background_blur_grey(background_blur_grey), difference_threshold(difference_threshold), detection_percentage(detection_percentage), local_motion_frames(0) {}

    void* svc(Mat* frame) {
        if(is_motion_frame(background_blur_grey, *frame, difference_threshold, detection_percentage))
            local_motion_frames++;
        delete frame;
        return GO_ON;
    }

    void svc_end() {
        // Small and unlikely optimization that might save a lock operation
        if(local_motion_frames > 0)
            // Update the global atomic motion frames counter
            global_motion_frames->fetch_add(local_motion_frames);
    }
};

struct VideoDetectionFastFlow : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 vector<float> background_blur_grey,
                 float detection_percentage,
                 float difference_threshold,
                 size_t n_workers,
                 bool set_thread_affinity) {
        atomic<size_t> total_motion_frames;

        // Reserve one worker for the emitter
        size_t farm_workers = n_workers <= 1 ? 1 : n_workers - 1;

        vector<unique_ptr<ff_node>> workers(farm_workers);

        for(auto& w : workers)
            w = make_unique<Worker>(&total_motion_frames, background_blur_grey, difference_threshold, detection_percentage);

        ff_Farm<void, void> motion_detection_farm(
            move(workers),
            make_unique<Emitter>(move(video)),
            nullptr); // No collector is required

        motion_detection_farm.blocking_mode();

        if(motion_detection_farm.run_and_wait_end() < 0) {
            cerr << "Error running FastFlow farm" << endl;
            exit(1);
        }

        return total_motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionFastFlow().main(argn, argv);
}
