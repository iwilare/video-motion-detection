#pragma once
#include <iostream>
#include <chrono>
#include <functional>

class utimer {
    std::chrono::system_clock::time_point begin;
    std::chrono::microseconds* result;
public:
    // Start the timer immediately.
    utimer(std::chrono::microseconds* result) : begin(std::chrono::high_resolution_clock::now()), result(result) {}

    // Following the RAII pattern, the timer is stopped when its lifetime ends.
    ~utimer() {
        auto end = std::chrono::high_resolution_clock::now();
        *result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    }
};

class cumulative_utimer {
    std::chrono::system_clock::time_point begin;
    std::chrono::microseconds* result;
public:
    // Start the timer immediately.
    cumulative_utimer(std::chrono::microseconds* result) : begin(std::chrono::high_resolution_clock::now()), result(result) {}

    // The timer is stopped manually and summed up to the pointer given.
    void stop() {
        auto end = std::chrono::high_resolution_clock::now();
        *result += std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    }
};
