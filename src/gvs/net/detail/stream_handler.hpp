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

#include "gvs/net/detail/rpc_handler_interface.hpp"
#include "gvs/net/detail/tag.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/container_util.hpp"

#include <atomic>
#include <experimental/optional>
#include <thread>
#include <unordered_map>
#include <unordered_set>

namespace gvs {
namespace net {

template <typename Response>
class StreamInterface {
public:
    virtual ~StreamInterface() = 0;
    virtual bool write(const Response& update) = 0;
};

template <typename Response>
StreamInterface<Response>::~StreamInterface() = default;

namespace detail {

template <typename Request, typename Response>
struct StreamConnection {
    grpc::ServerContext context;
    Request request;
    grpc::ServerAsyncWriter<Response> responder;

    StreamConnection() : responder(&context) {}
};

template <typename Service, typename Request, typename Response, typename Callback>
class StreamHandler : public RpcHandlerInterface, public StreamInterface<Response> {
public:
    explicit StreamHandler(Service& service,
                           grpc::ServerCompletionQueue& server_queue,
                           AysncServerStreamFunc<Service, Request, Response> stream_func,
                           Callback callback);

    ~StreamHandler() override;

    void activate_next() override;
    bool write(const Response& update) override;

private:
    Service& service_;
    grpc::ServerCompletionQueue& server_queue_;
    AysncServerStreamFunc<Service, Request, Response> stream_func_;
    Callback callback_;

    struct Connections {
        std::unique_ptr<StreamConnection<Request, Response>> next = nullptr;
        std::unordered_map<void*, std::unique_ptr<StreamConnection<Request, Response>>> active = {};
        std::unordered_set<void*> processing = {};
        std::unordered_map<void*, std::unique_ptr<Tag>> tags;
    };

    gvs::util::AtomicData<Connections> connections_;

    grpc::CompletionQueue queue_;

    std::thread sync_thread_;

    void run_synchronization();
};

template <typename Service, typename Request, typename Response, typename Callback>
StreamHandler<Service, Request, Response, Callback>::StreamHandler(
    Service& service,
    grpc::ServerCompletionQueue& server_queue,
    AysncServerStreamFunc<Service, Request, Response> stream_func,
    Callback callback)
    : service_(service), server_queue_(server_queue), stream_func_(stream_func), callback_(std::move(callback)) {

    sync_thread_ = std::thread(&StreamHandler<Service, Request, Response, Callback>::run_synchronization, this);

    activate_next();
}

template <typename Service, typename Request, typename Response, typename Callback>
StreamHandler<Service, Request, Response, Callback>::~StreamHandler() {
    queue_.Shutdown();
    sync_thread_.join();
}

template <typename Service, typename Request, typename Response, typename Callback>
void StreamHandler<Service, Request, Response, Callback>::activate_next() {
    connections_.use_safely([this](Connections& connections) {
        if (connections.next) {
            callback_(connections.next->request);
            // 'next' is now an active connection
            void* key = connections.next.get();
            connections.active.emplace(key, std::move(connections.next));
            connections.next = nullptr; // just in case because the data was moved
        }

        // Add a new connection that is waiting to be activated
        connections.next = std::make_unique<StreamConnection<Request, Response>>();

        connections.next->context.AsyncNotifyWhenDone(
            detail::make_tag(connections.next.get(), TagLabel::done, &connections.tags));

        (service_.*stream_func_)(&connections.next->context,
                                 &connections.next->request,
                                 &connections.next->responder,
                                 &queue_,
                                 &server_queue_,
                                 this);
    });
}

template <typename Service, typename Request, typename Response, typename Callback>
bool StreamHandler<Service, Request, Response, Callback>::write(const Response& update) {

    auto previous_updates_processed = [](const Connections& connections) { return connections.processing.empty(); };

    bool notify;

    // Wait until all previous updates have finished processing
    connections_.wait_to_use_safely(previous_updates_processed, [&](Connections& connections) {
        notify = connections.active.empty();

        // Write the update to all active streams
        for (auto& active_pair : connections.active) {
            void* key = active_pair.first;
            std::unique_ptr<StreamConnection<Request, Response>>& connection = active_pair.second;

            connection->responder.Write(update, detail::make_tag(key, TagLabel::writing, &connections.tags));
            connections.processing.emplace(key); // mark as being processed
        }
    });

    if (notify) {
        connections_.notify_one();
    }

    return true;
}

template <typename Service, typename Request, typename Response, typename Callback>
void StreamHandler<Service, Request, Response, Callback>::run_synchronization() {

    void* recv_tag;
    bool call_ok;

    while (queue_.Next(&recv_tag, &call_ok)) {
        bool all_streams_processed = false;

        connections_.use_safely([&](Connections& connections) {
            Tag tag = detail::get_tag(recv_tag, &connections.tags);

            switch (tag.label) {

            case TagLabel::writing:
                // This tag is no longer being processed
                connections.processing.erase(tag.data);

                // Remove the stream if it is finished
                if (not call_ok) {
                    connections.active.erase(tag.data);
                }

                all_streams_processed = connections.processing.empty();
                break;

            case TagLabel::done:
                // If the stream is not being processed then delete it. Otherwise, it will
                // be deleted when the queue returns this tag because 'call_ok' will be false.
                if (not util::has_key(connections.processing, tag.data)) {
                    connections.active.erase(tag.data);
                    connections.processing.erase(tag.data);
                }
                break;
            }
        });

        // Allow the 'write' function to continue since all updates have been processed
        if (all_streams_processed) {
            connections_.notify_one();
        }
    }
}

} // namespace detail
} // namespace net
} // namespace gvs
