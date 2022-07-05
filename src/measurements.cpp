#include <string>
#include <fstream>
#include <algorithm>
#include <atomic>
#include <opencv2/opencv.hpp>

#include "lib/main.cpp"
#include "lib/video_detection.cpp"

#define N_ITERATIONS 3

using namespace cv;
using namespace std;

struct VideoDetectionMetrics : VideoDetectionMain {
    size_t logic(VideoCapture video,
                 vector<float> background_blur_grey,
                 float detection_percentage,
                 float difference_threshold,
                 size_t workers,
                 bool set_thread_affinity) {
        std::chrono::microseconds total_read_time(0);
        std::chrono::microseconds total_greyscale_time(0);
        std::chrono::microseconds total_blur_time(0);
        std::chrono::microseconds total_detection_time(0);

        size_t motion_frames = 0;
        size_t iterations = N_ITERATIONS;
        size_t total_frames = video.get(cv::CAP_PROP_FRAME_COUNT);

        for(size_t i = 0; i < iterations; i++) {

            motion_frames = 0;

            // Reset the video at the beginning
            video.set(cv::CAP_PROP_POS_FRAMES, 0);

            while(true) {
                //{
                cumulative_utimer read_time(&total_read_time);
                Mat frame;
                video >> frame;
                if(frame.empty())
                    break;
                read_time.stop();
                //}

                //{
                cumulative_utimer greyscale_time(&total_greyscale_time);
                auto g = greyscale(frame);
                greyscale_time.stop();
                //}

                //{
                cumulative_utimer blur_time(&total_blur_time);
                auto b = blur(AverageKernel, g);
                blur_time.stop();
                //}

                //{
                cumulative_utimer detection_time(&total_detection_time);
                auto d = is_motion_processed_frame(background_blur_grey, b, difference_threshold, detection_percentage);
                detection_time.stop();
                //}

                if(d)
                    motion_frames++;
            }

            std::cout << i + 1 << " / " << iterations << ", = " << motion_frames << std::endl;
        }

        auto counts = total_frames * iterations;

        std::cout << "Avg read time:" << (total_read_time / counts).count() << std::endl;
        std::cout << "Avg greyscale time:" << (total_greyscale_time / counts).count() << std::endl;
        std::cout << "Avg blur time:" << (total_blur_time / counts).count() << std::endl;
        std::cout << "Avg detection time:" << (total_detection_time / counts).count() << std::endl;

        auto processing_time = total_greyscale_time + total_blur_time + total_detection_time;
        auto total_time = total_read_time + processing_time;

        std::cout << "Processing time:" << (processing_time / counts).count() << std::endl;
        std::cout << "Total time:" << (total_time / counts).count() << std::endl;

        return motion_frames; // Output of the last iteration, as sanity check
    }
};

int main(int argn, char** argv) {
    return VideoDetectionMetrics().main(argn, argv);
}
