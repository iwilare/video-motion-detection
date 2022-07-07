#include <string>
#include <fstream>
#include <thread>
#include <algorithm>
#include <numeric>
#include <opencv2/opencv.hpp>

#include "args.cpp"
#include "utimer.cpp"
#include "video_detection.cpp"

using namespace cv;
using namespace std;

#define DEFAULT_DETECTION_PERCENTAGE 0.05
#define DEFAULT_DIFFERENCE_THRESHOLD 0.05

#define BENCHMARK_MAX_THREADS 64

/*
    Thin wrapper class to provide a uniform interface for
    input, error management, and benchmarking.
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
        size_t benchmark_iterations = 0;

        // Read arguments from command line
        get_arguments(argn, argv, &filename, &difference_threshold, &detection_percentage, &n_workers, &benchmark_iterations);

        auto main_body = [&](string filename, size_t n_workers) {
            /*
                Main body that will be used as timing reference for the entire program.
                Notice how both the initialization of the VideoCapture and the processing
                of the first frame will be *included* in the total program time, as it is
                a crucial part of the serial fraction of the program, and can hardly be parallelized.
            */

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
            size_t total_frames = video.get(cv::CAP_PROP_FRAME_COUNT);
            size_t frames = logic(video, background_blur_grey, detection_percentage, difference_threshold, n_workers);

            return make_tuple(frames, total_frames);
        };

        if(benchmark_iterations == 0) {
            /*
                Standard mode:
                Run the program normally, outputting the result to stdout
            */
            std::chrono::microseconds total_time(0);
            {
                utimer timer(&total_time);
                auto [frames, total_frames] = main_body(filename, n_workers);
                cout << "Motion frames over total video frames: (" << frames << " / " << total_frames << ")" << endl;
            }
            cout << "Total microseconds: " << total_time.count() << endl;
            return 0;
        } else {
            /*
                Benchmark mode:
                Ignore the input file given and search for the datasets in the videos/ folder.
            */
            vector<pair<string,size_t>> filenames =
               {{"videos/test_big.mp4", 790}, // Example solutions by running with the default settings
                {"videos/test_mid.mp4", 190},
                {"videos/test_small.mp4", 113}};
            // Follow the Unix philosophy and write everything to stdout
            cout << "name,filename,nworkers,avg,var" << endl;
            for(auto& file : filenames) {
                // Use the n_workers given as input as the limit
                for(size_t nw = 1; nw <= nworkers; nw++) {
                    auto [filename, solution] = file;

                    // Save the times in a vector to compute the variance
                    vector<int64_t> times;
                    for(size_t i = 0; i < benchmark_iterations; i++) {
                        std::chrono::microseconds time(0);
                        {
                            cumulative_utimer timer(&time);
                            auto [frames, total_frames] = main_body(filename, nw);
                            // Check that the number of moving frames is correct
                            if(frames != solution) {
                                cerr << "Wrong answer!" << endl;
                                return 1;
                            }
                        }
                        times.push_back(time.count());
                    }
                    // Compute average
                    int64_t avg = (reduce(times.begin(), times.end(), 0.0) / (float)times.size());

                    // Compute variance
                    transform(times.begin(), times.end(), times.begin(), [&](auto i) { return (i - avg) * (i - avg); });
                    int64_t var = sqrt(reduce(times.begin(), times.end(), 0.0) / (float)times.size());

                    // Output to csv/out
                    cout << argv[0] << "," << filename << "," << nw << "," << avg << "," << var << endl;
                }
            }
            return 0;
        }
    }
};
