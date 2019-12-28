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

// project
#include "gvs/display/forward_declarations.hpp"
#include "gvs/util/atomic_data.hpp"
#include "gvs/util/blocking_queue.hpp"
#include "gvs/vis-client/app/imgui_magnum_application.hpp"

// generated
#include <scene.grpc.pb.h>

// external
#include <grpcw/forward_declarations.hpp>
#include <gvs/gui/error_alert.hpp>

namespace gvs::vis {

class VisClient : public ImGuiMagnumApplication {
public:
    explicit VisClient(std::string initial_host_address, const Arguments& arguments);
    ~VisClient() override;

private:
    void update() override;
    void render(const display::CameraPackage& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    void on_state_change();

    // General Info
    std::string gl_version_str_;
    std::string gl_renderer_str_;

    // Errors
    gui::ErrorAlert error_alert_;

    // Networking
    std::string server_address_input_ = "address:port";

    using Service = net::Scene;
    std::unique_ptr<grpcw::client::GrpcClient<Service>> grpc_client_;

    // Debugging
    bool run_as_fast_as_possible_ = false;

    // Scene
    std::unique_ptr<display::LocalScene> scene_; // forward declaration
};

} // namespace gvs::vis
