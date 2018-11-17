// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <mutex>
#include <memory>
#include <condition_variable>
#include <queue>

namespace gvs {
namespace util {

// Pretty similar to https://stackoverflow.com/a/12805690
template <typename T>
class BlockingQueue {
public:
    void push_back(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value)); // push_back
        }
        condition_.notify_one();
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.emplace(std::forward<Args>(args)...); // emplace_back
        }
        condition_.notify_one();
    }

    T pop_front() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [=] { return !queue_.empty(); });
        T rc(std::move(queue_.front()));
        queue_.pop(); // pop_front
        return rc;
    }

    T pop_all_but_most_recent() {
        std::lock_guard<std::mutex> scoped_lock(mutex_);
        while (queue_.size() > 1) {
            queue_.pop();
        }
        return queue_.front();
    }

    bool empty() {
        std::lock_guard<std::mutex> scoped_lock(mutex_);
        return queue_.empty();
    }

    auto size() {
        std::lock_guard<std::mutex> scoped_lock(mutex_);
        return queue_.size();
    }

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<T> queue_;
};

} // namespace util
} // namespace gvs
