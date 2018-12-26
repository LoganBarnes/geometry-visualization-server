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

// project
#include "gvs/log/geometry_item_stream.hpp"

// third party
#include <crossguid/guid.hpp>
#include <grpcpp/create_channel.h>

namespace gvs {
namespace log {

class GeometryLogger {
public:
    /// \brief Handles the server connection for geometry streams
    ///
    ///        If 'server_address' is an empty string, no connection attempt will be made.
    ///        Defaults to a maximum 3 second wait time when attempting to connect.
    ///
    /// \param server_address
    explicit GeometryLogger(const std::string& server_address);

    template <typename Rep, typename Period>
    explicit GeometryLogger(const std::string& server_address,
                            const std::chrono::duration<Rep, Period>& max_connection_wait_duration);

    bool connected() const;

    std::string generate_uuid() const;

    std::string clear_all_items();
    GeometryItemStream item_stream(const std::string& id = "");

private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<proto::Scene::Stub> stub_;
};

template <typename Rep, typename Period>
GeometryLogger::GeometryLogger(const std::string& server_address,
                               const std::chrono::duration<Rep, Period>& max_connection_wait_duration) {

    if (server_address.empty()) {
        std::cout << "No server address provided. Ignoring stream requests." << std::endl;

    } else {
        channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());

        if (channel_->WaitForConnected(std::chrono::system_clock::now() + max_connection_wait_duration)) {
            stub_ = proto::Scene::NewStub(channel_);

        } else {
            channel_ = nullptr;
            std::cerr << "Failed to connect to " << server_address << ". No messages will be sent." << std::endl;
        }
    }
}

} // namespace log
} // namespace gvs
