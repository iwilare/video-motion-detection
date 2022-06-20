#include <thread>
#include <iostream>

using namespace std;

// Set the affinity of the given thread so that it is pinned to the i-th core.
void pin_thread_to_cpu(size_t i, std::thread& thread) {
    cpu_set_t cpuset;

    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);

    // Give the native handler a mask that tells on which core the thread should run
    int status = pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (status != 0) {
        std::cerr << "Warning: pthread_setaffinity_np returned " << status << " while pinning thread to CPU #" << i << "\n";
    }
}
