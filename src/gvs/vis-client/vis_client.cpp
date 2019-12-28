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
#include "vis_client.hpp"

// project
#include "gvs/display/local_scene.hpp"
#include "gvs/gui/imgui_utils.hpp"
#include "gvs/gui/scene_gui.hpp"
#include "gvs/vis-client/app/imgui_theme.hpp"

// generated
#include "gvs/gvs_paths.hpp"

// external
#include <Magnum/GL/Context.h>
#include <grpcw/client/grpc_client.hpp>
#include <imgui.h>

// standard
#include <limits>

using namespace Magnum;

namespace gvs::vis {

namespace {

std::string to_pretty_string(const grpcw::client::GrpcClientState& state) {
    switch (state) {
    case grpcw::client::GrpcClientState::not_connected:
        return "Not Connected";

    case grpcw::client::GrpcClientState::connected:
        return "Connected";

    case grpcw::client::GrpcClientState::attempting_to_connect:
        return "Connecting...";
    }
    return "Unknown Enum value";
}

ImVec4 to_pretty_color(const grpcw::client::GrpcClientState& state) {
    switch (state) {
    case grpcw::client::GrpcClientState::not_connected:
        return {1.f, 0.f, 0.f, 1.f}; // Red

    case grpcw::client::GrpcClientState::connected:
        return {0.f, 1.f, 0.f, 1.f}; // Green

    case grpcw::client::GrpcClientState::attempting_to_connect:
        return {1.f, 1.f, 0.f, 1.f}; // Yellow
    }
    return {1.f, 1.f, 1.f, 1.f}; // White
}

} // namespace

VisClient::VisClient(std::string initial_host_address, const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("Geometry Visualisation Client")
                                 .setSize({1280, 720})
                                 .setWindowFlags(Configuration::WindowFlag::Resizable)),
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()),
      error_alert_("Vis Client Errors"),
      server_address_input_(std::move(initial_host_address)),
      grpc_client_(std::make_unique<grpcw::client::GrpcClient<Service>>()) {

    scene_ = std::make_unique<display::LocalScene>();

    grpc_client_->change_server(server_address_input_, [this](const auto&) { this->on_state_change(); });
}

vis::VisClient::~VisClient() = default;

void vis::VisClient::update() {
    //scene_->update();
}

void vis::VisClient::render(const display::CameraPackage& camera_package) const {
    if (!scene_->empty()) {
        scene_->render(camera_package);
    }
}

void vis::VisClient::configure_gui() {

    auto add_three_line_separator = [] {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    };

    int h = this->windowSize().y();
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, h), ImVec2(std::numeric_limits<float>::infinity(), h));
    ImGui::Begin("Settings", nullptr, ImVec2(350.f, h));

    ImGui::Text("GL Version:   ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_version_str_.c_str());

    ImGui::Text("GL Renderer:  ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_renderer_str_.c_str());

    add_three_line_separator();

    ImGui::Text("Host Address: ");
    ImGui::SameLine();
    ImGui::TextColored({7.f, 0.3f, 0.05f, 1.f}, "%s\t", grpc_client_->get_server_address().c_str());

    ImGui::Text("Server State: ");
    ImGui::SameLine();

    auto        state       = grpc_client_->get_state();
    std::string state_text  = to_pretty_string(state);
    ImVec4      state_color = to_pretty_color(state);

    ImGui::TextColored(state_color, "%s\t", state_text.c_str());

    if (ImGui::TreeNode("Server Settings")) {

        bool value_changed = gui::configure_gui("Change Server", &server_address_input_);
        {
            gui::Disable::Guard disable(server_address_input_ == grpc_client_->get_server_address()
                                        and state != grpcw::client::GrpcClientState::connected);
            value_changed |= (ImGui::Button("Connect"));
        }

        if (value_changed) {
            grpc_client_->change_server(server_address_input_, [this](const auto&) { this->on_state_change(); });
        }

        if (state == grpcw::client::GrpcClientState::attempting_to_connect) {
            ImGui::SameLine();
            if (ImGui::Button("Stop Connecting")) {
                grpc_client_->kill_streams_and_channel();
            }
        }

        ImGui::TreePop();
    }

    add_three_line_separator();

    ImGui::Checkbox("Run app as fast as possible", &run_as_fast_as_possible_);

    if (run_as_fast_as_possible_) {
        ImGui::TextColored({0.f, 1.f, 0.f, 1.f}, "FPS: %.3f", static_cast<double>(ImGui::GetIO().Framerate));
        reset_draw_counter();
    }

    add_three_line_separator();

#ifdef OptiX_FOUND
    // Quick hack for now to switch between OptiX and OpenGL rendering
    bool using_optix = (dynamic_cast<OptiXScene*>(scene_.get()) != nullptr);
    if (ImGui::Checkbox("Use OptiX for rendering", &using_optix)) {
        if (using_optix) {
            scene_->set_backend(std::make_unique<OptixBackend>());

        } else {
            scene_->set_backend(std::make_unique<OpenglBackend>());
        }
    }
#endif

    gui::configure_gui(scene_.get());

    ImGui::End();

    error_alert_.display_next_error();
}

void VisClient::resize(const Vector2i& viewport) {
    scene_->resize(viewport);
}

void VisClient::on_state_change() {
    reset_draw_counter();
}

} // namespace gvs::vis
