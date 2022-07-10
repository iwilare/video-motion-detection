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
    string filename("default.mp4");
    float detection_percentage = DEFAULT_DETECTION_PERCENTAGE;
    float difference_threshold = DEFAULT_DIFFERENCE_THRESHOLD;
    size_t iterations = 1;

    // SPECIAL CONDITIONS TO ENABLE SPECIFIC MEASUREMENT BEHAVIOUR
    bool PRINT_READ_TIMES = false;
    bool GREY_AND_BLUR = true;

    // Read arguments from command line
    int i = 1;
    if(i < argn) filename = argv[i++];
    if(i < argn) iterations = atoi(argv[i++]);
    if(i < argn) PRINT_READ_TIMES = atoi(argv[i++]);
    if(i < argn) GREY_AND_BLUR = atoi(argv[i++]);

    std::chrono::microseconds total_time(0);
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
    size_t total_frames = 0; // Initialize at each iteration

    //{
    cumulative_manual_utimer total_timer(&total_time);

    if(PRINT_READ_TIMES) {
        iterations = 1;
        cout << "time" << endl; // For the .csv file
    }

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
        auto background_blur_grey = blur(KERNEL, g);
        background_blur_time.stop();
        //}
        preprocess_time.stop();
        //}
        init_time.stop();
        //}

        motion_frames = 0;
        total_frames = 0; // The last iteration gives it the correct value

        while(true) {

            std::chrono::microseconds read_time(0);
            //{
            cumulative_manual_utimer read_timer(&read_time);
            Mat frame;
            video >> frame;
            read_timer.stop();
            //}
            total_read_time += read_time;
            if(PRINT_READ_TIMES) {
                cout << read_time.count() << endl;
            }

            if(frame.empty()) {
                break;
            }

            if(GREY_AND_BLUR) {
            //{
            cumulative_manual_utimer greyscale_time(&total_greyscale_time);
            auto g = greyscale(frame);
            greyscale_time.stop();
            //}

            //{
            cumulative_manual_utimer blur_time(&total_blur_time);
            auto b = blur(KERNEL, g);
            blur_time.stop();
            //}

            //{
            cumulative_manual_utimer detection_time(&total_detection_time);
            auto d = is_motion_processed_frame(background_blur_grey, b, difference_threshold, detection_percentage);
            detection_time.stop();
            //}

            if(d)
                motion_frames++;
            total_frames++;
            }
        }
    }
    total_timer.stop();
    // }

    auto counts = total_frames * iterations;

    auto processing_time = total_greyscale_time + total_blur_time + total_detection_time;
    auto total_one_frame = total_read_time + processing_time;

    if(!PRINT_READ_TIMES) {
    std::cout << "Filename:                    " << filename << std::endl;
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
    std::cout << "Avg total time:              " << (total_time / iterations).count() << std::endl;
    }
}
