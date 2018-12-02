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

#include <tuple>
#include <utility>

namespace gvs {
namespace util {

template <typename F, typename T1, typename... Args>
auto invoke(F&& f, T1&& arg1, Args&&... args) -> decltype((arg1->*std::forward<F>(f))(args...)) {
    return (arg1->*std::forward<F>(f))(args...);
}

template <typename F, typename... Args>
auto invoke(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(args...)) {
    return std::forward<F>(f)(args...);
}

namespace detail {

template <typename F, typename Tuple, std::size_t... I, typename... Args>
auto apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>, Args&&... more_args) {
    return invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))..., std::forward<Args>(more_args)...);
}

} // namespace detail

/**
 * Modification of std::apply (C++17)
 */
template <typename F, typename Tuple, typename... Args>
auto apply(F&& f, Tuple&& t, Args&&... more_args) {
    return detail::apply_impl(std::forward<F>(f),
                              std::forward<Tuple>(t),
                              std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{},
                              std::forward<Args>(more_args)...);
}

} // namespace util
} // namespace gvs
