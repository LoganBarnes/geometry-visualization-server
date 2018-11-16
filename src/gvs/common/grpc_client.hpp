// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/common/apply.hpp"
#include "gvs/common/atomic_data.hpp"

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
namespace util {

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

    AtomicData<GrpcClientState> state_;
    std::unique_ptr<grpc::CompletionQueue> connection_queue_ = nullptr;

    std::thread update_thread_;

    template <typename Callback, typename... Args>
    void update(Callback state_change_callback, Args&&... callback_args); // function used by update_thread_
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

        update_thread_ = std::thread([=] { update(state_change_callback, callback_args...); });
    } else {
        state_.unsafe_data() = GrpcClientState::connected;
        apply(state_change_callback, std::make_tuple(std::forward<Args>(callback_args)...), state_.unsafe_data());
    }
}

template <typename Callback, typename... Args>
void GrpcClient::update(Callback state_change_callback, Args&&... callback_args) {

    auto args = std::make_tuple(std::forward<Args>(callback_args)...);
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

                apply(state_change_callback, args, state);

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

} // namespace util
} // namespace gvs
