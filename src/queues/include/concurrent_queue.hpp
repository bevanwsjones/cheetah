#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <optional>

namespace cheetah
{

template <typename T>
class ConcurrentQueue
{
    public: 

    ConcurrentQueue() = default;
    ~ConcurrentQueue() = default;

    void push(const T& value) {
        std::lock_guard lg(mut_que);
        // assert not done
        queue.push(value);
        cv_lock.notify_one();        
    };

    void push(const std::vector<T>& value){
        std::lock_guard lg(mut_que);
        // assert not done
        for(const auto& val : value){
            queue.push(val);
        }
        cv_lock.notify_one();
    };

    void set_done() {
        std::lock_guard lg(mut_que);
        done = true;
        cv_lock.notify_all();
    }

    std::optional<T> front_pop() {
        std::unique_lock lg(mut_que);
        cv_lock.wait(lg, [this] { return !queue.empty() | done; });
        if (!queue.empty()) [[likely]] {
            const T value = queue.front();
            queue.pop();
            return value;
        }
        return std::nullopt;
    };

    private:

    bool done{false};
    std::queue<T> queue;
    std::mutex mut_que;
    std::condition_variable cv_lock;
    
};

}  // namespace cheetah
