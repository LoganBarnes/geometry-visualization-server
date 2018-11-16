// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "testing/testing.grpc.pb.h"

namespace gvs {
namespace test {

class TestService : public gvs::test::proto::Test::Service {
public:
    grpc::Status echo(grpc::ServerContext* context,
                      const gvs::test::proto::TestMessage* request,
                      gvs::test::proto::TestMessage* response) override;
};

} // namespace test
} // namespace gvs
