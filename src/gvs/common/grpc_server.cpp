// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/common/grpc_server.hpp"

#include <grpcpp/server_builder.h>

#include <utility>

namespace gvs {
namespace util {

GrpcServer::GrpcServer(const std::string& server_address, std::shared_ptr<grpc::Service> service)
    : service_(std::move(service)) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(service_.get());

    server_ = builder.BuildAndStart();
}

GrpcServer::GrpcServer(std::shared_ptr<grpc::Service> service) : service_(std::move(service)) {
    grpc::ServerBuilder builder;
    builder.RegisterService(service_.get());

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

} // namespace util
} // namespace gvs
