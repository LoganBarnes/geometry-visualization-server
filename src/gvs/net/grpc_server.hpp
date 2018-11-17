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
