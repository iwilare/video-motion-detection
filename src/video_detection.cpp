#include <string>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

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

vector<vector<float>> greyscale(const Mat* m) {
    Mat a(move(*m));
    vector<vector<float>> result(a.rows, vector<float>(a.cols));
    for (auto i = 0; i < a.rows; i++) {
        for (auto j = 0; j < a.cols; j++) {
            auto colors = a.at<Vec3b>(i, j);
            result[i][j] = ((float)colors[0] + (float)colors[1] + (float)colors[2]) / 3.0 / 255.0;
        }
    }
    return result;
}

vector<float> blur_grey(const Mat* frame) {
    return blur(Kernel, greyscale(frame));
}

size_t changed_pixels(vector<float>& background_blur_grey, const Mat* frame, float threshold) {
    return count_differences(background_blur_grey, blur_grey(frame), threshold);
}

bool is_motion_frame(vector<float> background_blur_grey, size_t total_pixels, const Mat* frame, float difference_threshold, float detection_percentage) {
    return (float)changed_pixels(background_blur_grey, frame, difference_threshold) / (float)total_pixels > detection_percentage;
}
