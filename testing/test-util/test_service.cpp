// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "test_service.hpp"

namespace gvs {
namespace test {

grpc::Status TestService::echo(grpc::ServerContext* /*context*/,
                               const gvs::test::proto::TestMessage* request,
                               gvs::test::proto::TestMessage* response) {
    response->CopyFrom(*request);
    return grpc::Status::OK;
}

} // namespace test
} // namespace gvs
