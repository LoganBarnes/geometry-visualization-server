// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/net/grpc_server.hpp"

#include <grpcpp/server_builder.h>

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
