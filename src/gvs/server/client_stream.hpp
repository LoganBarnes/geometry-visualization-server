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

#include <grpcpp/server_context.h>
#include <grpcpp/completion_queue.h>
#include <thread>

namespace gvs {
namespace net {

enum class StreamState {
    WAITING_FOR_CONNECTION,
    CONNECTED,
    DONE,
};

template <typename Request, typename Response>
using AsyncRequestFunc = void (*)(grpc::ServerContext*,
                                  Request*,
                                  grpc::ServerAsyncWriter<Response>*,
                                  grpc::CompletionQueue*,
                                  grpc::ServerCompletionQueue*,
                                  void*);

template <typename Request, typename Response>
class ClientStream {
public:
    template <typename Update>
    explicit ClientStream(AsyncRequestFunc<Request, Response> request_func,
                          Request request,
                          grpc::ServerCompletionQueue* server_queue,
                          Update update_func);

    ~ClientStream();

    bool process(bool ok);

private:
    grpc::ServerContext context_;
    Request request_;
    grpc::ServerAsyncWriter<Response> responder_;
    StreamState state_;
    grpc::CompletionQueue queue_;

    std::thread run_thread_;
};

template <typename Request, typename Response>
template <typename Update>
ClientStream<Request, Response>::ClientStream(AsyncRequestFunc<Request, Response> request_func,
                                              Request request,
                                              grpc::ServerCompletionQueue* server_queue,
                                              Update update_func)

    : request_(std::move(request)), responder_(context_), state_(StreamState::WAITING_FOR_CONNECTION) {

    request_func(&context_, &request_, &responder_, &queue_, server_queue, this);

    run_thread_ = std::thread([&] {
        void* tag;
        bool not_cancelled;

        while (auto update = update_func()) {
            responder_.Write(update, this);

            queue_.Next(&tag, &not_cancelled);
            queue_
        }
    });
}

template <typename Request, typename Response>
ClientStream<Request, Response>::~ClientStream() {
    if (not context_.IsCancelled()) {
        context_.TryCancel();
    }
    run_thread_.join();
}

template <typename Request, typename Response>
bool ClientStream<Request, Response>::process(bool ok) {
    return not ok;
}

template <typename Request, typename Response>
ClientStream<Request, Response> make_client_stream(AsyncRequestFunc<Request, Response> request_func, Request request) {
    return ClientStream<Request, Response>(request_func, std::move(request));
}

} // namespace net
} // namespace gvs
