// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Proto Scene Project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "test-util/scene_server.hpp"
#include <testing/testing.grpc.pb.h>
#include <grpcpp/create_channel.h>
#include <gmock/gmock.h>

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

    stub->echo(&context, request, &response);

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

    stub->echo(&context, request, &response);

    ASSERT_EQ(response.msg(), test_msg);
}

} // namespace
