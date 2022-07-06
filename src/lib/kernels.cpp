#pragma once
#include <string>
#include <fstream>
#include <algorithm>
#include <limits>
#include <opencv2/opencv.hpp>

// Some default kernels usable to blur; no constraint on the kernel size is enforced.

const float f1_8 = 1.0 / 8.0;
const float f1_9 = 1.0 / 9.0;
const float f1_10 = 1.0 / 10.0;
const float f1_16 = 1.0 / 16.0;

const vector<vector<float>> AverageKernel = {
    {f1_9, f1_9, f1_9},
    {f1_9, f1_9, f1_9},
    {f1_9, f1_9, f1_9},
};

const vector<vector<float>> H2_Kernel = {
    {1.0 * f1_10, 1.0 * f1_10, 1.0 * f1_10},
    {1.0 * f1_10, 2.0 * f1_10, 1.0 * f1_10},
    {1.0 * f1_10, 1.0 * f1_10, 1.0 * f1_10},
};

const vector<vector<float>> H3_Kernel = {
    {1.0 * f1_16, 2.0 * f1_16, 1.0 * f1_16},
    {2.0 * f1_16, 4.0 * f1_16, 2.0 * f1_16},
    {1.0 * f1_16, 2.0 * f1_16, 1.0 * f1_16},
};

const vector<vector<float>> H4_Kernel = {
    {1.0 * f1_8, 1.0 * f1_8, 1.0 * f1_8},
    {1.0 * f1_8, 0.0 * f1_8, 1.0 * f1_8},
    {1.0 * f1_8, 1.0 * f1_8, 1.0 * f1_8},
};
