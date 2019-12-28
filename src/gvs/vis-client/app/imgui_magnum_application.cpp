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
#include "imgui_magnum_application.hpp"

#include "gvs/vis-client/app/imgui_theme.hpp"

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/SceneGraph/Camera.h>
#include <imgui.h>

static void initialize_resources() {
    CORRADE_RESOURCE_INITIALIZE(gvs_client_RESOURCES)
}

namespace gvs::vis {

using namespace Magnum;

namespace {

// 'p' is a point on a plane
float intersect_plane(const display::Ray& ray, Vector3 p, const Vector3& plane_normal) {
    // plane ray intersection
    const float denom = Math::dot(ray.direction, plane_normal);

    p -= ray.origin;
    const float t = Math::dot(p, plane_normal) / denom;

    if (t >= 0.f && t != std::numeric_limits<float>::infinity()) {
        return t;
    }
    return std::numeric_limits<float>::infinity();
}

} // namespace

ImGuiMagnumApplication::ImGuiMagnumApplication(const Arguments& arguments, const Configuration& configuration)
    : GlfwApplication(arguments, configuration, GLConfiguration().setSampleCount(4)),
      imgui_(Vector2{windowSize()} / dpiScaling(), windowSize(), framebufferSize()) {
    initialize_resources();

    this->startTextInput(); // allow for text input callbacks

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();

    // Some fonts are loaded in the theme class but we might also want the default so it's loaded here
    theme_ = std::make_unique<detail::Theme>();

    theme_->set_style();
    GL::Renderer::setClearColor({theme_->background.Value.x,
                                 theme_->background.Value.y,
                                 theme_->background.Value.z,
                                 theme_->background.Value.w});

    /* Set up proper blending to be used by ImGui. There's a great chance
       you'll need this exact behavior for the rest of your scene. If not, set
       this only for the drawFrame() call. */
    GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add, GL::Renderer::BlendEquation::Add);
    GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                   GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    camera_package_.object.setParent(&camera_scene_).translate(Vector3::zAxis(5.0f));

    // Memory ownership is handled elsewhere
    camera_package_.set_camera(new SceneGraph::Camera3D(camera_package_.object),
                               GL::defaultFramebuffer.viewport().size());

    update_camera();
    reset_draw_counter();
}

ImGuiMagnumApplication::ImGuiMagnumApplication(const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("ImGui Magnum Application")
                                 .setWindowFlags(Configuration::WindowFlag::Resizable)) {}

ImGuiMagnumApplication::~ImGuiMagnumApplication() = default;

void ImGuiMagnumApplication::reset_draw_counter() {
    draw_counter_ = 5;
    redraw();
}

void ImGuiMagnumApplication::drawEvent() {
    update();

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    render(camera_package_);

    imgui_.newFrame();

    configure_gui();

    /* Set appropriate states. If you only draw imgui UI, it is sufficient to
       do this once in the constructor. */
    GL::Renderer::enable(GL::Renderer::Feature::Blending);
    GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

    imgui_.drawFrame();

    /* Reset state. Only needed if you want to draw something else with
       different state next frame. */
    GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    GL::Renderer::disable(GL::Renderer::Feature::Blending);

    swapBuffers();

    if (draw_counter_-- > 0) {
        redraw();
    }
}

void ImGuiMagnumApplication::viewportEvent(ViewportEvent& event) {
    reset_draw_counter();
    GL::defaultFramebuffer.setViewport({{}, event.windowSize()});

    imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());

    camera_package_.update_viewport(event.windowSize());

    resize(event.windowSize());
}

void ImGuiMagnumApplication::keyPressEvent(KeyEvent& event) {
    reset_draw_counter();
    if (imgui_.handleKeyPressEvent(event)) {
        return;
    }
    event.setAccepted(true);
}

void ImGuiMagnumApplication::keyReleaseEvent(KeyEvent& event) {
    reset_draw_counter();

    using KE = KeyEvent;

    // Ctrl + Shift + Q == Exit
    if (event.modifiers() & KE::Modifier::Ctrl and event.modifiers() & KE::Modifier::Shift
        and event.key() == KE::Key::Q) {

        this->exit();
        return;
    }

    if (imgui_.handleKeyReleaseEvent(event)) {
        return;
    }
    event.setAccepted(true);

    switch (event.key()) {

    case KE::Key::R: {
        camera_orbit_point_ = {0.f, 0.f, 0.f};
        update_camera();
    } break;

    default:
        break;
    }
}

void ImGuiMagnumApplication::textInputEvent(TextInputEvent& event) {
    reset_draw_counter();
    if (imgui_.handleTextInputEvent(event)) {
        return;
    }
    event.setAccepted(true);
}

void ImGuiMagnumApplication::mousePressEvent(MouseEvent& event) {
    reset_draw_counter();
    if (imgui_.handleMousePressEvent(event)) {
        return;
    }
    event.setAccepted(true);

    previous_position_ = Vector2(event.position());
}

void ImGuiMagnumApplication::mouseReleaseEvent(MouseEvent& event) {
    reset_draw_counter();
    if (imgui_.handleMouseReleaseEvent(event)) {
        return;
    }
    event.setAccepted(true);
}

void ImGuiMagnumApplication::mouseMoveEvent(MouseMoveEvent& event) {
    reset_draw_counter();
    if (imgui_.handleMouseMoveEvent(event)) {
        return;
    }
    event.setAccepted(true);

    auto current_position = Vector2(event.position());

    if (event.buttons() & MouseMoveEvent::Button::Left) {
        camera_yaw_and_pitch_ += (previous_position_ - current_position) * 0.005f;

    } else if (event.buttons() & MouseMoveEvent::Button::Middle) {
        // pan camera

        auto world_from_mouse_pos = [&](const auto& mouse_pos, float* dist_to_plane) {
            auto ray          = camera_package_.get_camera_ray_from_window_pos(mouse_pos);
            auto plane_normal = (ray.origin - camera_orbit_point_).normalized();

            *dist_to_plane = intersect_plane(ray, camera_orbit_point_, plane_normal);
            return ray.origin + ray.direction * (*dist_to_plane);
        };

        float should_not_be_inf1, should_not_be_inf2;

        auto previous_world_pos = world_from_mouse_pos(previous_position_, &should_not_be_inf1);
        auto current_world_pos  = world_from_mouse_pos(current_position, &should_not_be_inf2);

        if (not std::isinf(should_not_be_inf1) and not std::isinf(should_not_be_inf2)) {
            auto diff = current_world_pos - previous_world_pos;
            camera_orbit_point_ -= diff;
        }
    }

    previous_position_ = current_position;
    update_camera();
}

void ImGuiMagnumApplication::mouseScrollEvent(MouseScrollEvent& event) {
    reset_draw_counter();
    if (imgui_.handleMouseScrollEvent(event)) {
        return;
    }
    event.setAccepted(true);

    if (event.offset().y() == 0.f) {
        return;
    }

    camera_orbit_distance_ += (event.offset().y() > 0.f ? event.offset().y() / 0.85f : event.offset().y() * 0.85f);
    camera_orbit_distance_ = std::max(0.f, camera_orbit_distance_);

    update_camera();
}

void ImGuiMagnumApplication::update_camera() {
    camera_package_.transformation = Matrix4::translation(camera_orbit_point_)
        * Matrix4::rotation(Math::Rad<float>(camera_yaw_and_pitch_.x()), {0.f, 1.f, 0.f})
        * Matrix4::rotation(Math::Rad<float>(camera_yaw_and_pitch_.y()), {1.f, 0.f, 0.f})
        * Matrix4::translation({0.f, 0.f, camera_orbit_distance_});

    camera_package_.object.setTransformation(camera_package_.transformation);
}

} // namespace gvs::vis
