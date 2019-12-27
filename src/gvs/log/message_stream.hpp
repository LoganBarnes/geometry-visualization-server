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
#include "gvs/log/send.hpp"

// generated
#include <scene.grpc.pb.h>

// third party
#include <grpc++/create_channel.h>

// standard
#include <sstream>

namespace gvs {
namespace log {

class MessageStream {
public:
    /// \brief Handles the server connection for messages
    ///
    ///        Defaults to a maximum 4 second wait time when attempting to connect.
    ///
    /// \param server_address - the logger will attempt to connect to this address
    /// \param identifier - the "name" of this client when sending messages
    explicit MessageStream(const std::string& server_address, const std::string& identifier = "Client");

    template <typename Rep, typename Period>
    explicit MessageStream(const std::string&                        server_address,
                           const std::chrono::duration<Rep, Period>& max_connection_wait_duration,
                           const std::string&                        identifier = "Client");

    bool connected() const;

    void send();

    MessageStream& operator<<(MessageStream& (*func)(MessageStream&));

    template <typename T>
    MessageStream& operator<<(const T& data);

private:
    std::shared_ptr<grpc::Channel>      channel_;
    std::unique_ptr<proto::Scene::Stub> stub_;
    proto::Message                      message_;
    std::stringstream                   content_stream_;
};

template <typename Rep, typename Period>
MessageStream::MessageStream(const std::string&                        server_address,
                             const std::chrono::duration<Rep, Period>& max_connection_wait_duration,
                             const std::string&                        identifier) {

    grpc::ChannelArguments channel_args;
    channel_args.SetMaxReceiveMessageSize(std::numeric_limits<int>::max());

    channel_ = grpc::CreateCustomChannel(server_address, grpc::InsecureChannelCredentials(), channel_args);

    message_.set_identifier(identifier);

    if (channel_->WaitForConnected(std::chrono::system_clock::now() + max_connection_wait_duration)) {
        stub_ = proto::Scene::NewStub(channel_);

    } else {
        std::cerr << "Failed to connect to " << server_address << ". No messages will be sent." << std::endl;
    }
}

template <typename T>
MessageStream& MessageStream::operator<<(const T& data) {
    if (stub_) {
        content_stream_ << data;
    }
    return *this;
}

} // namespace log
} // namespace gvs
