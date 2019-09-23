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

#if __cplusplus >= 201703L

#include <type_traits>
#include <utility>

namespace gvs::detail {

template <typename T, std::size_t I>
constexpr auto check_indexable(int) -> decltype(std::declval<T>()[I], bool()) {
    return true;
}
template <typename T, std::size_t>
constexpr bool check_indexable(long) {
    return false;
}

template <typename T, typename, std::size_t I>
constexpr bool is_indexable() {
    return check_indexable<T, I>(0);
}

template <typename T, typename F, std::size_t I, std::size_t... Is, typename = std::enable_if_t<sizeof...(Is) != 0>>
constexpr bool is_indexable() {
    return check_indexable<T, I>(0) && std::is_same_v<decltype(std::declval<T>()[I]), F> && is_indexable<T, F, Is...>();
}

template <typename T, typename F, std::size_t... Is>
constexpr bool is_indexable(std::index_sequence<Is...>) {
    return is_indexable<T, F, Is...>();
}

template <std::size_t N, typename T>
constexpr bool is_vec() {
    return sizeof(T) == sizeof(float) * N && is_indexable<T, float&>(std::make_index_sequence<N>());
}

template <std::size_t N, std::size_t M, typename T>
constexpr bool is_mat() {
    return sizeof(T) == sizeof(float) * N * M && is_indexable<T, float*>(std::make_index_sequence<N>())
        && is_indexable<decltype(std::declval<T>()[N - 1]), float&>(std::make_index_sequence<M>());
}

template <typename T>
constexpr auto is_vec3 = is_vec<3, T>;

template <typename T>
constexpr auto is_vec2 = is_vec<2, T>;

template <typename T>
constexpr auto is_mat4 = is_mat<4, 4, T>;

} // namespace gvs::detail

#endif
