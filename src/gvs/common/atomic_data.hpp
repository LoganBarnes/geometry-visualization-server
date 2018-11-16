// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <mutex>
#include <condition_variable>

namespace gvs {
namespace util {

template <typename T>
class AtomicData {
public:
    explicit AtomicData(T data = {});

    template <typename Func>
    void use_safely(const Func& func);

    template <typename Func>
    void use_safely(const Func& func) const;

    template <typename Pred, typename Func>
    void wait_to_use_safely(const Pred& predicate, const Func& func);

    template <typename Pred, typename Func>
    void wait_to_use_safely(const Pred& predicate, const Func& func) const;

    template <typename Pred, typename Func>
    bool wait_to_use_safely(unsigned max_wait_time_millis, const Pred& predicate, const Func& func);

    template <typename Pred, typename Func>
    bool wait_to_use_safely(unsigned max_wait_time_millis, const Pred& predicate, const Func& func) const;

    void notify_one();

    void notify_all();

    T& unsafe_data();
    const T& unsafe_data() const;

private:
    std::unique_ptr<std::mutex> lock_;
    std::condition_variable condition_;
    T data_;
};

template <typename T>
AtomicData<T>::AtomicData(T data) : lock_(std::make_unique<std::mutex>()), data_(std::move(data)) {}

template <typename T>
T& AtomicData<T>::unsafe_data() {
    return data_;
}

template <typename T>
const T& AtomicData<T>::unsafe_data() const {
    return data_;
}

template <typename T>
template <typename Func>
void AtomicData<T>::use_safely(const Func& func) {
    std::lock_guard<std::mutex> scoped_lock(*lock_);
    func(data_);
}

template <typename T>
template <typename Func>
void AtomicData<T>::use_safely(const Func& func) const {
    std::lock_guard<std::mutex> scoped_lock(*lock_);
    func(data_);
}

template <typename T>
template <typename Pred, typename Func>
void AtomicData<T>::wait_to_use_safely(const Pred& predicate, const Func& func) {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    condition_.wait(unlockable_lock, [&] { return predicate(data_); });
    func(data_);
}

template <typename T>
template <typename Pred, typename Func>
void AtomicData<T>::wait_to_use_safely(const Pred& predicate, const Func& func) const {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    condition_.wait(unlockable_lock, [&] { return predicate(data_); });
    func(data_);
}

template <typename T>
template <typename Pred, typename Func>
bool AtomicData<T>::wait_to_use_safely(unsigned max_wait_time_millis, const Pred& predicate, const Func& func) {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    if (condition_.wait_for(unlockable_lock, std::chrono::milliseconds(max_wait_time_millis), [&] {
            return predicate(data_);
        })) {
        func(data_);
        return true;
    }
    return false;
}

template <typename T>
template <typename Pred, typename Func>
bool AtomicData<T>::wait_to_use_safely(unsigned max_wait_time_millis, const Pred& predicate, const Func& func) const {
    std::unique_lock<std::mutex> unlockable_lock(*lock_);
    if (condition_.wait_for(unlockable_lock, std::chrono::milliseconds(max_wait_time_millis), [&] {
            return predicate(data_);
        })) {
        func(data_);
        return true;
    }
    return false;
}

template <typename T>
void AtomicData<T>::notify_one() {
    condition_.notify_one();
}

template <typename T>
void AtomicData<T>::notify_all() {
    condition_.notify_all();
}

} // namespace util
} // namespace gvs
