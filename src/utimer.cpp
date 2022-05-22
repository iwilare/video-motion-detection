#include <iostream>
#include <chrono>
#include <functional>

class utimer {
    std::chrono::system_clock::time_point begin;
    int64_t* result;
public:

    // Postpone starting the timer until the result location is provided.
    utimer() {}

    // Start the timer immediately.
    utimer(int64_t* result) : begin(std::chrono::high_resolution_clock::now()), result(result) {}

    // Manually start the timer with the result location.
    void start(int64_t* r) {
        result = r;
        begin = std::chrono::high_resolution_clock::now();
    }

    // Following the RAII pattern, the timer is stopped when its lifetime ends.
    ~utimer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - begin;
        auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        *result = usec;
    }
};

template<typename F> int64_t measure_utime(F f) {
    int64_t utime;
    {
        utimer timer(&utime);
        f();
    }
    return utime;
}

template<typename F> int64_t measure_utime(size_t repetitions, F f) {
    int64_t utime_sum = 0;
    for(size_t i = 0; i < repetitions; i++) {
        int64_t utime;
        {
            utimer timer(&utime);
            f();
        }
        utime_sum += utime;
    }
    return utime_sum / repetitions;
}
