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

/**
 * @brief This interface makes declarations of CallbackHandler more practical
 *
 * Instead of defining:
 *
 *     CallbackHandler<MyReturnType, decltype(&MyClass::my_function), MyClass*, Arg1Type, etc...> callback;
 *
 * we can define:
 *
 *     std::unique_ptr<CallbackInterface<MyReturnType>> callback;
 *
 * The callback can then be created and used in a more readable fashion:
 *
 *     callback = make_callback<MyReturnType>(&MyClass::my_function, my_instance, arg1, etc...); // create
 *     callback->invoke(); // use
 */
template <typename ReturnType, typename... Args>
class CallbackInterface {
public:
    virtual ~CallbackInterface() = 0;

    /**
     * The function that invokes the callback with optional extra data
     */
    virtual ReturnType invoke(Args&&... additional_arguments) = 0;
};

template <typename ReturnType, typename... Args>
CallbackInterface<ReturnType, Args...>::~CallbackInterface() = default;

/**
 * @brief The typed callback class containing the callback function and expected arguments
 *
 * NOTE: All callbacks will require a void* as the last argument. This can be avoided with a bit of
 * template magic but for the sake of keeping this class relatively simple that hasn't been added.
 */
template <typename ReturnType, typename Callback, typename Tuple, typename... Args>
class CallbackHandler : public CallbackInterface<ReturnType, Args...> {
public:
    template <typename... StaticArgs>
    explicit CallbackHandler(Callback callback, StaticArgs&&... args);
    ~CallbackHandler() override = default;

    ReturnType invoke(Args&&... additional_arguments) override;

private:
    Callback callback_;
    Tuple args_;
};

template <typename ReturnType, typename Callback, typename Tuple, typename... Args>
template <typename... StaticArgs>
CallbackHandler<ReturnType, Callback, Tuple, Args...>::CallbackHandler(Callback callback, StaticArgs&&... args)
    : callback_(std::move(callback)), args_(std::make_tuple(std::forward<StaticArgs>(args)...)) {}

template <typename ReturnType, typename Callback, typename Tuple, typename... Args>
ReturnType CallbackHandler<ReturnType, Callback, Tuple, Args...>::invoke(Args&&... additional_arguments) {
    return apply(callback_, args_, std::forward<Args>(additional_arguments)...); // all the magic happens here
}

/**
 * @brief Used to create callbacks without having to define all the template arguments:
 *
 *     auto callback = make_callback<MyReturnType>(&MyClass::my_function, my_instance, arg1, etc...);
 */
template <typename ReturnType, typename... Args, typename Callback, typename... StaticArgs>
std::unique_ptr<CallbackHandler<ReturnType, Callback, std::tuple<typename std::decay<StaticArgs>::type...>, Args...>>
make_callback(Callback callback, StaticArgs&&... args) {
    return std::make_unique<
        CallbackHandler<ReturnType, Callback, std::tuple<typename std::decay<StaticArgs>::type...>, Args...>>(
        std::move(callback), std::forward<StaticArgs>(args)...);
}

} // namespace util
} // namespace gvs
