#include <mutex>
#include <queue>
#include <condition_variable>
#include <optional>
#include <utility>

// A straightworward blocking wrapper on std::queue to
// ensure thread-safety on push and pop.

template<typename T> class shared_queue {
    std::condition_variable available;
    std::mutex              mutex;
    std::queue<T>           queue;
    bool                    is_done = false;
public:
    shared_queue() {}
    ~shared_queue() {}

    void push(const T v) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(v);
        available.notify_one();
    }
    void done() {
        std::unique_lock<std::mutex> lock(mutex);
        is_done = true;
        available.notify_all();
    }
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex);

        while(queue.empty() && !is_done)
            available.wait(lock);

        if(queue.empty() && is_done) {
            return {};
        } else {
            T value(std::move(queue.front()));
            queue.pop();
            return value;
        }
    }
};
