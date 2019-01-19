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

#include "gvs/net/grpc_client_state.hpp"
#include "gvs/net/grpc_client_stream.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/callback_handler.hpp"

#include <grpc++/channel.h>
#include <grpc++/completion_queue.h>
#include <grpc++/create_channel.h>

#ifdef DOCTEST_LIBRARY_INCLUDED
#include <sstream>
#endif

#include <thread>
#include <unordered_map>

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

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-net] testing the grpc_connectivity_state string functions") {
    std::stringstream ss;

    SUBCASE("GRPC_CHANNEL_CONNECTING_string") {
        ss << grpc_connectivity_state::GRPC_CHANNEL_CONNECTING;
        CHECK(ss.str() == "GRPC_CHANNEL_CONNECTING");
    }

    SUBCASE("GRPC_CHANNEL_IDLE_string") {
        ss << grpc_connectivity_state::GRPC_CHANNEL_IDLE;
        CHECK(ss.str() == "GRPC_CHANNEL_IDLE");
    }

    SUBCASE("GRPC_CHANNEL_READY_string") {
        ss << grpc_connectivity_state::GRPC_CHANNEL_READY;
        CHECK(ss.str() == "GRPC_CHANNEL_READY");
    }

    SUBCASE("GRPC_CHANNEL_SHUTDOWN_string") {
        ss << grpc_connectivity_state::GRPC_CHANNEL_SHUTDOWN;
        CHECK(ss.str() == "GRPC_CHANNEL_SHUTDOWN");
    }

    SUBCASE("GRPC_CHANNEL_TRANSIENT_FAILURE_string") {
        ss << grpc_connectivity_state::GRPC_CHANNEL_TRANSIENT_FAILURE;
        CHECK(ss.str() == "GRPC_CHANNEL_TRANSIENT_FAILURE");
    }

    SUBCASE("invalid_grpc_connectivity_state_string") {
        // Really have to do some shadily incorrect coding to cause this
        ss << static_cast<grpc_connectivity_state>(-1);
        CHECK(ss.str() == "Invalid enum value");
    }
}
#endif

namespace gvs {
namespace net {

inline GrpcClientState to_typed_state(grpc_connectivity_state state) {
    switch (state) {
    case GRPC_CHANNEL_SHUTDOWN:
    case GRPC_CHANNEL_IDLE:
        return GrpcClientState::not_connected;

    case GRPC_CHANNEL_CONNECTING:
    case GRPC_CHANNEL_TRANSIENT_FAILURE:
        return GrpcClientState::attempting_to_connect;

    case GRPC_CHANNEL_READY:
        return GrpcClientState::connected;
    }

    throw std::invalid_argument("Invalid grpc_connectivity_state");
}

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-net] testing the grpc_connectivity_state string functions") {
    std::stringstream ss;

    SUBCASE("GRPC_CHANNEL_CONNECTING_to_typed_state") {
        CHECK(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_CONNECTING)
              == gvs::net::GrpcClientState::attempting_to_connect);
    }

    SUBCASE("GRPC_CHANNEL_IDLE_to_typed_state") {
        CHECK(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_IDLE)
              == gvs::net::GrpcClientState::not_connected);
    }

    SUBCASE("GRPC_CHANNEL_READY_to_typed_state") {
        CHECK(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_READY)
              == gvs::net::GrpcClientState::connected);
    }

    SUBCASE("GRPC_CHANNEL_SHUTDOWN_to_typed_state") {
        CHECK(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_SHUTDOWN)
              == gvs::net::GrpcClientState::not_connected);
    }

    SUBCASE("GRPC_CHANNEL_TRANSIENT_FAILURE_to_typed_state") {
        CHECK(gvs::net::to_typed_state(grpc_connectivity_state::GRPC_CHANNEL_TRANSIENT_FAILURE)
              == gvs::net::GrpcClientState::attempting_to_connect);
    }

    SUBCASE("invalid_grpc_connectivity_state_throws") {
        // Really have to do some shadily incorrect coding to cause this
        CHECK_THROWS_AS(gvs::net::to_typed_state(static_cast<grpc_connectivity_state>(-1)), std::invalid_argument);
    }
}
#endif

template <typename Service>
class GrpcClient {
public:
    explicit GrpcClient() = default;
    ~GrpcClient();

    template <typename ConnectionCallback, typename... Args>
    void change_server(std::string address, ConnectionCallback connection_change_callback, Args&&... callback_args);

    template <typename TimePoint>
    bool change_server_and_wait_for_connect(std::string address, const TimePoint& deadline);

    template <typename Return, typename InitFunc, typename Callback>
    void* register_stream(InitFunc init_func, Callback callback);

    void kill_streams_and_channel();

    const std::string& get_server_address() const;
    GrpcClientState get_state();

    /**
     * @brief Safely use the service stub to make RPC calls.
     *
     * If the client is not connected this function will return false and 'usage_func' will not be invoked
     */
    template <typename UsageFunc>
    bool use_stub(const UsageFunc& usage_func);

private:
    struct SharedData {
        grpc_connectivity_state connection_state = GRPC_CHANNEL_IDLE;
        std::shared_ptr<grpc::Channel> channel = nullptr;
        std::unique_ptr<typename Service::Stub> stub = nullptr;
        std::unordered_map<void*, std::unique_ptr<GrpcClientStreamInterface<Service>>> streams;
    };

    std::string server_address_;
    util::AtomicData<SharedData> shared_data_;

    void* state_change_tag = reinterpret_cast<void*>(0x1);

    // These are resused after calling 'Shutdown' and 'join' respectively.
    std::unique_ptr<grpc::CompletionQueue> queue_;
    std::unique_ptr<std::thread> run_thread_;

    void run(std::unique_ptr<util::CallbackInterface<void, const GrpcClientState&>> connection_change_callback);
};

template <typename Service>
GrpcClient<Service>::~GrpcClient() {
    kill_streams_and_channel();
}

template <typename Service>
template <typename ConnectionCallback, typename... Args>
void GrpcClient<Service>::change_server(std::string address,
                                        ConnectionCallback connection_change_callback,
                                        Args&&... callback_args) {
    kill_streams_and_channel();

    server_address_ = std::move(address);
    queue_ = std::make_unique<grpc::CompletionQueue>();

    bool state_changed = false;
    GrpcClientState typed_state;

    shared_data_.use_safely([&](SharedData& data) {
        data.channel = grpc::CreateChannel(server_address_, grpc::InsecureChannelCredentials());
        data.stub = Service::NewStub(data.channel);

        // Get the current state and check if it has changed
        auto new_state = data.channel->GetState(true);

        typed_state = to_typed_state(new_state);
        state_changed = (typed_state != to_typed_state(data.connection_state));

        data.connection_state = new_state;

        auto deadline = std::chrono::system_clock::now() + std::chrono::seconds(15);
        data.channel->NotifyOnStateChange(data.connection_state, deadline, queue_.get(), state_change_tag);
    });

    if (state_changed) {
        util::invoke(connection_change_callback, std::forward<Args>(callback_args)..., typed_state);
    }

    run_thread_
        = std::make_unique<std::thread>(&GrpcClient<Service>::run,
                                        this,
                                        util::make_callback<void, const GrpcClientState&>(connection_change_callback,
                                                                                          std::forward<Args>(
                                                                                              callback_args)...));
}

template <typename Service>
template <typename TimePoint>
bool GrpcClient<Service>::change_server_and_wait_for_connect(std::string address, const TimePoint& deadline) {
    kill_streams_and_channel();

    server_address_ = std::move(address);
    queue_ = std::make_unique<grpc::CompletionQueue>();

    bool connected;

    shared_data_.use_safely([&](SharedData& data) {
        data.channel = grpc::CreateChannel(server_address_, grpc::InsecureChannelCredentials());
        data.stub = Service::NewStub(data.channel);

        connected = data.channel->WaitForConnected(deadline);
        data.connection_state = data.channel->GetState(false);
    });

    return connected;
}

template <typename Service>
template <typename Return, typename InitFunc, typename Callback>
void* GrpcClient<Service>::register_stream(InitFunc init_func, Callback callback) {
    void* key;

    shared_data_.use_safely([=, &key](SharedData& data) {
        auto stream = std::make_unique<GrpcClientStream<Service, Return, InitFunc, Callback>>(init_func, callback);

        // Start the stream if the channel is already connected
        if (data.channel and data.connection_state == GRPC_CHANNEL_READY) {
            stream->start_stream(data.stub);
        }

        key = stream.get();
        data.streams.emplace(key, std::move(stream));
    });

    return key;
}

template <typename Service>
void GrpcClient<Service>::kill_streams_and_channel() {
    shared_data_.use_safely([](SharedData& data) {
        for (auto& stream_pair : data.streams) {
            stream_pair.second->stop_stream();
        }

        data.stub = nullptr;
        data.channel = nullptr;
    });

    if (queue_) {
        queue_->Shutdown();
    }

    if (run_thread_ and run_thread_->joinable()) {
        run_thread_->join();
    }
}

template <typename Service>
const std::string& GrpcClient<Service>::get_server_address() const {
    return server_address_;
}

template <typename Service>
GrpcClientState GrpcClient<Service>::get_state() {
    grpc_connectivity_state state_copy;
    shared_data_.use_safely([&](const SharedData& data) { state_copy = data.connection_state; });
    return to_typed_state(state_copy);
}

template <typename Service>
template <typename UsageFunc>
bool GrpcClient<Service>::use_stub(const UsageFunc& usage_func) {
    bool stub_valid = false;

    shared_data_.use_safely([&](const SharedData& data) {
        if (data.connection_state == GRPC_CHANNEL_READY) {
            stub_valid = true;
            usage_func(data.stub);
        }
    });

    return stub_valid;
}

template <typename Service>
void GrpcClient<Service>::run(
    std::unique_ptr<util::CallbackInterface<void, const GrpcClientState&>> connection_change_callback) {

    GrpcClientState typed_state;
    grpc::Status status;

    void* current_tag;
    bool result_ok;

    while (queue_->Next(&current_tag, &result_ok)) {
        assert(current_tag == state_change_tag);

        bool state_changed = false;

        shared_data_.use_safely([&](SharedData& data) {
            if (data.channel) { // not shutdown yet

                if (result_ok) {
                    auto old_state = to_typed_state(data.connection_state);
                    data.connection_state = data.channel->GetState(true);
                    typed_state = to_typed_state(data.connection_state);

                    state_changed = (typed_state != old_state);

                    if (state_changed) {
                        if (data.connection_state == GRPC_CHANNEL_READY) {
                            for (auto& stream_pair : data.streams) {
                                stream_pair.second->start_stream(data.stub);
                            }
                        } else {
                            for (auto& stream_pair : data.streams) {
                                stream_pair.second->stop_stream();
                            }
                        }
                    }
                }

                auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::seconds(60);
                data.channel->NotifyOnStateChange(data.connection_state, deadline, queue_.get(), state_change_tag);

            } else if (to_typed_state(data.connection_state) != GrpcClientState::not_connected) {
                data.connection_state = GRPC_CHANNEL_SHUTDOWN;
                typed_state = to_typed_state(data.connection_state);
                state_changed = true;
            }
        });

        // Do the callback outside the locked code to prevent the user from deadlocking the program.
        if (state_changed) {
            connection_change_callback->invoke(typed_state);
        }
    }
}

} // namespace net
} // namespace gvs
