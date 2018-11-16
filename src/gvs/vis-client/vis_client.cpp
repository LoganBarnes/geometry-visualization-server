// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/vis-client/vis_client.hpp"
#include "gvs/vis-client/imgui_utils.hpp"
#include "gvs/server/scene_service.hpp"
#include "gvs/common/grpc_server.hpp"
#include "gvs/common/dual_grpc_client.hpp"

#include <Magnum/GL/Context.h>
#include <imgui.h>

#include <limits>
#include <sstream>

namespace gvs {
namespace vis {

namespace {

std::string to_pretty_string(const util::GrpcClientState& state, int connection_attempt) {
    switch (state) {
    case util::GrpcClientState::not_connected:
        return "Not Connected";
    case util::GrpcClientState::attempting_to_connect:
        return "Connecting... (Attempt " + std::to_string(connection_attempt) + ")";
    case util::GrpcClientState::connected:
        return "Connected";
    }
    return "Unknown Enum value";
}

ImVec4 to_pretty_color(const util::GrpcClientState& state) {
    switch (state) {
    case util::GrpcClientState::not_connected:
        return {1.f, 0.f, 0.f, 1.f}; // Red
    case util::GrpcClientState::attempting_to_connect:
        return {1.f, 1.f, 0.f, 1.f}; // Yellow
    case util::GrpcClientState::connected:
        return {0.f, 1.f, 0.f, 1.f}; // Green
    }
    return {1.f, 1.f, 1.f, 1.f};
}

} // namespace

VisClient::VisClient(const std::string& initial_host_address, const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("Debug Visualiser Client")
                                 .setSize({1280, 720})
                                 .setWindowFlags(Configuration::WindowFlag::Resizable))
    , gl_version_str_(Magnum::GL::Context::current().versionString())
    , gl_renderer_str_(Magnum::GL::Context::current().rendererString())
    , grpc_client_(std::make_unique<util::DualGrpcClient<gvs::proto::Scene>>(&VisClient::redraw, this))
    , has_inprocess_server_(false) {

    grpc_client_->set_external_server(initial_host_address);
    server_address_input_ = grpc_client_->server_address();
}

VisClient::VisClient(std::unique_ptr<grpc::Server>& inprocess_server, const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("Debug Visualiser Client")
                                 .setSize({1280, 720})
                                 .setWindowFlags(Configuration::WindowFlag::Resizable))
    , gl_version_str_(Magnum::GL::Context::current().versionString())
    , gl_renderer_str_(Magnum::GL::Context::current().rendererString())
    , grpc_client_(std::make_unique<util::DualGrpcClient<gvs::proto::Scene>>(&VisClient::redraw, this))
    , has_inprocess_server_(true) {

    grpc_client_->set_inprocess_server(inprocess_server);
}

VisClient::~VisClient() = default;

void VisClient::update() {}

void VisClient::render() const {}

void VisClient::configure_gui() {
    int h = this->windowSize().y();
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, h), ImVec2(std::numeric_limits<float>::infinity(), h));
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("GL Version:   ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "%s\t", gl_version_str_.c_str());

    ImGui::Text("GL Renderer:  ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "%s\t", gl_renderer_str_.c_str());

    ImGui::Separator();

    ImGui::Text("Host Address: ");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(7.f, 0.3f, 0.05f, 1.f), "%s\t", grpc_client_->server_address().c_str());

    ImGui::Text("Server State: ");
    ImGui::SameLine();

    int connection_attempt;
    auto state = grpc_client_->connection_state(&connection_attempt);
    std::string state_text = to_pretty_string(state, connection_attempt);
    ImVec4 state_color = to_pretty_color(state);

    ImGui::TextColored(state_color, "%s\t", state_text.c_str());

    if (ImGui::TreeNode("Server Settings")) {

        char buf[1024];
        server_address_input_.copy(buf, server_address_input_.size());
        buf[server_address_input_.size()] = '\0';

        bool value_changed = ImGui::InputText("Change Server", buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue);
        server_address_input_ = buf;

        if (value_changed) {
            grpc_client_->set_external_server(server_address_input_);
        }

        if (state == util::GrpcClientState::attempting_to_connect and ImGui::Button("Stop Connecting")) {
            grpc_client_->stop_connection_attempts();
        }

        {
            imgui::Disable::Guard disable(grpc_client_->using_inprocess_channel());

            if (has_inprocess_server_ and ImGui::Button("Use In-Process Server")) {
                grpc_client_->set_using_inprocess_channel(true);
            }
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

} // namespace vis
} // namespace gvs
