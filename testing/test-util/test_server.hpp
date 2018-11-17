// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/common/grpc_server.hpp"

#include <grpcpp/channel.h>

#include <memory>
#include <thread>

namespace gvs {
namespace test {

class TestServer {
public:
    explicit TestServer(std::string server_address = "");
    ~TestServer();

    std::shared_ptr<grpc::Channel> inprocess_channel();

private:
    std::unique_ptr<gvs::util::GrpcServer> server_;
    std::thread run_thread_;
};

} // namespace test
} // namespace gvs
