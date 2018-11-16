// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "test-util/test_server.hpp"
#include "test-util/test_service.hpp"
#include "test_server.hpp"

namespace gvs {
namespace test {

TestServer::TestServer(std::string server_address)
    : server_(server_address.empty()
                  ? std::make_unique<gvs::util::GrpcServer>(std::make_shared<gvs::test::TestService>())
                  : std::make_unique<gvs::util::GrpcServer>(std::move(server_address),
                                                            std::make_shared<gvs::test::TestService>()))
    , run_thread_([&] { server_->run(); }) {}

TestServer::~TestServer() {
    server_->shutdown();
    run_thread_.join();
}

std::shared_ptr<grpc::Channel> TestServer::local_channel() {
    grpc::ChannelArguments channel_arguments;
    return server_->server()->InProcessChannel(channel_arguments);
}

} // namespace test
} // namespace gvs
