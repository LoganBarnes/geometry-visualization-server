// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "gvs/vis-client/imgui_magnum_application.hpp"
#include "gvs/forward_declarations.hpp"

#include <gvs/scene.grpc.pb.h>

#include <grpcpp/channel.h>

namespace gvs {
namespace vis {

class VisClient : public ImGuiMagnumApplication {
public:
    explicit VisClient(const std::string& initial_host_address, const Arguments& arguments);
    explicit VisClient(std::unique_ptr<grpc::Server>& inprocess_server, const Arguments& arguments);
    ~VisClient() override;

private:
    void update() override;
    void render() const override;
    void configure_gui() override;

    std::string gl_version_str_;
    std::string gl_renderer_str_;

    std::unique_ptr<util::DualGrpcClient<gvs::proto::Scene>> grpc_client_;

    bool has_inprocess_server_ = false;
    std::string server_address_input_ = "address:port";
};

} // namespace vis
} // namespace gvs
