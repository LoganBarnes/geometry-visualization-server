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

namespace gvs {
namespace net {

GrpcClient::GrpcClient() : state_(GrpcClientState::not_connected) {}

GrpcClient::~GrpcClient() {
    stop_connection_attempts();
}

void GrpcClient::stop_connection_attempts() {
    if (update_thread_.joinable()) {
        {
            state_.use_safely([this](auto& state) {
                state = GrpcClientState::not_connected;
                channel_ = nullptr;
                connection_queue_->Shutdown();
            });
        }
        update_thread_.join();
    }
}

GrpcClientState GrpcClient::get_state() {
    GrpcClientState state_copy;

    state_.use_safely([&](const auto& state) { state_copy = state; });

    return state_copy;
}

void GrpcClient::update(std::unique_ptr<util::CallbackInterface<void, const GrpcClientState&>> callback) {

    bool queue_ok = true;
    void* got_tag;

    do {
        state_.use_safely([&](auto& state) {
            DEBUG_PRINT("queue ok: " + std::to_string(queue_ok));

            if (queue_ok and channel_) {
                auto channel_state = channel_->GetState(true);
                DEBUG_PRINT("channel_state: " + std::to_string(channel_state));

                if (state == GrpcClientState::attempting_to_connect and channel_state == GRPC_CHANNEL_READY) {
                    state = GrpcClientState::connected;
                }

                callback->invoke(state);

                if (state == GrpcClientState::attempting_to_connect) {
                    // never wait more than 5 seconds
                    auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(15);
                    int tag = 1;
                    channel_->NotifyOnStateChange(channel_state,
                                                  deadline,
                                                  connection_queue_.get(),
                                                  reinterpret_cast<void*>(tag));
                }
            }
        });

    } while (connection_queue_->Next(&got_tag, &queue_ok));
}

} // namespace net
} // namespace gvs
