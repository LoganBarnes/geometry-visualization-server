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
#include "test-util/scene_server.hpp"
#include "test-util/test_service.hpp"
#include "scene_server.hpp"

namespace gvs {
namespace test {

TestServer::TestServer(std::string server_address)
    : server_(
          std::make_unique<gvs::net::GrpcServer>(std::make_shared<gvs::test::TestService>(), std::move(server_address)))
    , run_thread_([&] { server_->run(); }) {}

TestServer::~TestServer() {
    server_->shutdown();
    run_thread_.join();
}

std::shared_ptr<grpc::Channel> TestServer::inprocess_channel() {
    grpc::ChannelArguments channel_arguments;
    return server_->server()->InProcessChannel(channel_arguments);
}

} // namespace test
} // namespace gvs
