// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/vis-client/vis_client.hpp"
#include "gvs/server/scene_service.hpp"
#include "gvs/common/grpc_server.hpp"
#include <thread>

int main(int argc, char** argv) {

    gvs::util::GrpcServer server(std::make_shared<gvs::host::SceneService>());

    std::thread run_thread([&] { server.run(); });

    gvs::vis::VisClient app(server.server(), {argc, argv});

    int exit_code = app.exec();

    server.shutdown();
    run_thread.join();

    return exit_code;
}
