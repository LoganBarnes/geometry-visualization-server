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

// project
#include "gvs/util/error.hpp"

// external
#include <tl/expected.hpp>

#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CHECK_RESULT __attribute__((warn_unused))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define CHECK_RESULT _Check_return_
#else
#define CHECK_RESULT
#endif

namespace gvs::util {

/**
 * @brief A type of tl::expected that requires the result to be used.
 *
 * Example usage:
 *
 * @code{.cpp}

    // function that returns 'MyType' or 'gvs::util::Error'
    gvs::util::Result<MyType> my_function(int non_negative) {
        if (non_negative < 0) {
            return tl::make_unexpected(MAKE_ERROR("number is negative"));
        }
        return non_negative; // Success
    }

    int main() {
        my_function(0); // compilation error since the return value is not used (C++17)

        auto result1 = my_function(1); // compilation error since 'result1' is unused

        auto result2 = my_function(2); // OK, result is used
        if (result2) {
            ...
        } else {
            ...
        }

        my_function(3).map(...).map_error(...); // OK

        gvs::util::ignore(my_function(4)); // OK, but not recommended

        auto result3 = my_function(5); // OK, but not recommended
        gvs::util::ignore(result3);

        return 0;
    }

 * @endcode
 *
 */
template <typename T, typename E>
struct CHECK_RESULT Result;

template <typename T, typename Error = ::gvs::util::Error>
struct [[nodiscard]] Result : tl::expected<T, Error> {
    using tl::expected<T, Error>::expected;

    template <typename F, typename... Args>
    constexpr auto and_then(F && f, Args && ... args)& {
        return and_then_impl(*this, std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    constexpr auto and_then(F && f, Args && ... args)&& {
        return and_then_impl(std::move(*this), std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    constexpr auto and_then(F && f, Args && ... args) const& {
        return and_then_impl(*this, std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename F, typename... Args>
    constexpr auto and_then(F && f, Args && ... args) const&& {
        return and_then_impl(std::move(*this), std::forward<F>(f), std::forward<Args>(args)...);
    }

    auto value_or_throw() { return value_or_throw_impl(std::move(*this)); }
};

template <typename... Args>
void ignore(Args&&...) {}

inline Result<void> success() {
    return {};
}

template <typename T1, typename E1, typename T2, typename E2>
auto operator==(const Result<T1, E1>& lhs, const Result<T2, E2>& rhs) -> bool {
    return static_cast<const tl::expected<T1, E1>&>(lhs) == static_cast<const tl::expected<T2, E2>&>(rhs);
}

template <typename T1, typename E1, typename T2, typename E2>
auto operator!=(const Result<T1, E1>& lhs, const Result<T2, E2>& rhs) -> bool {
    return !(lhs == rhs);
}

template <typename T, typename E, typename = std::enable_if_t<std::is_same_v<E, Error>>>
auto value_or_throw_impl(Result<T, E>&& result) -> T {
    if (!result.has_value()) {
        throw std::runtime_error(result.error().error_message());
    }
    return result.value();
}

template <typename E, typename = std::enable_if_t<std::is_same_v<E, Error>>>
auto value_or_throw_impl(Result<void, E>&& result) -> void {
    if (!result.has_value()) {
        throw std::runtime_error(result.error().error_message());
    }
}

} // namespace gvs::util

namespace tl::detail {

// Trait for checking if a type is a gvs::util::Result
template <class T>
struct is_result_impl : std::false_type {};
template <class T, class E>
struct is_result_impl<::gvs::util::Result<T, E>> : std::true_type {};
template <class T>
using is_result = is_result_impl<decay_t<T>>;

template <typename Exp,
          typename F,
          typename... Args,
          detail::enable_if_t<!std::is_void<exp_t<Exp>>::value>* = nullptr,
          typename Ret = decltype(detail::invoke(std::declval<F>(), *std::declval<Exp>(), std::declval<Args>()...))>
constexpr auto and_then_impl(Exp&& exp, F&& f, Args&&... args) {
    static_assert(detail::is_result<Ret>::value, "F must return an expected");

    return exp.has_value() ? detail::invoke(std::forward<F>(f), *std::forward<Exp>(exp), std::forward<Args>(args)...)
                           : Ret(unexpect, std::forward<Exp>(exp).error());
}

template <typename Exp,
          typename F,
          typename... Args,
          detail::enable_if_t<std::is_void<exp_t<Exp>>::value>* = nullptr,
          typename Ret = decltype(detail::invoke(std::declval<F>(), std::declval<Args>()...))>
constexpr auto and_then_impl(Exp&& exp, F&& f, Args&&... args) {
    static_assert(detail::is_result<Ret>::value, "F must return an expected");

    return exp.has_value() ? detail::invoke(std::forward<F>(f), std::forward<Args>(args)...)
                           : Ret(unexpect, std::forward<Exp>(exp).error());
}

} // namespace tl::detail
