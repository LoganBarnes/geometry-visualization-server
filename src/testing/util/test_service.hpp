// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2019 Logan Barnes - All Rights Reserved
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

#include "testing/testing.grpc.pb.h"

#ifdef DOCTEST_LIBRARY_INCLUDED
#include "testing/util/test_proto_util.hpp"
#endif

namespace gvs {
namespace test {

class TestService : public gvs::test::proto::Test::Service {
public:
    grpc::Status echo(grpc::ServerContext*                 context,
                      gvs::test::proto::TestMessage const* request,
                      gvs::test::proto::TestMessage*       response) override;
};

#ifdef DOCTEST_LIBRARY_INCLUDED
TEST_CASE("[gvs-test-util] echo_service_echos") {
    const std::string message = "01234aBcDeFgHiJkLmNoPqRsTuVwXyZ56789";

    TestService service;

    gvs::test::proto::TestMessage request, response;
    request.set_msg(message);

    grpc::ServerContext context;
    service.echo(&context, &request, &response);

    CHECK(request.msg() == message); // request is unchanged
    CHECK(response == request); // response is an exact copy of request
}
#endif

} // namespace test
} // namespace gvs
