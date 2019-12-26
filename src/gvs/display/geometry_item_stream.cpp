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
#include "geometry_item_stream.hpp"

namespace gvs::log {

GeometryItemStream::GeometryItemStream(std::string id, SceneInfoSender& sender) : id_(std::move(id)), sender_(sender) {}

void GeometryItemStream::send_current_data(SendType type) {
    auto result = sender_.update_scene(id_, std::move(info_), type);
    if (!result) {
        error_message_ = result.error().error_message();
    }
    info_ = {};
}

GeometryItemStream& GeometryItemStream::operator<<(GeometryItemStream& (*send_func)(GeometryItemStream&)) {
    return send_func(*this);
}

const std::string& GeometryItemStream::id() const {
    return id_;
}

bool GeometryItemStream::success() const {
    return error_message_.empty();
}

const std::string& GeometryItemStream::error_message() const {
    return error_message_;
}

} // namespace gvs::log
