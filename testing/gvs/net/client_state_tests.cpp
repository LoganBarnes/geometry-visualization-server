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
#include "gvs/net/grpc_client.hpp"

#include <gmock/gmock.h>
#include <grpcpp/create_channel.h>

namespace {

test(ClientStateTests, GRPC_CHANNEL_CONNECTING_string) {
    std::stringstream ss;
    ss << grpc_connectivity_state::GRPC_CHANNEL_CONNECTING;
    EXPECT_EQ(ss.str(), "GRPC_CHANNEL_CONNECTING");
}

TEST(ClientStateTests, GRPC_CHANNEL_IDLE_string) {
    std::stringstream ss;
    ss << grpc_connectivity_state::GRPC_CHANNEL_IDLE;
    EXPECT_EQ(ss.str(), "GRPC_CHANNEL_IDLE");
}

TEST(ClientStateTests, GRPC_CHANNEL_READY_string) {
    std::stringstream ss;
    ss << grpc_connectivity_state::GRPC_CHANNEL_READY;
    EXPECT_EQ(ss.str(), "GRPC_CHANNEL_READY");
}

TEST(ClientStateTests, GRPC_CHANNEL_SHUTDOWN_string) {
    std::stringstream ss;
    ss << grpc_connectivity_state::GRPC_CHANNEL_SHUTDOWN;
    EXPECT_EQ(ss.str(), "GRPC_CHANNEL_SHUTDOWN");
}

TEST(ClientStateTests, GRPC_CHANNEL_TRANSIENT_FAILURE_string) {
    std::stringstream ss;
    ss << grpc_connectivity_state::GRPC_CHANNEL_TRANSIENT_FAILURE;
    EXPECT_EQ(ss.str(), "GRPC_CHANNEL_TRANSIENT_FAILURE");
}

TEST(ClientStateTests, invalid_grpc_connectivity_state_string) {
    std::stringstream ss;
    // Really have to do some shadily incorrect coding to cause this
    ss << static_cast<grpc_connectivity_state>(-1);
    EXPECT_EQ(ss.str(), "Invalid enum value");
}

TEST(ClientStateTests, GRPC_CHANNEL_CONNECTING_to_typed_state) {
    EXPECT_EQ(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_CONNECTING),
              gvs::net::GrpcClientState::attempting_to_connect);
}

TEST(ClientStateTests, GRPC_CHANNEL_IDLE_to_typed_state) {
    EXPECT_EQ(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_IDLE),
              gvs::net::GrpcClientState::not_connected);
}

TEST(ClientStateTests, GRPC_CHANNEL_READY_to_typed_state) {
    EXPECT_EQ(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_READY),
              gvs::net::GrpcClientState::connected);
}

TEST(ClientStateTests, GRPC_CHANNEL_SHUTDOWN_to_typed_state) {
    EXPECT_EQ(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_SHUTDOWN),
              gvs::net::GrpcClientState::not_connected);
}

TEST(ClientStateTests, GRPC_CHANNEL_TRANSIENT_FAILURE_to_typed_state) {
    EXPECT_EQ(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_TRANSIENT_FAILURE),
              gvs::net::GrpcClientState::attempting_to_connect);
}

TEST(ClientStateTests, invalid_grpc_connectivity_state_throws) {
    // Really have to do some shadily incorrect coding to cause this
    EXPECT_THROW(gvs::net::to_typed_state(static_cast<grpc_connectivity_state>(-1)), std::invalid_argument);
}

} // namespace
