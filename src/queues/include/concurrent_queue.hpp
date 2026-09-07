#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

namespace cheetah
{

template <typename T>
class ConcurrentQueue
{
    public: 

    ConcurrentQueue() = default;
    ~ConcurrentQueue() = default;

    void push(const T& value) {
        std::unique_lock lg(mut_que);
        queue.push_back(value);
        cv_lock.notify_one();        
    };

    void push(const std::vector<T>& value){
        std::unique_lock lg(mut_que);
        for(const auto& val : value){
            queue.push_back(val);
        }
        cv_lock.notify_one();
    };

    T front_pop() {
        std::unique_lock lg(mut_que);
        cv_lock.wait(lg, [this] { return !queue.empty(); });
        const T value = queue.front();
        queue.pop_front();
        return value;
    };

    private:

    std::deque<T> queue;
    std::mutex mut_que;
    std::condition_variable cv_lock;
    
};

}  // namespace cheetah
