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
#include "test-util/test_service.hpp"
#include "test-util/scene_server.hpp"

#include "gvs/net/grpc_client.hpp"
#include "gvs/net/grpc_server.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/blocking_queue.hpp"

#include <grpcpp/create_channel.h>

#include <gmock/gmock.h>

namespace {

void check_connects(gvs::util::BlockingQueue<gvs::net::GrpcClientState>& queue) {
    gvs::net::GrpcClientState state = queue.pop_front();

    // The client may enter an 'attempting_to_connect' state before connecting but
    // many times it will connect immediately since the server is hosted locally.
    if (state == gvs::net::GrpcClientState::attempting_to_connect) {
        EXPECT_EQ(queue.pop_front(), gvs::net::GrpcClientState::connected);

    } else {
        EXPECT_EQ(state, gvs::net::GrpcClientState::connected);
    }
}

class GrpcClientTests : public testing::Test {
public:
    void handle_state_change(gvs::net::GrpcClientState state) { state_queue.push_back(state); }

protected:
    gvs::util::BlockingQueue<gvs::net::GrpcClientState> state_queue;
};

TEST_F(GrpcClientTests, no_server) {
    std::string address = "0.0.0.0:50050";

    {
        gvs::net::GrpcClient<gvs::test::proto::Test> client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(address, &GrpcClientTests::handle_state_change, this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
    }

    EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
    EXPECT_TRUE(state_queue.empty());
}

TEST_F(GrpcClientTests, no_server_stop_connection_attempts) {
    std::string address = "0.0.0.0:50051";

    {
        gvs::net::GrpcClient<gvs::test::proto::Test> client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(address, &GrpcClientTests::handle_state_change, this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);

        client.kill_streams_and_channel();

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
    }

    EXPECT_TRUE(state_queue.empty());
}

TEST_F(GrpcClientTests, delayed_server) {
    std::string server_address = "0.0.0.0:50052";

    {
        gvs::net::GrpcClient<gvs::test::proto::Test> client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(server_address, &GrpcClientTests::handle_state_change, this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);

        {
            gvs::test::TestServer server(server_address);
            EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::connected);
        }
        // Server is deleted
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::attempting_to_connect);
    }
    // Client is deleted
    EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
    EXPECT_TRUE(state_queue.empty());
}

TEST_F(GrpcClientTests, external_server) {
    std::string server_address = "0.0.0.0:50053";

    gvs::test::TestServer server(server_address);

    {
        gvs::net::GrpcClient<gvs::test::proto::Test> client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(server_address, &GrpcClientTests::handle_state_change, this);

        check_connects(state_queue);
    }

    EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
    EXPECT_TRUE(state_queue.empty());
}

TEST_F(GrpcClientTests, mutiple_channels) {
    std::string server_address1 = "0.0.0.0:50054";
    std::string server_address2 = "0.0.0.0:50055";

    gvs::test::TestServer server1(server_address1);

    {
        gvs::net::GrpcClient<gvs::test::proto::Test> client;
        EXPECT_EQ(client.get_state(), gvs::net::GrpcClientState::not_connected);

        client.change_server(server_address1, &GrpcClientTests::handle_state_change, this);

        check_connects(state_queue);

        gvs::test::TestServer server2(server_address2);

        // Connect to server 2
        client.change_server(server_address2, &GrpcClientTests::handle_state_change, this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
        check_connects(state_queue);

        // Reconnect to server 1
        client.change_server(server_address1, &GrpcClientTests::handle_state_change, this);

        EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
        check_connects(state_queue);
    }
    // Client is deleted.
    EXPECT_EQ(state_queue.pop_front(), gvs::net::GrpcClientState::not_connected);
    EXPECT_TRUE(state_queue.empty());
}

} // namespace
