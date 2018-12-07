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

// project
#include "gvs/log/log_params.hpp"
#include "gvs/log/send.hpp"

// generated
#include <gvs/scene.grpc.pb.h>

namespace gvs {
namespace log {

#define CHECK_WITH_THROW(errors)                                                                                       \
    if (not errors.success()) {                                                                                        \
        throw std::runtime_error("Error (stream " + stream.id() + "): " + stream.error_message());                     \
    }

#define CHECK(stream)                                                                                                  \
    if (not stream.success()) {                                                                                        \
        std::cerr << "Error (stream " << stream.id() << "): " << stream.error_message() << std::endl;                  \
    }

enum class SendType {
    safe,
    replace,
    append,
};

class GeometryItemStream {
public:
    explicit GeometryItemStream(std::string id, proto::Scene::Stub* stub);

    void send(SendType type);

    template <typename Functor>
    GeometryItemStream& operator<<(Functor&& functor);

    GeometryItemStream& operator<<(GeometryItemStream& (*send_func)(GeometryItemStream&));

    const std::string& id() const;
    bool success() const;
    const std::string& error_message() const;

private:
    const std::string id_;
    proto::Scene::Stub* stub_;
    proto::SceneItemInfo info_;

    std::string error_message_ = "";
};

template <typename Functor>
GeometryItemStream& GeometryItemStream::operator<<(Functor&& functor) {
    std::string error_name = functor(&info_);
    if (not error_name.empty()) {
        throw std::invalid_argument(error_name + " is already set");
    }
    return *this;
}

} // namespace log
} // namespace gvs
