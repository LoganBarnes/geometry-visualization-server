// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2018 Logan Barnes - All Rights Reserved
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

#include "gvs/util/apply.hpp"
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
