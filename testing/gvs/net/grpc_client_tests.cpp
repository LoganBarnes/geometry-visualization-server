// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "test-util/test_service.hpp"
#include "test-util/scene_server.hpp"

#include "gvs/net/grpc_client.hpp"
#include "gvs/net/grpc_server.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/blocking_queue.hpp"

#include <grpcpp/create_channel.h>

#include <gmock/gmock.h>

namespace {

class GrpcClientTests : public testing::Test {
public:
    void handle_state_change(gvs::net::GrpcClientState state) {
        DEBUG_PRINT(state);
        state_queue.push_back(state);
    }

protected:
    gvs::util::BlockingQueue<gvs::net::GrpcClientState> state_queue;
};

TEST_F(GrpcClientTests, no_server) {
    std::string address = "0.0.0.0:50050";

    {
        gvs::net::GrpcClient client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()),
                             &GrpcClientTests::handle_state_change,
                             this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
    }
}

TEST_F(GrpcClientTests, no_server_stop_connection_attempts) {
    std::string address = "0.0.0.0:50051";

    {
        gvs::net::GrpcClient client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()),
                             &GrpcClientTests::handle_state_change,
                             this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);

        client.stop_connection_attempts();

        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);
    }
}

TEST_F(GrpcClientTests, delayed_server) {
    std::string server_address = "0.0.0.0:50052";

    {
        gvs::net::GrpcClient client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()),
                             &GrpcClientTests::handle_state_change,
                             this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);

        gvs::test::TestServer server(server_address);

        // The client should connect to the new server within a few attempts but just
        // in case we set a maximum number of attempts to avoid an infinite while loop
        int attempts = 0;
        int max_attempts = 5;
        gvs::net::GrpcClientState state = state_queue.pop_front();

        while (state != gvs::net::GrpcClientState::connected and attempts++ < max_attempts) {
            EXPECT_EQ(state, gvs::net::GrpcClientState::attempting_to_connect);
            state = state_queue.pop_front();
        }

        EXPECT_LT(attempts, max_attempts);
        EXPECT_EQ(state, gvs::net::GrpcClientState::connected);
        EXPECT_TRUE(state_queue.empty());
    }
}

TEST_F(GrpcClientTests, external_server) {
    std::string server_address = "0.0.0.0:50053";

    gvs::test::TestServer server(server_address);

    {
        gvs::net::GrpcClient client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()),
                             &GrpcClientTests::handle_state_change,
                             this);

        // The client should connect to the server within a few attempts but just in
        // case we set a maximum number of attempts to avoid an infinite while loop
        int attempts = 0;
        int max_attempts = 5;
        gvs::net::GrpcClientState state = state_queue.pop_front();
        do {
            EXPECT_EQ(state, gvs::net::GrpcClientState::attempting_to_connect);
            state = state_queue.pop_front();
        } while (state != gvs::net::GrpcClientState::connected and attempts++ < max_attempts);

        EXPECT_LT(attempts, max_attempts);
        EXPECT_EQ(state, gvs::net::GrpcClientState::connected);
        EXPECT_TRUE(state_queue.empty());
    }
}

TEST_F(GrpcClientTests, mutiple_channels) {
    std::string server_address1 = "0.0.0.0:50054";
    std::string server_address2 = "0.0.0.0:50055";

    gvs::test::TestServer server1(server_address1);

    {
        gvs::net::GrpcClient client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        auto channel1 = grpc::CreateChannel(server_address1, grpc::InsecureChannelCredentials());
        client.change_server(channel1, &GrpcClientTests::handle_state_change, this);

        // The client should connect to the server within a few attempts but just in
        // case we set a maximum number of attempts to avoid an infinite while loop
        int attempts = 0;
        int max_attempts = 5;
        gvs::net::GrpcClientState state = state_queue.pop_front();

        do {
            EXPECT_EQ(state, gvs::net::GrpcClientState::attempting_to_connect);
            state = state_queue.pop_front();
        } while (state != gvs::net::GrpcClientState::connected and attempts++ < max_attempts);

        EXPECT_LT(attempts, max_attempts);
        EXPECT_EQ(state, gvs::net::GrpcClientState::connected); // Connected to server 1
        EXPECT_TRUE(state_queue.empty());

        gvs::test::TestServer server2(server_address2);

        // Connect to server 2
        auto channel2 = grpc::CreateChannel(server_address2, grpc::InsecureChannelCredentials());
        client.change_server(channel2, &GrpcClientTests::handle_state_change, this);

        // The client should connect to the server within a few attempts but just in
        // case we set a maximum number of attempts to avoid an infinite while loop
        attempts = 0;
        max_attempts = 5;
        state = state_queue.pop_front();

        do {
            EXPECT_EQ(state, gvs::net::GrpcClientState::attempting_to_connect);
            state = state_queue.pop_front();
        } while (state != gvs::net::GrpcClientState::connected and attempts++ < max_attempts);

        EXPECT_LT(attempts, max_attempts);
        EXPECT_EQ(state, gvs::net::GrpcClientState::connected); // Connected to server 2
        EXPECT_TRUE(state_queue.empty());

        // Reconnect to server 1
        client.change_server(channel1, &GrpcClientTests::handle_state_change, this);

        // Should be an instant connection since we haven't deleted the channel since we last connected
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::connected);
    }
}

} // namespace
