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
#include "gvs/net/detail/stream_handler.hpp"
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

template <typename Request, typename Response>
struct AsyncConnection {
    grpc::ServerContext context;
    Request request;
    grpc::ServerAsyncResponseWriter<Response> responder;

    AsyncConnection() : responder(&context) {}
};

template <typename Service, typename Request, typename Response, typename Callback>
class AsyncHandler : public RpcHandlerInterface {
public:
    explicit AsyncHandler(Service& service,
                          grpc::ServerCompletionQueue& server_queue,
                          AysncFunc<Service, Request, Response> stream_func,
                          Callback callback);

    ~AsyncHandler() override;

    void activate_next() override;

private:
    Service& service_;
    grpc::ServerCompletionQueue& server_queue_;
    AysncFunc<Service, Request, Response> stream_func_;
    Callback callback_;
    grpc::CompletionQueue queue_;

    std::unique_ptr<AsyncConnection<Request, Response>> connection_;
};

template <typename Service, typename Request, typename Response, typename Callback>
AsyncHandler<Service, Request, Response, Callback>::AsyncHandler(Service& service,
                                                                 grpc::ServerCompletionQueue& server_queue,
                                                                 AysncFunc<Service, Request, Response> stream_func,
                                                                 Callback callback)
    : service_(service), server_queue_(server_queue), stream_func_(stream_func), callback_(std::move(callback)) {
    activate_next();
}

template <typename Service, typename Request, typename Response, typename Callback>
AsyncHandler<Service, Request, Response, Callback>::~AsyncHandler() {
    queue_.Shutdown();

    void* ignored_tag;
    bool call_ok;
    while (queue_.Next(&ignored_tag, &call_ok)) {
        assert(not call_ok);
    }
}

template <typename Service, typename Request, typename Response, typename Callback>
void AsyncHandler<Service, Request, Response, Callback>::activate_next() {
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
    connection_ = std::make_unique<AsyncConnection<Request, Response>>();

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
