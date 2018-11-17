// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/net/grpc_server.hpp"

#include <grpcpp/channel.h>

#include <memory>
#include <thread>

namespace gvs {
namespace host {

class SceneServer {
public:
    explicit SceneServer(std::string server_address = "");
    ~SceneServer();

    std::shared_ptr<grpc::Channel> inprocess_channel();

private:
    std::unique_ptr<gvs::net::GrpcServer> server_;
    std::thread run_thread_;
};

} // namespace host
} // namespace gvs
