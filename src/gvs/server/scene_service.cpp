// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/server/scene_service.hpp"

namespace gvs {
namespace host {

SceneService::~SceneService() = default;

grpc::Status host::SceneService::add_item(grpc::ServerContext* /*context*/,
                                          const gvs::proto::SceneItemInfo* /*request*/,
                                          gvs::proto::SceneResponse* /*response*/) {
    return grpc::Status::OK;
}

grpc::Status host::SceneService::scene_updates(grpc::ServerContext* /*context*/,
                                               const google::protobuf::Empty* /*request*/,
                                               grpc::ServerWriter<gvs::proto::SceneUpdate>* /*writer*/) {
    return grpc::Status::OK;
}

} // namespace host
} // namespace gvs
