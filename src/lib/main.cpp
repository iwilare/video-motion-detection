#include <string>
#include <fstream>
#include <algorithm>
#include <thread>
#include <opencv2/opencv.hpp>

#include "args.cpp"
#include "utimer.cpp"
#include "video_detection.cpp"

using namespace cv;
using namespace std;

#define DEFAULT_DETECTION_PERCENTAGE 0.05
#define DEFAULT_DIFFERENCE_THRESHOLD 0.05

/*
    Thin wrapper class to provide a uniform interface for
    input, error management, and benchmarking with a
    standardized global program timer
*/
struct VideoDetectionMain {
    // Virtual method to encapsulate the main logic
    virtual size_t logic(VideoCapture video,
                         vector<float> background_blur_grey,
                         float detection_percentage,
                         float difference_threshold,
                         size_t n_workers) = 0;

    int main(int argn, char** argv) {
        ios_base::sync_with_stdio(false);

        // Default values for arguments
        string filename;
        float detection_percentage = DEFAULT_DETECTION_PERCENTAGE;
        float difference_threshold = DEFAULT_DIFFERENCE_THRESHOLD;
        size_t n_workers = thread::hardware_concurrency();
        size_t benchmark_iterations = 1;

        // Read arguments from command line
        get_arguments(argn, argv, &filename, &difference_threshold, &detection_percentage, &n_workers, &benchmark_iterations);

        std::chrono::microseconds total_time(0);

        size_t frames;
        size_t total_frames;
        for(size_t i = 0; i < benchmark_iterations; i++)
        {
            /*
                Start the global timer for the entire program.
                Notice how both the initialization of the VideoCapture and the processing
                of the first frame are *included* in the total program time, as it is
                a crucial part of the serial fraction of the program, and can hardly be parallelized.
            */
            cumulative_utimer total_timer(&total_time);

            // Initialize the input stream for the frames of the videos
            VideoCapture video(filename);
            if (!video.isOpened()) {
                cout << "Error opening video stream or file" << endl;
                exit(1);
            }

            // Get the first background frame
            Mat background;
            video >> background;

            // Pre-process the background frame
            vector<float> background_blur_grey = preprocess_blur_greyscale(background);

            // Start the main logic
            total_frames = video.get(cv::CAP_PROP_FRAME_COUNT);
            frames = logic(video, background_blur_grey, detection_percentage, difference_threshold, n_workers);

            total_timer.stop();
        }
        if(benchmark_iterations == 1) {
            cout << "Motion frames over total video frames: (" << frames << " / " << total_frames << ")" << endl;
            cout << "Total microseconds: " << total_time.count() << endl;
        } else {
            // Benchmark mode, simply write the averaged out timing
            cout << (total_time / benchmark_iterations).count() << endl;
        }
        return 0;
    }
};
