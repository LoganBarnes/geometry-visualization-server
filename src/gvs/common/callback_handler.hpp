// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/common/apply.hpp"
#include <memory>

namespace gvs {
namespace util {

template <typename ReturnType>
class CallbackInterface {
public:
    virtual ~CallbackInterface() = 0;
    virtual ReturnType invoke() = 0;
};

template <typename ReturnType>
CallbackInterface<ReturnType>::~CallbackInterface() = default;

template <typename ReturnType, typename Callback, typename... Args>
class CallbackHandler : public CallbackInterface<ReturnType> {
public:
    explicit CallbackHandler(Callback callback, Args&&... args);
    ~CallbackHandler() override = default;

    ReturnType invoke() override;

private:
    Callback callback_;
    std::tuple<typename std::decay<Args>::type...> args_;
};

template <typename ReturnType, typename Callback, typename... Args>
CallbackHandler<ReturnType, Callback, Args...>::CallbackHandler(Callback callback, Args&&... args)
    : callback_(callback), args_(std::make_tuple(std::forward<Args>(args)...)) {}

template <typename ReturnType, typename Callback, typename... Args>
ReturnType CallbackHandler<ReturnType, Callback, Args...>::invoke() {
    return apply(callback_, args_);
}

template <typename ReturnType, typename Callback, typename... Args>
std::unique_ptr<CallbackHandler<ReturnType, Callback, Args...>> make_callback(Callback callback, Args&&... args) {
    return std::make_unique<CallbackHandler<ReturnType, Callback, Args...>>(callback, std::forward<Args>(args)...);
}

} // namespace util
} // namespace gvs
