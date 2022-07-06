#pragma once
#include <iostream>
#include <chrono>
#include <functional>

// Useful timer classes, either cumulative or manually stopped

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

    // The timer is stopped using the RAII pattern and summed up to the pointer given.
    ~cumulative_utimer() {
        auto end = std::chrono::high_resolution_clock::now();
        *result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    }
};

class cumulative_manual_utimer {
    std::chrono::system_clock::time_point begin;
    std::chrono::microseconds* result;
public:
    // Start the timer immediately.
    cumulative_manual_utimer(std::chrono::microseconds* result) : begin(std::chrono::high_resolution_clock::now()), result(result) {}

    // The timer is stopped manually and summed up to the pointer given.
    std::chrono::microseconds stop() {
        auto end = std::chrono::high_resolution_clock::now();
        *result += std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        return std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    }
};
