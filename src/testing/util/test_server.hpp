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

#include "gvs/net/grpc_server.hpp"

#include <grpc++/channel.h>

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
    std::unique_ptr<gvs::net::GrpcServer> server_;
    std::thread                           run_thread_;
};

} // namespace test
} // namespace gvs

// //////////////////////////////////////////////////////////////////////////////////// //
// ///////////////////////////////////  TESTING  ////////////////////////////////////// //
// //////////////////////////////////////////////////////////////////////////////////// //
#ifdef DOCTEST_LIBRARY_INCLUDED

TEST_CASE("[gvs-test-util] run_test_server_and_check_echo_rpc_call") {
    // Create a server and run it in a separate thread
    std::string           server_address = "0.0.0.0:50050";
    gvs::test::TestServer server(server_address);

    // Create a client to connect to the server
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    auto stub    = gvs::test::proto::Test::NewStub(channel);

    // Send a message to the server
    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext           context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    // Check the server recieved the message and responded with the same message
    CHECK(status.ok());
    CHECK(response.msg() == test_msg);
}

TEST_CASE("[gvs-test-util] run_inprocess_test_server_and_check_echo_rpc_call") {

    // Create a server and run it in a separate thread
    gvs::test::TestServer server;

    // Create a client using the server's in-process channel
    auto stub = gvs::test::proto::Test::NewStub(server.inprocess_channel());

    // Send a message to the server
    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext           context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    // Check the server recieved the message and responded with the same message
    CHECK(status.ok());
    CHECK(response.msg() == test_msg);
}

#endif
