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

#include <grpcpp/client_context.h>
#include <thread>

namespace gvs {
namespace net {

template <typename Service>
class GrpcClientStreamInterface {
public:
    virtual ~GrpcClientStreamInterface() = 0;
    virtual void start_stream(std::unique_ptr<typename Service::Stub>& stub) = 0;
    virtual void stop_stream() = 0;
    virtual bool streaming() = 0;
};

template <typename Service>
GrpcClientStreamInterface<Service>::~GrpcClientStreamInterface() = default;

template <typename Service, typename Return, typename InitFunc, typename Callback>
class GrpcClientStream : public GrpcClientStreamInterface<Service> {
public:
    GrpcClientStream(InitFunc init_func, Callback callback);
    ~GrpcClientStream() = default;

    void start_stream(std::unique_ptr<typename Service::Stub>& stub) override;
    void stop_stream() override;
    bool streaming() override;

private:
    std::unique_ptr<grpc::ClientContext> context_ = nullptr;
    std::unique_ptr<grpc::ClientReader<Return>> reader_ = nullptr;
    std::unique_ptr<std::thread> stream_thread_ = nullptr;

    InitFunc init_func_;
    Callback callback_;
};

template <typename Service, typename Return, typename InitFunc, typename Callback>
GrpcClientStream<Service, Return, InitFunc, Callback>::GrpcClientStream(InitFunc init_func, Callback callback)
    : init_func_(init_func), callback_(callback) {}

template <typename Service, typename Return, typename InitFunc, typename Callback>
void GrpcClientStream<Service, Return, InitFunc, Callback>::start_stream(
    std::unique_ptr<typename Service::Stub>& stub) {
    if (streaming()) {
        return;
    }

    context_ = std::make_unique<grpc::ClientContext>();
    reader_ = init_func_(stub, context_.get());

    stream_thread_ = std::make_unique<std::thread>([this] {
        Return update;

        while (reader_->Read(&update)) {
            callback_(update);
        }

        grpc::Status status = reader_->Finish();
    });
}

template <typename Service, typename Return, typename InitFunc, typename Callback>
void GrpcClientStream<Service, Return, InitFunc, Callback>::stop_stream() {
    if (not streaming()) {
        return;
    }

    context_->TryCancel();
    stream_thread_->join();
    context_ = nullptr;
}

template <typename Service, typename Return, typename InitFunc, typename Callback>
bool GrpcClientStream<Service, Return, InitFunc, Callback>::streaming() {
    return context_ != nullptr;
}

} // namespace net
} // namespace gvs
