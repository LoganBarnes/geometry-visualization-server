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

// external
#include <mapbox/variant.hpp>

// standard
#include <stdexcept>
#include <string>

namespace gvs {
namespace util11 {

struct Error {
    std::string error_message;

    explicit operator bool() const { return !error_message.empty(); }

    auto throw_if_error() -> void {
        if (*this) {
            throw std::runtime_error("Error: " + error_message);
        }
    }
};

inline auto success() -> Error {
    return Error{}; // empty error
}

/**
 * @brief A simplified version of gvs::util::Result for use in Cpp11 code
 */
template <typename T, typename E = Error, typename = typename std::enable_if<!std::is_same<T, E>::value>::type>
struct Result : mapbox::util::variant<T, E> {
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Result(T v) : mapbox::util::variant<T, E>(std::move(v)) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Result(E e) : mapbox::util::variant<T, E>(std::move(e)) {}

    struct HasValueVisitor {
        auto operator()(T const&) const -> bool { return true; }
        auto operator()(E const&) const -> bool { return false; }
    };

    auto has_value() const -> bool { return mapbox::util::apply_visitor(HasValueVisitor{}, *this); }

    struct ValueVisitor {
        auto operator()(T const& t) -> T const& { return t; }
        auto operator()(E const&) const -> T const& {
            throw std::runtime_error("Attempting to access non existent value");
        }
    };

    auto value() const -> T const& { return mapbox::util::apply_visitor(ValueVisitor{}, *this); }

    struct ErrorVisitor {
        auto operator()(E const& e) const -> E const& { return e; }
        auto operator()(T const&) const -> E const& {
            throw std::runtime_error("Attempting to access non existent error");
        }
    };

    auto error() const -> E const& { return mapbox::util::apply_visitor(ErrorVisitor{}, *this); }

    explicit operator bool() const { return has_value(); }

    auto value_or_throw() const -> T const& {
        if (*this) {
            return value();
        } else {
            throw std::runtime_error("Error: " + error().error_message);
        }
    }
};

} // namespace util11
} // namespace gvs