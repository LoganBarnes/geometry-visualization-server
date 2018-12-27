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
#include "gvs/net/grpc_server.hpp"

#include <grpc++/server_builder.h>

#include <utility>

namespace gvs {
namespace net {

GrpcServer::GrpcServer(std::shared_ptr<grpc::Service> service, const std::string& server_address)
    : service_(std::move(service)) {

    grpc::ServerBuilder builder;
    builder.RegisterService(service_.get());

    if (not server_address.empty()) {
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    }

    server_ = builder.BuildAndStart();
}

void GrpcServer::run() {
    server_->Wait();
}

void GrpcServer::shutdown() {
    server_->Shutdown();
}

std::shared_ptr<grpc::Service>& GrpcServer::service() {
    return service_;
}

const std::shared_ptr<grpc::Service>& GrpcServer::service() const {
    return service_;
}

std::unique_ptr<grpc::Server>& GrpcServer::server() {
    return server_;
}

const std::unique_ptr<grpc::Server>& GrpcServer::server() const {
    return server_;
}

} // namespace net
} // namespace gvs
