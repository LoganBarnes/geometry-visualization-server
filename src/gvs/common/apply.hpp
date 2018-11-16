// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <tuple>
#include <utility>

namespace gvs {
namespace util {
namespace detail {

template <typename F, typename T1, typename... Args>
auto invoke(F&& f, T1&& arg1, Args&&... args) -> decltype((arg1->*std::forward<F>(f))(args...)) {
    return (arg1->*std::forward<F>(f))(args...);
}

template <typename F, typename... Args>
auto invoke(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(args...)) {
    return std::forward<F>(f)(args...);
}

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
