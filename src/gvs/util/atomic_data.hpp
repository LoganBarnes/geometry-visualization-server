// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2019 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// standard
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

namespace gvs::util {

struct ScopedLock {
    std::shared_ptr<std::mutex>                  mutex;
    std::shared_ptr<std::lock_guard<std::mutex>> lock;
};

/**
 * @brief Owns complex data that can be accessed in a thread safe way.
 *
 * Example:
 *
 *     struct MyComplexData {
 *         int thing1;
 *         bool thing2;
 *         std::vector<double> more_things;
 *         OtherStruct complex_thing;
 *     };
 *
 *     util::AtomicData<MyComplexData> shared_data;
 *
 *     ... Later, in different threads
 *
 *     shared_data.use_safely([] (MyComplexData& data) {
 *         // Do things with 'data' here:
 *         ...
 *     });
 *
 *     ...
 */
template <typename T>
class AtomicData {
public:
    explicit AtomicData(T&& data);

    template <typename... Args>
    explicit AtomicData(Args&&... args);

    /**
     * @brief Use the data in a thread safe manner.
     */
    template <typename Func>
    auto use_safely(Func func);

    template <typename Func>
    auto use_safely(Func func) const;

    /**
     * @brief Wait for 'notify_one' or 'notify_all' to be called on this data
     *        structure before using the data in a thread safe manner.
     */
    template <typename Pred, typename Func>
    auto wait_to_use_safely(Pred predicate, Func func) -> void;

    template <typename Pred, typename Func>
    auto wait_to_use_safely(Pred predicate, Func func) const -> void;

    /**
     * @brief Same as 'wait_to_use_safely' except a maximum wait time can be set.
     */
    template <typename Rep, typename Period, typename Pred, typename Func>
    auto wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration, Pred predicate, Func func) -> bool;

    template <typename Rep, typename Period, typename Pred, typename Func>
    auto wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration, Pred predicate, Func func) const
        -> bool;

    /**
     * @brief Allow one 'wait_to_use_safely' function to continue.
     */
    auto notify_one() -> void;

    /**
     * @brief Allow all 'wait_to_use_safely' functions to continue.
     */
    auto notify_all() -> void;

    auto unsafe_data() -> T&;
    auto unsafe_data() const -> T const&;

    [[nodiscard]] ScopedLock lock() const;

private:
    std::shared_ptr<std::mutex>              lock_;
    std::shared_ptr<std::condition_variable> condition_;
    T                                        data_;
};

template <typename T>
AtomicData<T>::AtomicData(T&& data)
    : lock_(std::make_shared<std::mutex>()),
      condition_(std::make_shared<std::condition_variable>()),
      data_(std::forward<T>(data)) {}

template <typename T>
template <typename... Args>
AtomicData<T>::AtomicData(Args&&... args)
    : lock_(std::make_shared<std::mutex>()),
      condition_(std::make_shared<std::condition_variable>()),
      data_(std::forward<Args>(args)...) {}

template <typename T>
template <typename Func>
auto AtomicData<T>::use_safely(Func func) {
    std::lock_guard<std::mutex> scoped_lock(*lock_);
    return func(data_);
}

template <typename T>
template <typename Func>
auto AtomicData<T>::use_safely(Func func) const {
    std::lock_guard<std::mutex> scoped_lock(*lock_);
    return func(data_);
}

template <typename T>
template <typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(Pred predicate, Func func) -> void {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    condition_->wait(unlockable_lock, [&] { return predicate(data_); });
    func(data_);
}

template <typename T>
template <typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(Pred predicate, Func func) const -> void {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    condition_->wait(unlockable_lock, [&] { return predicate(data_); });
    func(data_);
}

template <typename T>
template <typename Rep, typename Period, typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration, Pred predicate, Func func)
    -> bool {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    if (condition_->wait_for(unlockable_lock, duration, [&] { return predicate(data_); })) {
        func(data_);
        return true;
    }
    return false;
}

template <typename T>
template <typename Rep, typename Period, typename Pred, typename Func>
auto AtomicData<T>::wait_to_use_safely(std::chrono::duration<Rep, Period> const& duration,
                                       Pred                                      predicate,
                                       Func                                      func) const -> bool {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    if (condition_->wait_for(unlockable_lock, duration, [&] { return predicate(data_); })) {
        func(data_);
        return true;
    }
    return false;
}

template <typename T>
auto AtomicData<T>::notify_one() -> void {
    condition_->notify_one();
}

template <typename T>
auto AtomicData<T>::notify_all() -> void {
    condition_->notify_all();
}

template <typename T>
auto AtomicData<T>::unsafe_data() -> T& {
    return data_;
}

template <typename T>
auto AtomicData<T>::unsafe_data() const -> T const& {
    return data_;
}

template <typename T>
ScopedLock AtomicData<T>::lock() const {
    ScopedLock scoped_lock{lock_};
    scoped_lock.lock = std::make_shared<std::lock_guard<std::mutex>>(*scoped_lock.mutex);
    return scoped_lock;
}

} // namespace gvs::util
