#include <opencv2/opencv.hpp>
#include <string>
#include <fstream>
#include <algorithm>

#include "utimer.cpp"
#include "args.cpp"
#include "cpu_affinity.cpp"

using namespace cv;
using namespace std;

size_t count_differences(const vector<float>& a, const vector<float>& b, float threshold) {
    size_t count = 0;
    for (size_t i = 0; i < a.size(); i++)
        count += abs(a[i] - b[i]) >= threshold ? 1 : 0;
    return count;
}

vector<float> blur(const vector<vector<float>>& kernel, const vector<vector<float>>& a) {
    vector<float> result(a.size() * a[0].size());
    for (size_t i = 0; i < a.size(); i++) {
        for (size_t j = 0; j < a[0].size(); j++) {
            float sum = 0;
            for (size_t ki = 0; ki < kernel.size(); ki++) {
                for (size_t kj = 0; kj < kernel[0].size(); kj++) {
                    size_t x = i + ki - kernel.size() / 2;
                    size_t y = j + kj - kernel[0].size() / 2;
                    if (0 <= x && x < a.size() && 0 <= y && y < a[0].size())
                        sum += kernel[ki][kj] * a[x][y];
                }
            }
            result[i * a[0].size() + j] = sum;
        }
    }
    return result;
}

vector<vector<float>> Kernel = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
};

vector<vector<float>> greyscale(const Mat& a) {
    vector<vector<float>> result(a.rows, vector<float>(a.cols));
    for (size_t i = 0; i < a.rows; i++) {
        for (size_t j = 0; j < a.cols; j++) {
            auto colors = a.at<Vec3b>(i, j);
            result[i][j] = ((float)colors[0] + (float)colors[1] + (float)colors[2]) / 3.0 / 255.0;
        }
    }
    return result;
}

vector<float> blur_grey(const Mat& frame) {
    return blur(Kernel, greyscale(frame));
}

size_t changed_pixels(vector<float> background_blur_grey, Mat& frame, float threshold) {
    return count_differences(background_blur_grey, blur_grey(frame), threshold);
}

int main(int argn, char** argv) {
    ios_base::sync_with_stdio(false);

    string filename;
    float detection_percentage = 0.05;
    size_t workers = thread::hardware_concurrency();
    bool set_thread_affinity = false;

    check_arguments(argn, argv, &filename, &detection_percentage, &workers, &set_thread_affinity);

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

        size_t motion_frames = 0;
        size_t total_frames = 0;

        float threshold = 0.3;

        Mat frame;
        for(video >> frame; !frame.empty(); video >> frame) {
            auto changed = (float)changed_pixels(background_blur_grey, frame, threshold) / (float)total_pixels > detection_percentage ? 1 : 0;
            motion_frames += changed;
            total_frames += 1;
            cout << (changed ? "X" : "_");
        }

        cout << "Total frames: " << total_frames << endl;
        cout << "Number of frames with motion: " << motion_frames << endl;
    }

    cout << total_time << endl; //read_time << computation_time << endl;

    return 0;
}
