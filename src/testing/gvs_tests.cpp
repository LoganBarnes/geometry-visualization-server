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
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

//#include "gvs/server/scene_server.hpp"

#include "gvs/net/detail/async_rpc_handler_interface.hpp"
#include "gvs/net/detail/non_stream_rpc_handler.hpp"
#include "gvs/net/detail/stream_rpc_handler.hpp"
#include "gvs/net/detail/tag.hpp"
#include "gvs/net/grpc_async_server.hpp"
#include "gvs/net/grpc_client.hpp"
#include "gvs/net/grpc_client_state.hpp"
#include "gvs/net/grpc_client_stream.hpp"
#include "gvs/net/grpc_server.hpp"

#include "gvs/util/apply.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/blocking_queue.hpp"
#include "gvs/util/callback_handler.hpp"
#include "gvs/util/container_util.hpp"
#include "gvs/util/proto_util.hpp"
#include "gvs/util/string.hpp"

#include "testing/util/test_proto_util.hpp"
#include "testing/util/test_server.hpp"
#include "testing/util/test_service.hpp"
