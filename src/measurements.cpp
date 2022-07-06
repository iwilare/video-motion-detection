#include <string>
#include <fstream>
#include <algorithm>
#include <atomic>
#include <opencv2/opencv.hpp>

#include "lib/main.cpp"
#include "lib/video_detection.cpp"

using namespace cv;
using namespace std;

int main(int argn, char** argv) {
    ios_base::sync_with_stdio(false);

    // Default values for arguments
    string filename;
    float detection_percentage = DEFAULT_DETECTION_PERCENTAGE;
    float difference_threshold = DEFAULT_DIFFERENCE_THRESHOLD;
    size_t n_workers = thread::hardware_concurrency();
    size_t benchmarks = 0;

    // Read arguments from command line
    get_arguments(argn, argv, &filename, &difference_threshold, &detection_percentage, &n_workers, &benchmarks);

    std::chrono::microseconds total_open_time(0);
    std::chrono::microseconds total_init_time(0);
    std::chrono::microseconds total_background_read_time(0);
    std::chrono::microseconds total_background_greyscale_time(0);
    std::chrono::microseconds total_background_blur_time(0);
    std::chrono::microseconds total_preprocess_time(0);
    std::chrono::microseconds total_read_time(0);
    std::chrono::microseconds total_greyscale_time(0);
    std::chrono::microseconds total_blur_time(0);
    std::chrono::microseconds total_detection_time(0);

    size_t motion_frames = 0;
    size_t iterations = n_workers; // Use the number of threads as the number of iterations
    size_t total_frames = -1; // Initialize at each iteration

    for(size_t i = 0; i < iterations; i++) {
        //{
        cumulative_manual_utimer init_time(&total_init_time);
        //{
        cumulative_manual_utimer open_time(&total_open_time);
        VideoCapture video(filename);
        if (!video.isOpened()) {
            cout << "Error opening video stream or file" << endl;
            exit(1);
        }
        open_time.stop();
        //}

        //{
        cumulative_manual_utimer background_read_time(&total_background_read_time);
        Mat background;
        video >> background;
        if(background.empty())
            break;
        background_read_time.stop();
        //}

        //{
        cumulative_manual_utimer preprocess_time(&total_preprocess_time);
        //{
        cumulative_manual_utimer background_greyscale_time(&total_background_greyscale_time);
        auto g = greyscale(background);
        background_greyscale_time.stop();
        //}
        //{
        cumulative_manual_utimer background_blur_time(&total_background_blur_time);
        auto background_blur_grey = blur(AverageKernel, g);
        background_blur_time.stop();
        //}
        preprocess_time.stop();
        //}
        init_time.stop();
        //}

        total_frames = video.get(cv::CAP_PROP_FRAME_COUNT);
        motion_frames = 0;
        video.set(cv::CAP_PROP_POS_FRAMES, 0);

        while(true) {
            //{
            cumulative_manual_utimer read_time(&total_read_time);
            Mat frame;
            video >> frame;
            if(frame.empty())
                break;
            read_time.stop();
            //}

            //{
            cumulative_manual_utimer greyscale_time(&total_greyscale_time);
            auto g = greyscale(frame);
            greyscale_time.stop();
            //}

            //{
            cumulative_manual_utimer blur_time(&total_blur_time);
            auto b = blur(AverageKernel, g);
            blur_time.stop();
            //}

            //{
            cumulative_manual_utimer detection_time(&total_detection_time);
            auto d = is_motion_processed_frame(background_blur_grey, b, difference_threshold, detection_percentage);
            detection_time.stop();
            //}

            if(d)
                motion_frames++;
        }

        std::cout << i + 1 << " / " << iterations << ", = " << motion_frames << std::endl;
    }

    auto counts = total_frames * iterations;

    auto processing_time = total_greyscale_time + total_blur_time + total_detection_time;
    auto total_one_frame = total_read_time + processing_time;
    auto total_time = total_open_time + total_preprocess_time + total_one_frame * total_frames;

    std::cout << "Avg 1 frame read time:       " << (total_read_time / counts).count() << std::endl;
    std::cout << "Avg 1 frame greyscale time:  " << (total_greyscale_time / counts).count() << std::endl;
    std::cout << "Avg 1 frame blur time:       " << (total_blur_time / counts).count() << std::endl;
    std::cout << "Avg 1 frame detection time:  " << (total_detection_time / counts).count() << std::endl;
    std::cout << "Avg 1 frame processing time: " << (processing_time / counts).count() << std::endl;
    std::cout << "Avg 1 frame total time:      " << (total_one_frame / counts).count() << std::endl;
    std::cout << "Avg init time:               " << (total_init_time / iterations).count() << std::endl;
    std::cout << "Avg open time:               " << (total_open_time / iterations).count() << std::endl;
    std::cout << "Avg background read time:    " << (total_background_read_time / iterations).count() << std::endl;
    std::cout << "Avg background process time: " << (total_preprocess_time / iterations).count() << std::endl;
    //std::cout << "Avg background blur time:    " << (total_background_blur_time / iterations).count() << std::endl;
    //std::cout << "Avg background grey time:    " << (total_background_greyscale_time / iterations).count() << std::endl;
    std::cout << "Avg total time:              " << (total_time / iterations).count() << std::endl;
}
