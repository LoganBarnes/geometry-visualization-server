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
#include "gvs/vis-client/vis_client.hpp"

#include "gvs/net/grpc_client.hpp"
#include "gvs/net/grpc_server.hpp"
#include "gvs/server/scene_server.hpp"
#include "gvs/vis-client/app/imgui_theme.hpp"
#include "gvs/vis-client/imgui_utils.hpp"
#include "gvs/vis-client/scene/opengl_scene.hpp"

#include <gvs/gvs_paths.hpp>

#ifdef OptiX_FOUND
#include "gvs/optix/scene/optix_scene.hpp"
#endif

#include <Magnum/GL/Context.h>
#include <imgui.h>

#include <chrono>
#include <gvs/scene.pb.h>
#include <limits>
#include <sstream>

namespace gvs {
namespace vis {

#ifdef OptiX_FOUND
using DefaultSceneType = OptiXScene;
#else
using DefaultSceneType = OpenGLScene;
#endif

namespace {

std::string to_pretty_string(const net::GrpcClientState& state) {
    switch (state) {
    case net::GrpcClientState::not_connected:
        return "Not Connected";

    case net::GrpcClientState::connected:
        return "Connected";

    case net::GrpcClientState::attempting_to_connect:
        return "Connecting...";
    }
    return "Unknown Enum value";
}

ImVec4 to_pretty_color(const net::GrpcClientState& state) {
    switch (state) {
    case net::GrpcClientState::not_connected:
        return {1.f, 0.f, 0.f, 1.f}; // Red

    case net::GrpcClientState::connected:
        return {0.f, 1.f, 0.f, 1.f}; // Green

    case net::GrpcClientState::attempting_to_connect:
        return {1.f, 1.f, 0.f, 1.f}; // Yellow
    }
    return {1.f, 1.f, 1.f, 1.f}; // White
}

SceneInitializationInfo scene_init_info(const ImColor& background_color, const Magnum::Vector2i& viewport) {
    return {{background_color.Value.x, background_color.Value.y, background_color.Value.z}, viewport};
}

} // namespace

VisClient::VisClient(const std::string& initial_host_address, const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("Geometry Visualisation Client")
                                 .setSize({1280, 720})
                                 .setWindowFlags(Configuration::WindowFlag::Resizable))
    , gl_version_str_(Magnum::GL::Context::current().versionString())
    , gl_renderer_str_(Magnum::GL::Context::current().rendererString())
    , server_address_input_(initial_host_address)
    , grpc_client_(std::make_unique<net::GrpcClient<proto::Scene>>())
    , scene_(std::make_unique<DefaultSceneType>(scene_init_info(theme_->background, this->windowSize()))) {

    grpc_client_->change_server(server_address_input_,
                                [this](const net::GrpcClientState&) { this->on_state_change(); });

    // Connect to the stream that delivers message updates
    grpc_client_->register_stream<proto::Message>(
        [](std::unique_ptr<proto::Scene::Stub>& stub, grpc::ClientContext* context) {
            google::protobuf::Empty empty;
            return stub->MessageUpdates(context, empty);
        },
        [this](const proto::Message& msg) { this->process_message_update(msg); });

    // Connect to the stream that delivers scene updates
    grpc_client_->register_stream<proto::SceneUpdate>(
        [](std::unique_ptr<proto::Scene::Stub>& stub, grpc::ClientContext* context) {
            google::protobuf::Empty empty;
            return stub->SceneUpdates(context, empty);
        },
        [this](const proto::SceneUpdate& update) { this->process_scene_update(update); });
}

vis::VisClient::~VisClient() = default;

void vis::VisClient::update() {
    scene_updates_.use_safely([this](std::vector<proto::SceneUpdate>& updates) {
        for (const proto::SceneUpdate& update : updates) {

            switch (update.update_case()) {
            case proto::SceneUpdate::kAddItem:
                scene_->add_item(update.add_item());
                break;

            case proto::SceneUpdate::kResetAllItems:
                scene_->reset(update.reset_all_items());
                break;

            case proto::SceneUpdate::UPDATE_NOT_SET:
                break;
            }
        }
        updates.clear();
    });

    scene_->update(this->windowSize());
}

void vis::VisClient::render() const {
    scene_->render(this->windowSize());
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

    auto state = grpc_client_->get_state();
    std::string state_text = to_pretty_string(state);
    ImVec4 state_color = to_pretty_color(state);

    ImGui::TextColored(state_color, "%s\t", state_text.c_str());

    if (ImGui::TreeNode("Server Settings")) {

        bool value_changed = imgui::configure_gui("Change Server", &server_address_input_);
        {
            imgui::Disable::Guard disable(server_address_input_ == grpc_client_->get_server_address()
                                          and state != net::GrpcClientState::connected);
            value_changed |= (ImGui::Button("Connect"));
        }

        if (value_changed) {
            grpc_client_->change_server(server_address_input_,
                                        [this](const net::GrpcClientState&) { this->on_state_change(); });
        }

        if (state == net::GrpcClientState::attempting_to_connect) {
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
        ImGui::TextColored({0.f, 1.f, 0.f, 1.f}, "FPS: %.3f", ImGui::GetIO().Framerate);
        reset_draw_counter();
    }

    add_three_line_separator();

#ifdef OptiX_FOUND
    // Quick hack for now to switch between OptiX and OpenGL rendering
    bool using_optix = (dynamic_cast<OptiXScene*>(scene_.get()) != nullptr);
    if (ImGui::Checkbox("Use OptiX for rendering", &using_optix)) {
        if (using_optix) {
            scene_ = std::make_unique<OptiXScene>(scene_init_info(theme_->background, this->windowSize()));

        } else {
            scene_ = std::make_unique<OpenGLScene>(scene_init_info(theme_->background, this->windowSize()));
        }

        on_state_change(); // Get state if client is connected
    }
#endif

    scene_->configure_gui(this->windowSize());

    messages_.use_safely([&](const proto::Messages& messages) {
        float message_input_start_height = h - 100.f;
        float max_message_window_height = message_input_start_height - ImGui::GetCursorPos().y;

        add_three_line_separator();

        ImGui::BeginChild("Messages", {ImGui::GetWindowSize().x, max_message_window_height});

        auto text_func = (wrap_text_ ? &ImGui::TextWrapped : &ImGui::Text);

        for (const auto& message : messages.messages()) {
            ImGui::TextColored({0.7f, 0.7f, 0.7f, 1.f}, "%s: ", message.identifier().c_str());
            ImGui::SameLine();
            text_func("%s", message.contents().c_str());
        }
        ImGui::SetScrollY(ImGui::GetScrollMaxY());
        ImGui::EndChild();

        add_three_line_separator();
    });

    bool send_message = false;

    send_message |= imgui::configure_gui("Name", &message_id_input_);
    send_message |= imgui::configure_gui("Contents", &message_content_input_);

    send_message |= ImGui::Button("Send");

    ImGui::SameLine();
    ImGui::Checkbox("Wrap Text", &wrap_text_);

    if (send_message) {
        ImGui::SetKeyboardFocusHere(-1);
        proto::Message message;
        message.set_identifier(message_id_input_);
        message.set_contents(message_content_input_);

        if (grpc_client_->use_stub([&](auto& stub) {
                grpc::ClientContext context;
                proto::Errors errors;
                grpc::Status status = stub->SendMessage(&context, message, &errors);

                if (not status.ok()) {
                    error_message_ = status.error_message();

                } else if (not errors.error_msg().empty()) {
                    error_message_ = errors.error_msg();
                }
            })) {
            message_content_input_ = "";

        } else {
            error_message_ = "Not connected";
        }
    }

    ImGui::End();

    if (not error_message_.empty()) {
        int w = this->windowSize().x();
        ImGui::SetNextWindowPos({w * 0.5f, 0.f}, 0, {0.5f, 0.f});

        bool open = true;
        ImGui::Begin("Errors", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        ImGui::TextColored({1.f, 0.f, 0.f, 1.f}, "%s", error_message_.c_str());
        ImGui::End();

        if (not open) {
            error_message_ = "";
        }
    }
}

void VisClient::resize(const Magnum::Vector2i& viewport) {
    scene_->resize(viewport);
}

void VisClient::process_message_update(const proto::Message& message) {
    messages_.use_safely([&](proto::Messages& messages) { messages.add_messages()->CopyFrom(message); });
    reset_draw_counter();
}

void VisClient::process_scene_update(const proto::SceneUpdate& update) {
    scene_updates_.use_safely([&](std::vector<proto::SceneUpdate>& updates) { updates.emplace_back(update); });
    reset_draw_counter();
}

void VisClient::on_state_change() {

    // Load all the messages when the client first connects.
    get_message_state(false);

    // Load the current scene when the client first connects.
    get_scene_state(false);

    reset_draw_counter();
}

void VisClient::get_message_state(bool redraw) {
    proto::Messages messages;

    if (grpc_client_->use_stub([&](const auto& stub) {
            // This lambda is only used if the client is connected
            grpc::ClientContext context;
            google::protobuf::Empty empty;
            stub->GetAllMessages(&context, empty, &messages);
        })) {

        // This is only called if the client is connected
        messages_.use_safely([&](proto::Messages& msgs) { msgs.CopyFrom(messages); });
    }

    if (redraw) {
        reset_draw_counter();
    }
}

void VisClient::get_scene_state(bool redraw) {
    proto::SceneItems scene;

    if (grpc_client_->use_stub([&](const auto& stub) {
            // This lambda is only used if the client is connected
            grpc::ClientContext context;
            google::protobuf::Empty empty;
            stub->GetAllItems(&context, empty, &scene);
        })) {

        // This is only called if the client is connected
        scene_updates_.use_safely([&](std::vector<proto::SceneUpdate>& updates) {
            updates.emplace_back();
            updates.back().mutable_reset_all_items()->CopyFrom(scene);
        });
    }

    if (redraw) {
        reset_draw_counter();
    }
}

} // namespace vis
} // namespace gvs
