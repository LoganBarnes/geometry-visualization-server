// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "test-util/test_service.hpp"
#include "test-util/test_server.hpp"

#include "gvs/common/dual_grpc_client.hpp"
#include "gvs/common/grpc_server.hpp"
#include "gvs/common/atomic_data.hpp"
#include "gvs/common/blocking_queue.hpp"

#include <grpcpp/create_channel.h>

#include <gmock/gmock.h>

#if 0

namespace {

class DualGrpcClientTests : public testing::Test {
public:
    void handle_state_change(gvs::util::GrpcClientState state) {
        DEBUG_PRINT(state);
        state_queue.push_back(state);
    }

protected:
    gvs::util::BlockingQueue<gvs::util::GrpcClientState> state_queue;
};

TEST_F(DualGrpcClientTests, no_server) {
    std::string address = "0.0.0.0:50050";

    {
        gvs::util::DualGrpcClient<gvs::test::proto::Test> client(&DualGrpcClientTests::handle_state_change, this);
        EXPECT_EQ(client.connection_state(), gvs::util::GrpcClientState::not_connected);

        client.set_external_server(address);

        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
    }
}

//TEST_F(DualGrpcClientTests, no_server_stop_connection_attempts) {
//    std::string address = "0.0.0.0:50051";
//
//    {
//        gvs::util::GrpcClient client;
//        EXPECT_EQ(client.get_state(), gvs::util::GrpcClientState::not_connected);
//
//        auto channel = grpc::CreateChannel(address, grpc::InsecureChannelCredentials());
//        client.change_server(channel, &DualGrpcClientTests::handle_state_change, this);
//
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//
//        client.stop_connection_attempts();
//
//        EXPECT_EQ(client.get_state(), gvs::util::GrpcClientState::not_connected);
//    }
//}
//
//TEST_F(DualGrpcClientTests, delayed_server) {
//    std::string server_address = "0.0.0.0:50052";
//
//    {
//        gvs::util::GrpcClient client;
//        EXPECT_EQ(client.get_state(), gvs::util::GrpcClientState::not_connected);
//
//        auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
//        client.change_server(channel, &DualGrpcClientTests::handle_state_change, this);
//
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//
//        gvs::test::TestServer server(server_address);
//
//        // The client should connect to the new server within a few attempts but just
//        // in case we set a maximum number of attempts to avoid an infinite while loop
//        int attempts = 0;
//        int max_attempts = 5;
//        while (state_queue.wait_front() != gvs::util::GrpcClientState::connected and attempts++ < max_attempts) {
//            EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        }
//
//        EXPECT_LE(attempts, max_attempts);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::connected);
//    }
//}
//
//TEST_F(DualGrpcClientTests, external_server) {
//    std::string server_address = "0.0.0.0:50053";
//
//    gvs::test::TestServer server(server_address);
//
//    {
//        gvs::util::GrpcClient client;
//        EXPECT_EQ(client.get_state(), gvs::util::GrpcClientState::not_connected);
//
//        auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
//        client.change_server(channel, &DualGrpcClientTests::handle_state_change, this);
//
//        // The client should connect to the server within a few attempts but just in
//        // case we set a maximum number of attempts to avoid an infinite while loop
//        int attempts = 0;
//        int max_attempts = 5;
//        do {
//            EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        } while (state_queue.wait_front() != gvs::util::GrpcClientState::connected and attempts++ < max_attempts);
//
//        EXPECT_LE(attempts, max_attempts);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::connected);
//    }
//}
//
//TEST_F(DualGrpcClientTests, mutiple_channels) {
//    std::string server_address1 = "0.0.0.0:50054";
//    std::string server_address2 = "0.0.0.0:50055";
//
//    gvs::test::TestServer server1(server_address1);
//
//    {
//        gvs::util::GrpcClient client;
//        EXPECT_EQ(client.get_state(), gvs::util::GrpcClientState::not_connected);
//
//        auto channel1 = grpc::CreateChannel(server_address1, grpc::InsecureChannelCredentials());
//        client.change_server(channel1, &DualGrpcClientTests::handle_state_change, this);
//
//        // The client should connect to the server within a few attempts but just in
//        // case we set a maximum number of attempts to avoid an infinite while loop
//        int attempts = 0;
//        int max_attempts = 5;
//        do {
//            EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        } while (state_queue.wait_front() != gvs::util::GrpcClientState::connected and attempts++ < max_attempts);
//
//        EXPECT_LE(attempts, max_attempts);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::connected); // Connected to server 1
//
//        gvs::test::TestServer server2(server_address2);
//
//        // Connect to server 2
//        auto channel2 = grpc::CreateChannel(server_address2, grpc::InsecureChannelCredentials());
//        client.change_server(channel2, &DualGrpcClientTests::handle_state_change, this);
//
//        // The client should connect to the server within a few attempts but just in
//        // case we set a maximum number of attempts to avoid an infinite while loop
//        attempts = 0;
//        max_attempts = 5;
//        do {
//            EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::attempting_to_connect);
//        } while (state_queue.wait_front() != gvs::util::GrpcClientState::connected and attempts++ < max_attempts);
//
//        EXPECT_LE(attempts, max_attempts);
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::connected); // Connected to server 2
//
//        // Reconnect to server 1
//        client.change_server(channel1, &DualGrpcClientTests::handle_state_change, this);
//
//        // Should be an instant connection since we haven't deleted the channel since we last connected
//        EXPECT_EQ(state_queue.pop_front(), gvs::util::GrpcClientState::connected);
//    }
//}

} // namespace

#endif
