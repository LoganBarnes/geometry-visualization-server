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

#include "gvs/display/camera_package.hpp"
#include "gvs/forward_declarations.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/Platform/GlfwApplication.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

#include <memory>

namespace gvs::vis {

class ImGuiMagnumApplication : public Magnum::Platform::Application {
public:
    explicit ImGuiMagnumApplication(const Arguments& arguments, const Configuration& configuration);
    explicit ImGuiMagnumApplication(const Arguments& arguments);
    virtual ~ImGuiMagnumApplication();

private:
    virtual auto update() -> void                                                   = 0;
    virtual auto render(const display::CameraPackage& camera_package) const -> void = 0;
    virtual auto configure_gui() -> void                                            = 0;

    virtual auto resize(const Magnum::Vector2i& viewport) -> void = 0;

    auto drawEvent() -> void override;
    auto viewportEvent(ViewportEvent& event) -> void override;

    // keyboard
    auto keyPressEvent(KeyEvent& event) -> void override;
    auto keyReleaseEvent(KeyEvent& event) -> void override;
    auto textInputEvent(TextInputEvent& event) -> void override;

    // mouse
    auto mousePressEvent(MouseEvent& event) -> void override;
    auto mouseReleaseEvent(MouseEvent& event) -> void override;
    auto mouseMoveEvent(MouseMoveEvent& event) -> void override;
    auto mouseScrollEvent(MouseScrollEvent& event) -> void override;

    [[nodiscard]] auto position_on_arcball(Magnum::Vector2i const& position) const -> Magnum::Vector3;

    auto zoom(float amount) -> void;

    Magnum::ImGuiIntegration::Context imgui_{Magnum::NoCreate};
    int                               draw_counter_ = 1; // continue drawing until this counter is zero

    // Camera
    Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D> camera_scene_;

    Magnum::Vector2 previous_mouse_position_     = {};
    Magnum::Matrix4 previous_arcball_transform_  = {};
    Magnum::Vector3 previous_arcball_position_   = {};
    Magnum::Vector3 previous_camera_orbit_point_ = {};

protected:
    // forward declaration
    std::unique_ptr<detail::Theme> theme_;

    // Camera
    display::CameraPackage camera_package_;

    // Ensures the application renders at least 5 more times after all events are
    // finished to give ImGui a chance to update and render correctly.
    auto reset_draw_counter() -> void;
};

} // namespace gvs::vis
