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

#include "gvs/net/detail/async_handler.hpp"
#include "gvs/net/detail/stream_handler.hpp"

#include <grpc++/security/server_credentials.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>

namespace gvs {
namespace net {

template <typename Service>
class GrpcAsyncServer {
public:
    explicit GrpcAsyncServer(std::shared_ptr<Service> service, const std::string& address);
    ~GrpcAsyncServer();

    template <typename BaseService, typename Request, typename Response, typename Callback>
    void register_async(AysncFunc<BaseService, Request, Response> stream_func, Callback&& callback);

    /**
     * @brief StreamInterface* should stop being used before GrpcAsyncServer is destroyed
     */
    template <typename BaseService, typename Request, typename Response, typename Callback>
    StreamInterface<Response>* register_async_stream(AysncServerStreamFunc<BaseService, Request, Response> stream_func,
                                                     Callback&& callback);

    grpc::Server& server();

private:
    std::shared_ptr<Service> service_;
    std::unique_ptr<grpc::ServerCompletionQueue> server_queue_;
    std::unique_ptr<grpc::Server> server_;

    util::AtomicData<std::unordered_map<void*, std::unique_ptr<detail::RpcHandlerInterface>>> rpc_handlers_;

    std::thread run_thread_;
};

template <typename Service>
GrpcAsyncServer<Service>::GrpcAsyncServer(std::shared_ptr<Service> service, const std::string& address)
    : service_(std::move(service)) {

    grpc::ServerBuilder builder;
    builder.RegisterService(service_.get());
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    server_queue_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();

    run_thread_ = std::thread([&] {
        void* tag;
        bool call_ok;

        while (server_queue_->Next(&tag, &call_ok)) {
            if (call_ok) {
                rpc_handlers_.use_safely([&](auto& rpc_handlers) { rpc_handlers.at(tag)->activate_next(); });
            } else {
                rpc_handlers_.use_safely([&](auto& rpc_handlers) { rpc_handlers.erase(tag); });
            }
        }
    });
}

template <typename Service>
GrpcAsyncServer<Service>::~GrpcAsyncServer() {
    server_->Shutdown();
    server_queue_->Shutdown();

    run_thread_.join();
}

template <typename Service>
template <typename BaseService, typename Request, typename Response, typename Callback>
void GrpcAsyncServer<Service>::register_async(AysncFunc<BaseService, Request, Response> stream_func,
                                              Callback&& callback) {
    static_assert(std::is_base_of<BaseService, Service>::value, "BaseService must be a base class of Service");
    auto handler
        = std::make_unique<detail::AsyncHandler<BaseService, Request, Response, Callback>>(*service_,
                                                                                           *server_queue_,
                                                                                           stream_func,
                                                                                           std::forward<Callback>(
                                                                                               callback));

    void* tag = handler.get();
    rpc_handlers_.use_safely([&](auto& rpc_handlers) { rpc_handlers.emplace(tag, std::move(handler)); });
}

template <typename Service>
template <typename BaseService, typename Request, typename Response, typename Callback>
StreamInterface<Response>*
GrpcAsyncServer<Service>::register_async_stream(AysncServerStreamFunc<BaseService, Request, Response> stream_func,
                                                Callback&& callback) {
    static_assert(std::is_base_of<BaseService, Service>::value, "BaseService must be a base class of Service");
    auto handler
        = std::make_unique<detail::StreamHandler<BaseService, Request, Response, Callback>>(*service_,
                                                                                            *server_queue_,
                                                                                            stream_func,
                                                                                            std::forward<Callback>(
                                                                                                callback));

    auto* tag = handler.get();
    rpc_handlers_.use_safely([&](auto& rpc_handlers) { rpc_handlers.emplace(tag, std::move(handler)); });
    return tag;
}

template <typename Service>
grpc::Server& GrpcAsyncServer<Service>::server() {
    return *server_;
}

} // namespace net
} // namespace gvs
