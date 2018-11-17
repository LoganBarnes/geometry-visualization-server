// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/forward_declarations.hpp"

#include <grpcpp/server.h>

namespace gvs {
namespace net {

class GrpcServer {
public:
    explicit GrpcServer(std::shared_ptr<grpc::Service> service, const std::string& server_address = "");

    /**
     * @brief Blocks until server is shutdown and all rpc calls terminate
     */
    void run();

    template <typename TimePoint>
    void shutdown(const TimePoint& deadline);
    void shutdown();

    std::shared_ptr<grpc::Service>& service();
    const std::shared_ptr<grpc::Service>& service() const;

    std::unique_ptr<grpc::Server>& server();
    const std::unique_ptr<grpc::Server>& server() const;

private:
    std::shared_ptr<grpc::Service> service_;
    std::unique_ptr<grpc::Server> server_;
};

template <typename TimePoint>
void GrpcServer::shutdown(const TimePoint& deadline) {
    server_->Shutdown(deadline);
}

} // namespace net
} // namespace gvs
