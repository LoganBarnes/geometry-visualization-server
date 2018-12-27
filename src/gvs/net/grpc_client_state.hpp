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

#include <stdexcept>
#include <string>

#ifdef DOCTEST_LIBRARY_INCLUDED
#include <sstream>
#endif

namespace gvs {
namespace net {

enum class GrpcClientState {
    not_connected,
    attempting_to_connect,
    connected,
};

inline std::string to_string(const GrpcClientState& state) {
    switch (state) {
    case GrpcClientState::not_connected:
        return "not_connected";
    case GrpcClientState::attempting_to_connect:
        return "attempting_to_connect";
    case GrpcClientState::connected:
        return "connected";
    }
    throw std::invalid_argument("Invalid GrpcClientState");
}

inline ::std::ostream& operator<<(::std::ostream& os, const GrpcClientState& state) {
    return os << to_string(state);
}

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-net] testing the GrpcClientState string functions") {
    std::stringstream ss;

    SUBCASE("not_connected_string") {
        ss << GrpcClientState::not_connected;
        CHECK(ss.str() == "not_connected");
    }

    SUBCASE("attempting_to_connect_string") {
        ss << GrpcClientState::attempting_to_connect;
        CHECK(ss.str() == "attempting_to_connect");
    }

    SUBCASE("connected_string") {
        ss << GrpcClientState::connected;
        CHECK(ss.str() == "connected");
    }

    SUBCASE("invalid_GrpcClientState_string") {
        // Really have to do some shadily incorrect coding to cause this
        CHECK_THROWS_AS(ss << static_cast<GrpcClientState>(-1), std::invalid_argument);
    }
}
#endif

} // namespace net
} // namespace gvs
