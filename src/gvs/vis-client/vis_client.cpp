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
#include "gvs/vis-client/imgui_utils.hpp"
#include "gvs/server/scene_server.hpp"
#include "gvs/net/grpc_server.hpp"
#include "gvs/net/grpc_client.hpp"
#include "vis_client.hpp"

#include <Magnum/GL/Context.h>
#include <imgui.h>

#include <limits>
#include <sstream>
#include <chrono>

namespace gvs {
namespace vis {

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

} // namespace

VisClient::VisClient(const std::string& initial_host_address, const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("Debug Visualiser Client")
                                 .setSize({1280, 720})
                                 .setWindowFlags(Configuration::WindowFlag::Resizable))
    , gl_version_str_(Magnum::GL::Context::current().versionString())
    , gl_renderer_str_(Magnum::GL::Context::current().rendererString())
    , server_address_input_(initial_host_address)
    , grpc_client_(std::make_unique<net::GrpcClient<gvs::proto::Scene>>()) {

    grpc_client_->change_server(server_address_input_,
                                [this](const net::GrpcClientState&) { this->on_state_change(); });

    // Connect to the stream that delivers message updates
    grpc_client_->register_stream<proto::Message>(
        [](std::unique_ptr<proto::Scene::Stub>& stub, grpc::ClientContext* context) {
            google::protobuf::Empty empty;
            return stub->MessageUpdates(context, empty);
        },
        [this](const proto::Message& msg) { this->process_message_update(msg); });
}

vis::VisClient::~VisClient() = default;

void vis::VisClient::update() {}

void vis::VisClient::render() const {}

void vis::VisClient::configure_gui() {
    int h = this->windowSize().y();
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, h), ImVec2(std::numeric_limits<float>::infinity(), h));
    ImGui::Begin("Settings", nullptr, ImVec2(200.f, h));

    ImGui::Text("GL Version:   ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_version_str_.c_str());

    ImGui::Text("GL Renderer:  ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_renderer_str_.c_str());

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Separator();
    ImGui::Separator();
    ImGui::Spacing();

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

    messages_.use_safely([&](const gvs::proto::Messages& messages) {
        float message_input_start_height = h - 100.f;
        float max_message_window_height = message_input_start_height - ImGui::GetCursorPos().y;

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BeginChild("Messages", {ImGui::GetWindowSize().x, max_message_window_height});

        auto text_func = (wrap_text_ ? &ImGui::TextWrapped : &ImGui::Text);

        for (const auto& message : messages.messages()) {
            ImGui::TextColored({0.7f, 0.7f, 0.7f, 1.f}, "%s: ", message.identifier().c_str());
            ImGui::SameLine();
            text_func("%s", message.contents().c_str());
        }
        ImGui::SetScrollY(ImGui::GetScrollMaxY());
        ImGui::EndChild();

        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
    });

    bool send_message = false;

    send_message |= imgui::configure_gui("Name", &message_id_input_);
    send_message |= imgui::configure_gui("Contents", &message_content_input_);

    send_message |= ImGui::Button("Send");

    ImGui::SameLine();
    ImGui::Checkbox("Wrap Text", &wrap_text_);

    if (send_message) {
        ImGui::SetKeyboardFocusHere(-1);
        gvs::proto::Message message;
        message.set_identifier(message_id_input_);
        message.set_contents(message_content_input_);

        if (grpc_client_->use_stub([&](auto& stub) {
                grpc::ClientContext context;
                gvs::proto::Errors errors;
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

void VisClient::process_message_update(const proto::Message& message) {
    messages_.use_safely([&](gvs::proto::Messages& messages) { messages.add_messages()->CopyFrom(message); });
    reset_draw_counter();
}

void VisClient::on_state_change() {
    gvs::proto::Messages messages;

    // Load all the messages when the client first connects.
    if (grpc_client_->use_stub([&](const auto& stub) {
            // This lambda is only used if the client is connected
            grpc::ClientContext context;
            google::protobuf::Empty empty;
            stub->GetAllMessages(&context, empty, &messages);
        })) {

        // This is only called if the client is connected
        messages_.use_safely([&](gvs::proto::Messages& msgs) { msgs.CopyFrom(messages); });
    }
    reset_draw_counter();
}

} // namespace vis
} // namespace gvs
