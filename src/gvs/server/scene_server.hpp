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

#include "gvs/net/grpc_async_server.hpp"
#include <crossguid/guid.hpp>

#include <gvs/scene.grpc.pb.h>

#include <thread>

namespace gvs {
namespace host {

class SceneService;

class SceneServer {
public:
    explicit SceneServer(std::string server_address = "");

    grpc::Server& server();

private:
    using Service = proto::Scene::AsyncService;
    net::GrpcAsyncServer<Service> server_;

    // atomicize these
    proto::SceneItems scene_;
    proto::Messages messages_;

    gvs::net::StreamInterface<proto::Message>* message_stream_;
    gvs::net::StreamInterface<proto::SceneUpdate>* scene_stream_;

    /*
     * How items are handled based on the update request:
     *
     * | Request Type   | Contains Geometry | Item Already Exists             | Item Does Not Exist |
     * | -------------- |:-----------------:| ------------------------------- | ------------------- |
     * | `gvs::send`    |      **Yes**      | **Error**                       | Creates new item    |
     * | `gvs::send`    |       *No*        | Updates item                    | **Error**           |
     * | `gvs::replace` |      **Yes**      | Replaces existing geometry*     | Creates new item    |
     * | `gvs::replace` |       *No*        | Updates item                    | **Error**           |
     * | `gvs::append`  |      **Yes**      | Appends positions to geometry** | Creates new item    |
     * | `gvs::append`  |       *No*        | Updates item                    | **Error**           |
     */

    grpc::Status safe_set_item(const proto::SceneItemInfo& info, proto::Errors* errors);
    grpc::Status replace_item(const proto::SceneItemInfo& info, proto::Errors* errors);
    grpc::Status append_to_item(const proto::SceneItemInfo& info, proto::Errors* errors);
};

} // namespace host
} // namespace gvs
