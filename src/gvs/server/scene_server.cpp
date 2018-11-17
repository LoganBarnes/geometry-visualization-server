// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/server/scene_server.hpp"
#include "gvs/server/scene_service.hpp"

namespace gvs {
namespace host {

SceneServer::SceneServer(std::string server_address)
    : server_(std::make_unique<gvs::net::GrpcServer>(std::make_shared<gvs::host::SceneService>(),
                                                     std::move(server_address)))
    , run_thread_([&] { server_->run(); }) {}

SceneServer::~SceneServer() {
    server_->shutdown();
    run_thread_.join();
}

std::shared_ptr<grpc::Channel> SceneServer::inprocess_channel() {
    grpc::ChannelArguments channel_arguments;
    return server_->server()->InProcessChannel(channel_arguments);
}

} // namespace host
} // namespace gvs
