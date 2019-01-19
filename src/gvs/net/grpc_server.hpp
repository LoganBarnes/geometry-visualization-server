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
#pragma once

#include "gvs/forward_declarations.hpp"

#include <grpc++/server.h>

namespace gvs {
namespace net {

class GrpcServer {
public:
    explicit GrpcServer(std::shared_ptr<grpc::Service> service, const std::string& server_address = "");

    /**
     * @brief Blocks until server is shutdown and all rpc calls terminate
     */
    void run();

    template <typename TimePoint>
    void shutdown(const TimePoint& deadline);
    void shutdown();

    std::shared_ptr<grpc::Service>& service();
    const std::shared_ptr<grpc::Service>& service() const;

    // grpc::Server does not have const methods so a const version is not needed
    std::unique_ptr<grpc::Server>& server();

private:
    std::shared_ptr<grpc::Service> service_;
    std::unique_ptr<grpc::Server> server_;
};

template <typename TimePoint>
void GrpcServer::shutdown(const TimePoint& deadline) {
    server_->Shutdown(deadline);
}

} // namespace net
} // namespace gvs

// //////////////////////////////////////////////////////////////////////////////////// //
// ///////////////////////////////////  TESTING  ////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////// //
#ifdef DOCTEST_LIBRARY_INCLUDED
#include "testing/util/test_service.hpp"

#include <grpc++/create_channel.h>

#include <thread>

TEST_CASE("[gvs-net] shared_service_is_the_same") {
    std::string server_address = "0.0.0.0:50050";
    auto service = std::make_shared<gvs::test::TestService>();
    gvs::net::GrpcServer server(service, server_address);

    CHECK(server.service() == service);

    server.shutdown();
}

TEST_CASE("[gvs-net] const_service_can_be_used") {
    std::string server_address = "0.0.0.0:50050";
    gvs::net::GrpcServer server(std::make_shared<gvs::test::TestService>(), server_address);

    {
        const auto& const_server = server;

        CHECK_FALSE(const_server.service()->has_async_methods());
        CHECK_FALSE(const_server.service()->has_generic_methods());
        CHECK(const_server.service()->has_synchronous_methods());
    }

    server.shutdown();
}

TEST_CASE("[gvs-net] run_server_and_check_echo_rpc_call") {
    // Create a server and run it in a separate thread
    std::string server_address = "0.0.0.0:50050";
    gvs::net::GrpcServer server(std::make_shared<gvs::test::TestService>(), server_address);
    std::thread run_thread([&] { server.run(); });

    // Create a client to connect to the server
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    auto stub = gvs::test::proto::Test::NewStub(channel);

    // Send a message to the server
    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    // Check the server received the message and responded with the same message
    CHECK(status.ok());
    CHECK(response.msg() == test_msg);

    // Let the server exit
    auto deadline = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
    server.shutdown(deadline);
    run_thread.join();
}

TEST_CASE("[gvs-net] run_inprocess_server_and_check_echo_rpc_call") {

    // Create a server and run it in a separate thread
    std::string server_address = "0.0.0.0:50050";
    gvs::net::GrpcServer server(std::make_shared<gvs::test::TestService>(), server_address);
    std::thread run_thread([&] { server.run(); });

    // Create a client using the server's in-process channel
    auto stub = gvs::test::proto::Test::NewStub(server.server()->InProcessChannel(grpc::ChannelArguments{}));

    // Send a message to the server
    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    // Check the server received the message and responded with the same message
    CHECK(status.ok());
    CHECK(response.msg() == test_msg);

    // Let the server exit
    server.shutdown();
    run_thread.join();
}

#endif
