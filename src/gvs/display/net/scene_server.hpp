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

// project
#include "gvs/display/display_scene.hpp"
#include "gvs/util/atomic_data.hpp"

// generated
#include <scene.grpc.pb.h>

// external
#include <grpcpp/server.h>

namespace gvs::net {

class SceneServer : public Scene::Service {
public:
    explicit SceneServer(std::string const& host_address = "");

    grpc::Status AddItem(grpc::ServerContext*                 context,
                         const gvs::net::SparseSceneItemInfo* request,
                         gvs::net::SceneIdResult*             response) override;

    grpc::Status UpdateItem(grpc::ServerContext*                       context,
                            const gvs::net::SparseSceneItemInfoWithId* request,
                            gvs::net::Result*                          response) override;

    grpc::Status AppendToItem(grpc::ServerContext*                       context,
                              const gvs::net::SparseSceneItemInfoWithId* request,
                              gvs::net::Result*                          response) override;

    grpc::Status GetItemInfo(grpc::ServerContext*           context,
                             const gvs::net::SceneId*       request,
                             gvs::net::SceneItemInfoResult* response) override;

    grpc::Status Clear(grpc::ServerContext*             context,
                       const ::google::protobuf::Empty* request,
                       ::google::protobuf::Empty*       response) override;

    grpc::Status GetItemIds(grpc::ServerContext*             context,
                            const ::google::protobuf::Empty* request,
                            gvs::net::SceneIdList*           response) override;

    grpc::Status
    SetSeed(grpc::ServerContext* context, const gvs::net::Seed* request, ::google::protobuf::Empty* response) override;

private:
    util::AtomicData<display::DisplayScene> display_scene_;
    std::unique_ptr<grpc::Server>           server_;
};

} // namespace gvs::net
