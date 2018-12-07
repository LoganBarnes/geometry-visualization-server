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
#include "gvs/log/log_params.hpp"
#include "gvs/log/send.hpp"

// generated
#include <gvs/scene.grpc.pb.h>

// third party
#include <grpcpp/create_channel.h>

// standard
#include <sstream>

namespace gvs {
namespace log {

class GeometryStream {
public:
    template <typename Rep, typename Period>
    explicit GeometryStream(const std::string& server_address,
                            const std::chrono::duration<Rep, Period>& max_connection_wait_duration
                            /*, seed argument?*/);

    bool connected() const;

    void send();

    GeometryStream& operator<<(GeometryStream& (*func)(GeometryStream&));

    //    template <ParamType PT, typename Func>
    //    GeometryStream& operator<<(Param<PT, Func>&& param);

private:
    std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<gvs::proto::Scene::Stub> stub_;
    gvs::proto::SceneUpdate update_;
    //    gvs::proto::SceneItemInfo* info_;
};

template <typename Rep, typename Period>
GeometryStream::GeometryStream(const std::string& server_address,
                               const std::chrono::duration<Rep, Period>& max_connection_wait_duration)
    : channel_(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials())) {

    if (channel_->WaitForConnected(std::chrono::system_clock::now() + max_connection_wait_duration)) {
        stub_ = gvs::proto::Scene::NewStub(channel_);

    } else {
        std::cerr << "Failed to connect to " << server_address << ". No messages will be sent." << std::endl;
    }
}

//template <ParamType PT, typename Func>
//GeometryStream& GeometryStream::operator<<(Param<PT, Func>&& param) {
//    if (stub_) {
//        param.move_func(&info_);
//    }
//    return *this;
//}

} // namespace log
} // namespace gvs
