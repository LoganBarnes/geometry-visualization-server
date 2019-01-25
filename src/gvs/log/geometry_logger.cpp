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
#include "geometry_logger.hpp"

#include <grpc++/client_context.h>

namespace gvs {
namespace log {

GeometryLogger::GeometryLogger(const std::string& server_address)
    : GeometryLogger(server_address, std::chrono::seconds(4)) {}

bool GeometryLogger::connected() const {
    return stub_ != nullptr;
}

std::string GeometryLogger::generate_uuid() const {
    return xg::newGuid().str();
}

std::string GeometryLogger::clear_all_items() {
    if (stub_) {
        proto::SceneUpdateRequest update;
        update.mutable_clear_all();

        grpc::ClientContext context;
        proto::Errors errors;

        grpc::Status status = stub_->UpdateScene(&context, update, &errors);

        if (not status.ok()) {
            return status.error_message();
        }

        if (not errors.error_msg().empty()) {
            return errors.error_msg();
        }
    }
    return "";
}

GeometryItemStream GeometryLogger::item_stream(const std::string& id) const {
    if (id.empty()) {
        return GeometryItemStream(generate_uuid(), stub_.get());
    }
    return GeometryItemStream(id, stub_.get());
}

} // namespace log
} // namespace gvs
