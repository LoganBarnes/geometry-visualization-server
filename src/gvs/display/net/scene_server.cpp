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
#include "scene_server.hpp"

// project
#include "gvs/net/type_conversions.hpp"

// external
#include <grpcpp/server_builder.h>

namespace gvs::net {

SceneServer::SceneServer(const std::string& host_address) {
    grpc::ServerBuilder builder;

    builder.RegisterService(this);
    builder.SetMaxMessageSize(std::numeric_limits<int>::max());

    if (!host_address.empty()) {
        builder.AddListeningPort(host_address, grpc::InsecureServerCredentials());
    }

    // Prevent multiple servers from running at the same address with the same port?
    builder.AddChannelArgument("grpc.so_reuseport", 0);

    server_ = builder.BuildAndStart();
}

grpc::Status SceneServer::AddItem(grpc::ServerContext*                 context,
                                  const gvs::net::SparseSceneItemInfo* request,
                                  gvs::net::SceneIdResult*             response) {
    return Service::AddItem(context, request, response);
}

grpc::Status SceneServer::UpdateItem(grpc::ServerContext*                       context,
                                     const gvs::net::SparseSceneItemInfoWithId* request,
                                     gvs::net::Result*                          response) {
    return Service::UpdateItem(context, request, response);
}

grpc::Status SceneServer::AppendToItem(grpc::ServerContext*                       context,
                                       const gvs::net::SparseSceneItemInfoWithId* request,
                                       gvs::net::Result*                          response) {
    return Service::AppendToItem(context, request, response);
}

grpc::Status SceneServer::GetItemInfo(grpc::ServerContext*           context,
                                      const gvs::net::SceneId*       request,
                                      gvs::net::SceneItemInfoResult* response) {
    return Service::GetItemInfo(context, request, response);
}

grpc::Status SceneServer::Clear(grpc::ServerContext*             context,
                                const ::google::protobuf::Empty* request,
                                ::google::protobuf::Empty*       response) {
    return Service::Clear(context, request, response);
}

grpc::Status SceneServer::GetItemIds(grpc::ServerContext*             context,
                                     const ::google::protobuf::Empty* request,
                                     gvs::net::SceneIdList*           response) {
    return Service::GetItemIds(context, request, response);
}

grpc::Status
SceneServer::SetSeed(grpc::ServerContext* context, const gvs::net::Seed* request, ::google::protobuf::Empty* response) {
    return Service::SetSeed(context, request, response);
}

} // namespace gvs::net
