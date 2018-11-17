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

#include "gvs/net/grpc_client.hpp"
#include "gvs/util/blocking_queue.hpp"
#include "gvs/util/callback_handler.hpp"

#include <grpcpp/create_channel.h>
#include <grpcpp/server.h>

namespace gvs {
namespace net {

template <typename Service>
class DualGrpcClient {
public:
    DualGrpcClient();

    template <typename UpdateCallback, typename... Args>
    explicit DualGrpcClient(UpdateCallback update_callback, Args&&... args);

    void set_inprocess_server(std::unique_ptr<grpc::Server>& server);
    void set_external_server(const std::string& server_address);

    void set_using_inprocess_channel(bool use_inprocess_channel, bool detail_force_internal_updates = false);
    void stop_connection_attempts();

    /**
     * If the state is GrpcClientState::attempting_to_connect, connection_attempt will be set to the
     * current attempt number. Otherwise it is set to -1 (if a pointer is provided).
     */
    GrpcClientState connection_state(int* connection_attempt = nullptr);
    bool using_inprocess_channel() const;
    const std::string& server_address() const;

    const std::unique_ptr<typename Service::Stub>& current_stub() const;
    std::unique_ptr<typename Service::Stub>& current_stub();

private:
    std::string server_address_ = "NOT SET";

    struct StateAndAttempt {
        GrpcClientState state;
        int connection_attempt;

        explicit StateAndAttempt(GrpcClientState s, int attempt = -1) : state(s), connection_attempt(attempt) {}
    };
    util::BlockingQueue<StateAndAttempt> current_state_;

    GrpcClient external_client_;

    bool using_inprocess_channel_ = false;

    std::shared_ptr<grpc::Channel> inprocess_channel_ = nullptr;
    std::unique_ptr<typename Service::Stub> inprocess_stub_ = nullptr;

    std::string external_address_ = "";
    std::shared_ptr<grpc::Channel> external_channel_ = nullptr;
    std::unique_ptr<typename Service::Stub> external_stub_ = nullptr;

    int connection_attempt_ = 0; ///< only used in external_state_change_callback (which is called from another thread)

    std::unique_ptr<util::CallbackInterface<void, const GrpcClientState&>> update_callback_;

    void external_state_change_callback(const GrpcClientState& state);
};

template <typename Service>
DualGrpcClient<Service>::DualGrpcClient() {
    current_state_.emplace_back(GrpcClientState::not_connected);
}

template <typename Service>
template <typename UpdateCallback, typename... Args>
DualGrpcClient<Service>::DualGrpcClient(UpdateCallback update_callback, Args&&... args) : DualGrpcClient<Service>() {
    update_callback_ = util::make_callback<void, const GrpcClientState&>(update_callback, std::forward<Args>(args)...);
}

template <typename Service>
void DualGrpcClient<Service>::set_inprocess_server(std::unique_ptr<grpc::Server>& server) {
    grpc::ChannelArguments channel_arguments;
    inprocess_channel_ = server->InProcessChannel(channel_arguments);

    set_using_inprocess_channel(/*use_inprocess_channel=*/true, /*detail_force_internal_updates=*/true);
}

template <typename Service>
void DualGrpcClient<Service>::set_external_server(const std::string& server_address) {
    external_address_ = server_address;
    set_using_inprocess_channel(/*use_inprocess_channel=*/false,
                                /*detail_force_internal_updates=*/true);
}

template <typename Service>
void DualGrpcClient<Service>::stop_connection_attempts() {
    external_stub_ = nullptr;
    external_channel_ = nullptr;
    external_client_.stop_connection_attempts();
    current_state_.emplace_back(GrpcClientState::not_connected);
}

template <typename Service>
void DualGrpcClient<Service>::set_using_inprocess_channel(bool use_inprocess_channel,
                                                          bool detail_force_internal_updates) {

    if (not detail_force_internal_updates and using_inprocess_channel_ == use_inprocess_channel) {
        return;
    }

    using_inprocess_channel_ = use_inprocess_channel;

    if (using_inprocess_channel_) {
        if (not inprocess_channel_) {
            throw std::runtime_error("An in-process server must be specified before it can be used");
        }

        if (external_channel_) {
            stop_connection_attempts();
        }

        server_address_ = "In-Process";

        current_state_.emplace_back(GrpcClientState::connected);

        update_callback_->invoke(current_state_.pop_all_but_most_recent().state);

    } else {
        server_address_ = external_address_;

        external_stub_ = nullptr;
        external_channel_ = grpc::CreateChannel(server_address_, grpc::InsecureChannelCredentials());
        external_stub_ = Service::NewStub(external_channel_);

        connection_attempt_ = 0;
        external_client_.change_server(external_channel_,
                                       &DualGrpcClient<Service>::external_state_change_callback,
                                       this);
    }
}

template <typename Service>
GrpcClientState DualGrpcClient<Service>::connection_state(int* connection_attempt) {
    auto data = current_state_.pop_all_but_most_recent();
    if (connection_attempt) {
        *connection_attempt = (data.state == GrpcClientState::attempting_to_connect ? data.connection_attempt : -1);
    }
    return data.state;
}

template <typename Service>
bool DualGrpcClient<Service>::using_inprocess_channel() const {
    return using_inprocess_channel_;
}

template <typename Service>
const std::string& DualGrpcClient<Service>::server_address() const {
    return server_address_;
}

template <typename Service>
const std::unique_ptr<typename Service::Stub>& DualGrpcClient<Service>::current_stub() const {
    if ((using_inprocess_channel_ and not inprocess_channel_)
        or (not using_inprocess_channel_ and not external_stub_)) {
        throw std::runtime_error("An appropriate stub has not been created");
    }

    if (using_inprocess_channel_) {
        return inprocess_stub_;
    }
    return external_stub_;
}

template <typename Service>
std::unique_ptr<typename Service::Stub>& DualGrpcClient<Service>::current_stub() {
    if ((using_inprocess_channel_ and not inprocess_channel_)
        or (not using_inprocess_channel_ and not external_stub_)) {
        throw std::runtime_error("An appropriate stub has not been created");
    }

    if (using_inprocess_channel_) {
        return inprocess_stub_;
    }
    return external_stub_;
}

template <typename Service>
void DualGrpcClient<Service>::external_state_change_callback(const GrpcClientState& state) {
    current_state_.emplace_back(state, ++connection_attempt_);

    if (update_callback_) {
        update_callback_->invoke(state);
    }
}

} // namespace net
} // namespace gvs
