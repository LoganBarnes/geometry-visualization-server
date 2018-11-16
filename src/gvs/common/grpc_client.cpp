// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/common/grpc_client.hpp"

namespace gvs {
namespace util {

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

} // namespace util
} // namespace gvs
