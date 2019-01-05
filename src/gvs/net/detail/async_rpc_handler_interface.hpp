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

#include "gvs/util/blocking_queue.hpp"

#include <grpc++/completion_queue.h>
#include <grpc++/server_context.h>

#include <experimental/optional>

namespace gvs {
namespace net {

/**
 * @brief The function signature for a service's non-streaming calls
 */
template <typename Service, typename Request, typename Response>
using AsyncNoStreamFunc = void (Service::*)(grpc::ServerContext*,
                                            Request*,
                                            grpc::ServerAsyncResponseWriter<Response>*,
                                            grpc::CompletionQueue*,
                                            grpc::ServerCompletionQueue*,
                                            void*);

/**
 * @brief The function signature for a service's server-side-streaming calls
 */
template <typename Service, typename Request, typename Response>
using AsyncServerStreamFunc = void (Service::*)(grpc::ServerContext* context,
                                                Request*,
                                                grpc::ServerAsyncWriter<Response>*,
                                                grpc::CompletionQueue*,
                                                grpc::ServerCompletionQueue*,
                                                void*);

namespace detail {

class AsyncRpcHandlerInterface {
public:
    virtual ~AsyncRpcHandlerInterface() = default;

    /**
     * @brief Tells the handler to call the appropriate service
     *     function (AysncNoStreamFunc or AysncServerStreamFunc)
     *
     * This will update the server's queue so client requests can be handled when
     * they are received.
     */
    virtual void activate_next() = 0;
};

} // namespace detail
} // namespace net
} // namespace gvs
