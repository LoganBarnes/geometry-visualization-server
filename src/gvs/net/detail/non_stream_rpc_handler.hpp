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

#include "gvs/net/detail/async_rpc_handler_interface.hpp"
#include "gvs/net/detail/stream_rpc_handler.hpp"
#include "gvs/net/detail/tag.hpp"
#include "gvs/util/atomic_data.hpp"

#include <experimental/optional>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#ifdef DOCTEST_LIBRARY_INCLUDED
#include <testing/testing.grpc.pb.h>
#endif

namespace gvs {
namespace net {
namespace detail {

/**
 * @brief Handles an individual non-streaming gRPC call when a client requests this rpc
 * @tparam Request is the Protobuf request type
 * @tparam Response is the Protobuf response type
 */
template <typename Request, typename Response>
struct NonStreamRpcConnection {
    grpc::ServerContext context;
    Request request;
    grpc::ServerAsyncResponseWriter<Response> responder;

    NonStreamRpcConnection() : responder(&context) {}
};

/**
 * @brief Handles non-streaming gRPC responses for a single rpc call
 * @tparam Service is the gRPC service
 * @tparam Request is the Protobuf request type
 * @tparam Response is the Protobuf response type
 * @tparam Callback is the implementation of this rpc call (signature: <grpc::Status(const Request&, Response*)>)
 */
template <typename Service, typename Request, typename Response, typename Callback>
class NonStreamRpcHandler : public AsyncRpcHandlerInterface {
public:
    explicit NonStreamRpcHandler(Service& service,
                                 grpc::ServerCompletionQueue& server_queue,
                                 AsyncNoStreamFunc<Service, Request, Response> stream_func,
                                 Callback callback);

    ~NonStreamRpcHandler() override;

    /**
     * @see AsyncRpcHandlerInterface::activate_next()
     */
    void activate_next() override;

private:
    Service& service_; ///< The gRPC service with the RPC call this class is handling
    grpc::ServerCompletionQueue& server_queue_; ///< The queue that handles server updates
    AsyncNoStreamFunc<Service, Request, Response> stream_func_; ///< The service function used to update the queue
    Callback callback_; ///< The server specific implementation of this RPC call
    grpc::CompletionQueue queue_; ///< Internal queue used to handle responses (required for async api)

    /// All the data needed to handle the RPC call when a client make a request
    std::unique_ptr<NonStreamRpcConnection<Request, Response>> connection_;
};

template <typename Service, typename Request, typename Response, typename Callback>
NonStreamRpcHandler<Service, Request, Response, Callback>::NonStreamRpcHandler(
    Service& service,
    grpc::ServerCompletionQueue& server_queue,
    AsyncNoStreamFunc<Service, Request, Response> stream_func,
    Callback callback)
    : service_(service), server_queue_(server_queue), stream_func_(stream_func), callback_(std::move(callback)) {
    activate_next();
}

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-net] tests NonStreamRpcConnection") {
    using namespace gvs::test::proto;
    //    std::unique_ptr<AsyncRpcHandlerInterface> rpc_handle;
    // TODO: Make/use a test server
    //    rpc_handle = std::make_unique<NonStreamRpcHandler<Test::Service, TestMessage, TestMessage>>()
}
#endif

template <typename Service, typename Request, typename Response, typename Callback>
NonStreamRpcHandler<Service, Request, Response, Callback>::~NonStreamRpcHandler() {
    queue_.Shutdown();

    void* ignored_tag;
    bool call_ok;
    while (queue_.Next(&ignored_tag, &call_ok)) {
        assert(not call_ok);
    }
}

template <typename Service, typename Request, typename Response, typename Callback>
void NonStreamRpcHandler<Service, Request, Response, Callback>::activate_next() {
    if (connection_) {
        Response response;
        grpc::Status status = callback_(connection_->request, &response);
        connection_->responder.Finish(response, status, &response);

        void* recv_tag;
        bool call_ok;

        if (queue_.Next(&recv_tag, &call_ok)) {
            assert(call_ok);
            assert(recv_tag == &response);
        } else {
            assert(false);
        }
    }

    // Add a new connection that is waiting to be activated
    connection_ = std::make_unique<NonStreamRpcConnection<Request, Response>>();

    (service_.*stream_func_)(&connection_->context,
                             &connection_->request,
                             &connection_->responder,
                             &queue_,
                             &server_queue_,
                             this);
}

} // namespace detail
} // namespace net
} // namespace gvs
