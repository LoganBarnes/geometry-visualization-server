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

#include <google/protobuf/message.h>
#include <google/protobuf/util/message_differencer.h>

namespace gvs {
namespace proto {

template <typename Proto, typename = std::enable_if_t<std::is_base_of<google::protobuf::Message, Proto>::value>>
::std::ostream& operator<<(::std::ostream& os, const Proto& proto) {
    return os << proto.DebugString();
}

//template <typename Proto, typename = std::enable_if_t<std::is_base_of<google::protobuf::Message, Proto>::value>>
template <typename Proto>
bool operator==(const Proto& lhs, const Proto& rhs) {
    return google::protobuf::util::MessageDifferencer::Equals(lhs, rhs);
}

} // namespace proto
} // namespace gvs
