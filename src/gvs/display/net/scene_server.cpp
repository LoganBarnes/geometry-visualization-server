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

SceneServer::~SceneServer() = default; // shutdown server and wait?

grpc::Status SceneServer::AddItem(grpc::ServerContext* /*context*/,
                                  const gvs::net::SparseSceneItemInfo* request,
                                  gvs::net::SceneIdResult*             response) {
    auto&& info = from_proto(*request);

    auto result = display_scene_.use_safely([info = std::move(info)](display::DisplayScene& display_scene) mutable {
        return display_scene.actually_add_item(std::move(info));
    });
    if (result) {
        *response->mutable_value() = to_proto(result.value());
    } else {
        *response->mutable_error() = to_proto(result.error());
    }
    return grpc::Status::OK;
}

grpc::Status SceneServer::UpdateItem(grpc::ServerContext* /*context*/,
                                     const gvs::net::SparseSceneItemInfoWithId* request,
                                     gvs::net::Result*                          response) {
    auto   id   = from_proto(request->id());
    auto&& info = from_proto(request->info());

    auto error = display_scene_.use_safely([id, info = std::move(info)](display::DisplayScene& display_scene) mutable {
        return display_scene.actually_update_item(id, std::move(info));
    });

    if (error) {
        *response->mutable_error() = to_proto(error);
    } else {
        response->mutable_value(); // Success
    }
    return grpc::Status::OK;
}

grpc::Status SceneServer::AppendToItem(grpc::ServerContext* /*context*/,
                                       const gvs::net::SparseSceneItemInfoWithId* request,
                                       gvs::net::Result*                          response) {
    auto   id   = from_proto(request->id());
    auto&& info = from_proto(request->info());

    auto error = display_scene_.use_safely([id, info = std::move(info)](display::DisplayScene& display_scene) mutable {
        return display_scene.actually_append_to_item(id, std::move(info));
    });

    if (error) {
        *response->mutable_error() = to_proto(error);
    } else {
        response->mutable_value(); // Success
    }
    return grpc::Status::OK;
}

grpc::Status SceneServer::GetItemInfo(grpc::ServerContext* /*context*/,
                                      const gvs::net::SceneId*       request,
                                      gvs::net::SceneItemInfoResult* response) {
    auto id = from_proto(*request);

    auto error = display_scene_.use_safely([id, &response](display::DisplayScene& display_scene) {
        return display_scene.actually_get_item_info(id, [&response](gvs::SceneItemInfo const& info) {
            *response->mutable_value() = to_proto(info);
        });
    });

    if (error) {
        *response->mutable_error() = to_proto(error);
    }

    return grpc::Status::OK;
}

grpc::Status SceneServer::Clear(grpc::ServerContext* /*context*/,
                                const google::protobuf::Empty* /*request*/,
                                google::protobuf::Empty* /*response*/) {

    display_scene_.use_safely([](display::DisplayScene& display_scene) { display_scene.clear(); });
    return grpc::Status::OK;
}

grpc::Status SceneServer::GetItemIds(grpc::ServerContext* /*context*/,
                                     const google::protobuf::Empty* /*request*/,
                                     gvs::net::SceneIdList* response) {
    auto ids = display_scene_.use_safely([](display::DisplayScene& display_scene) { return display_scene.item_ids(); });
    *response = to_proto(ids);
    return grpc::Status::OK;
}

grpc::Status SceneServer::SetSeed(grpc::ServerContext* /*context*/,
                                  const gvs::net::Seed* request,
                                  google::protobuf::Empty* /*response*/) {
    display_scene_.use_safely(
        [seed = request->value()](display::DisplayScene& display_scene) { display_scene.set_seed(seed); });
    return grpc::Status::OK;
}

} // namespace gvs::net
