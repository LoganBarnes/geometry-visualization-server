// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/forward_declarations.hpp"

#include <gvs/scene.grpc.pb.h>

namespace gvs {
namespace host {

class SceneService : public gvs::proto::Scene::Service {
public:
    ~SceneService() override;

    grpc::Status add_item(grpc::ServerContext* context,
                          const gvs::proto::SceneItemInfo* request,
                          gvs::proto::SceneResponse* response) override;

    grpc::Status scene_updates(grpc::ServerContext* context,
                               const google::protobuf::Empty* request,
                               grpc::ServerWriter<gvs::proto::SceneUpdate>* writer) override;
};

} // namespace host
} // namespace gvs
