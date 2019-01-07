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

#include <google/protobuf/message.h>
#include <google/protobuf/util/message_differencer.h>

#ifdef DOCTEST_LIBRARY_INCLUDED
#include <sstream>
#include <testing/testing.grpc.pb.h>
#endif

namespace gvs {
namespace test {
namespace proto {

template <typename Proto, typename = std::enable_if_t<std::is_base_of<google::protobuf::Message, Proto>::value>>
::std::ostream& operator<<(::std::ostream& os, const Proto& proto) {
    return os << proto.DebugString();
}

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-test-util] operator<<_prints_string") {
    gvs::test::proto::TestMessage test_msg;
    test_msg.set_msg("Blah blah");

    std::stringstream ss;
    ss << test_msg;
    CHECK(ss.str() == "msg: \"Blah blah\"\n");
}
#endif

template <typename Proto, typename = std::enable_if_t<std::is_base_of<google::protobuf::Message, Proto>::value>>
bool operator==(const Proto& lhs, const Proto& rhs) {
    return google::protobuf::util::MessageDifferencer::Equals(lhs, rhs);
}

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-test-util] operator<<_prints_string") {
    gvs::test::proto::TestMessage test_msg;
    test_msg.set_msg("Blah blah");

    std::stringstream ss;
    ss << test_msg;
    CHECK(ss.str() == "msg: \"Blah blah\"\n");
}
#endif

} // namespace proto
} // namespace test
} // namespace gvs
