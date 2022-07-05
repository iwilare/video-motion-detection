#pragma once
#include <string>
#include <fstream>
#include <algorithm>
#include <limits>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

vector<vector<float>> greyscale(const Mat& a) {
    vector<vector<float>> result(a.rows, vector<float>(a.cols));
    for (auto i = 0; i < a.rows; i++) {
        for (auto j = 0; j < a.cols; j++) {
            auto colors = a.at<Vec3b>(i, j);
            result[i][j] = ((float)colors[0] + (float)colors[1] + (float)colors[2]) / 3.0 / 255.0;
        }
    }
    return result;
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

const float f1_9 = 1.0 / 9.0;

const vector<vector<float>> AverageKernel = {
    {f1_9, f1_9, f1_9},
    {f1_9, f1_9, f1_9},
    {f1_9, f1_9, f1_9},
};

vector<float> preprocess_blur_greyscale(const Mat& frame) {
    return blur(AverageKernel, greyscale(frame));
}

size_t count_differences(const vector<float>& a, const vector<float>& b, float threshold) {
    size_t count = 0;
    for (size_t i = 0; i < a.size(); i++)
        count += abs(a[i] - b[i]) >= threshold;
    return count;
}

bool is_motion_processed_frame(const vector<float>& background_blur_grey, const vector<float>& processed_frame, float threshold, float detection_percentage) {
    return (float)count_differences(background_blur_grey, processed_frame, threshold)
         / (float)background_blur_grey.size()
         > detection_percentage;
}

// Composition of the three conceptual stages to process a frame
bool is_motion_frame(const vector<float>& background_blur_grey, const Mat& f, float threshold, float detection_percentage) {
    auto g = greyscale(f);
    auto b = blur(AverageKernel, g);
    auto d = is_motion_processed_frame(background_blur_grey, b, threshold, detection_percentage);
    return d;
}
