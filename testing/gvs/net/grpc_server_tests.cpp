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

#include <testing/testing.grpc.pb.h>

#include <gmock/gmock.h>
#include <grpcpp/create_channel.h>

namespace {

TEST(GrpcServerTests, run_testing_server_and_check_echo_rpc_call) {
    std::string server_address = "0.0.0.0:50050";
    gvs::test::TestServer server(server_address);

    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    auto stub = gvs::test::proto::Test::NewStub(channel);

    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    ASSERT_TRUE(status.ok());
    ASSERT_EQ(response.msg(), test_msg);
}

TEST(GrpcServerTests, run_inprocess_testing_server_and_check_echo_rpc_call) {
    gvs::test::TestServer server;

    auto stub = gvs::test::proto::Test::NewStub(server.inprocess_channel());

    std::string test_msg = "a1, 23kqv9 !F(VMas3982fj!#!#+(*@)(a assdaf;le 1342 asdw32nm";

    gvs::test::proto::TestMessage request = {};
    request.set_msg(test_msg);

    grpc::ClientContext context;
    gvs::test::proto::TestMessage response;

    grpc::Status status = stub->echo(&context, request, &response);

    ASSERT_TRUE(status.ok());
    ASSERT_EQ(response.msg(), test_msg);
}

} // namespace
