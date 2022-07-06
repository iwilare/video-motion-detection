#include <string>
#include <fstream>
#include <algorithm>
#include <atomic>
#include <opencv2/opencv.hpp>

#include "lib/main.cpp"
#include "lib/video_detection.cpp"
#include "lib/shared_queue.cpp"
#include "lib/cpu_affinity.cpp"

using namespace cv;
using namespace std;

struct VideoDetectionThreads : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 vector<float> background_blur_grey,
                 float detection_percentage,
                 float difference_threshold,
                 size_t n_workers) {
        atomic<size_t> total_motion_frames(0);
        shared_queue<Mat> queue;

        /*
            Create the workers by *copying* global
            information including the background frame,
            except the shared structures (i.e., the global atomic
            counter and the shared queue)
        */
        auto worker = [=, &queue, &total_motion_frames]() {
            size_t local_motion_frames = 0;
            while(true) {
                optional<Mat> frame_opt = queue.pop();
                if(frame_opt) {
                    if(is_motion_frame(background_blur_grey, frame_opt.value(), difference_threshold, detection_percentage))
                        local_motion_frames++;
                } else {
                    // Unlikely optimization that might save a lock operation
                    if(local_motion_frames > 0)
                        // Update the global atomic motion frames counter
                        total_motion_frames.fetch_add(local_motion_frames);
                    break;
                }
            }
        };

        size_t thread_workers = n_workers <= 1 ? 1 : n_workers - 1;

        vector<thread> workers(thread_workers);

        for(size_t i = 0; i < thread_workers; i++) {
            workers[i] = thread(worker);
#ifdef THREAD_AFFINITY
            pin_thread_to_cpu(i, workers[i]);
#endif
        }

        {
            Mat frame;
            for(video >> frame; !frame.empty(); video >> frame) {
                queue.push(move(frame));
            }
            queue.done();

            for(size_t i = 0; i < thread_workers; i++)
                workers[i].join();
        }

        return total_motion_frames;
    }
};

int main(int argn, char** argv) {
    return VideoDetectionThreads().main(argn, argv);
}
