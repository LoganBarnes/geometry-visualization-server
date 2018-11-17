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

#include "gvs/util/atomic_data.hpp"
#include "gvs/util/callback_handler.hpp"

#include <grpcpp/channel.h>
#include <grpcpp/completion_queue.h>

#include <thread>

//#define DEBUG_PRINT(msg) std::cout << (msg) << std::endl;
#define DEBUG_PRINT(msg)

inline ::std::string to_string(grpc_connectivity_state state) {
    switch (state) {
    case GRPC_CHANNEL_IDLE:
        /** channel is idle */
        return "GRPC_CHANNEL_IDLE";

    case GRPC_CHANNEL_CONNECTING:
        /** channel is connecting */
        return "GRPC_CHANNEL_CONNECTING";

    case GRPC_CHANNEL_READY:
        /** channel is ready for work */
        return "GRPC_CHANNEL_READY";

    case GRPC_CHANNEL_TRANSIENT_FAILURE:
        /** channel has seen a failure but expects to recover */
        return "GRPC_CHANNEL_TRANSIENT_FAILURE";

    case GRPC_CHANNEL_SHUTDOWN:
        /** channel has seen a failure that it cannot recover from */
        return "GRPC_CHANNEL_SHUTDOWN";
    }

    return "Invalid enum value";
}

inline ::std::ostream& operator<<(::std::ostream& os, grpc_connectivity_state state) {
    return os << to_string(state);
}

namespace gvs {
namespace net {

enum class GrpcClientState {
    not_connected,
    attempting_to_connect,
    connected,
};

inline std::string to_string(const GrpcClientState& state) {
    switch (state) {
    case GrpcClientState::not_connected:
        return "not_connected";
    case GrpcClientState::attempting_to_connect:
        return "attempting_to_connect";
    case GrpcClientState::connected:
        return "connected";
    }
    return "Unknown Enum value";
}

inline ::std::ostream& operator<<(::std::ostream& os, const GrpcClientState& state) {
    return os << to_string(state);
}

class GrpcClient {
public:
    explicit GrpcClient();
    ~GrpcClient();

    template <typename Callback, typename... Args>
    void change_server(std::shared_ptr<grpc::Channel> channel, Callback state_change_callback, Args&&... callback_args);

    void stop_connection_attempts();

    GrpcClientState get_state();

private:
    std::shared_ptr<grpc::Channel> channel_ = nullptr;

    util::AtomicData<GrpcClientState> state_;
    std::unique_ptr<grpc::CompletionQueue> connection_queue_ = nullptr;

    std::thread update_thread_;

    // function used by update_thread_
    void update(std::unique_ptr<util::CallbackInterface<void, const GrpcClientState&>> callback);
};

template <typename Callback, typename... Args>
void GrpcClient::change_server(std::shared_ptr<grpc::Channel> channel,
                               Callback state_change_callback,
                               Args&&... callback_args) {
    stop_connection_attempts();

    channel_ = std::move(channel);
    connection_queue_ = std::make_unique<grpc::CompletionQueue>();

    if (channel_->GetState(false) != GRPC_CHANNEL_READY) {
        state_.unsafe_data() = GrpcClientState::attempting_to_connect;

        update_thread_ = std::thread([=] {
            update(util::make_callback<void, const GrpcClientState&>(state_change_callback, callback_args...));
        });
    } else {
        state_.unsafe_data() = GrpcClientState::connected;
        util::apply(state_change_callback, std::make_tuple(std::forward<Args>(callback_args)...), state_.unsafe_data());
    }
}

} // namespace net
} // namespace gvs
