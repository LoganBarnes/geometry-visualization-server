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

// project
#include "gvs/vis-client/app/imgui_theme.hpp"

// external
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

// standard
#include <algorithm>

static void initialize_resources() {
    CORRADE_RESOURCE_INITIALIZE(gvs_client_RESOURCES)
}

namespace gvs::vis {

using namespace Magnum;

namespace {

//// 'p' is a point on a plane
//float intersect_plane(const display::Ray& ray, Vector3 p, const Vector3& plane_normal) {
//    // plane ray intersection
//    const float denom = Math::dot(ray.direction, plane_normal);
//
//    p -= ray.origin;
//    const float t = Math::dot(p, plane_normal) / denom;
//
//    if (t >= 0.f && t != std::numeric_limits<float>::infinity()) {
//        return t;
//    }
//    return std::numeric_limits<float>::infinity();
//}

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

auto ImGuiMagnumApplication::reset_draw_counter() -> void {
    draw_counter_ = 5;
    redraw();
}

auto ImGuiMagnumApplication::drawEvent() -> void {
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

auto ImGuiMagnumApplication::viewportEvent(ViewportEvent& event) -> void {
    reset_draw_counter();
    GL::defaultFramebuffer.setViewport({{}, event.windowSize()});

    imgui_.relayout(Vector2{event.windowSize()} / event.dpiScaling(), event.windowSize(), event.framebufferSize());

    camera_package_.update_viewport(event.windowSize());

    resize(event.windowSize());
}

auto ImGuiMagnumApplication::keyPressEvent(KeyEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleKeyPressEvent(event)) {
        return;
    }
    event.setAccepted(true);
}

auto ImGuiMagnumApplication::keyReleaseEvent(KeyEvent& event) -> void {
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

auto ImGuiMagnumApplication::textInputEvent(TextInputEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleTextInputEvent(event)) {
        return;
    }
    event.setAccepted(true);
}

auto ImGuiMagnumApplication::mousePressEvent(MouseEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMousePressEvent(event)) {
        return;
    }
    event.setAccepted(true);

    previous_arcball_transform_ = camera_package_.object.transformation();
    previous_arcball_position_  = position_on_arcball(event.position());
}

auto ImGuiMagnumApplication::mouseReleaseEvent(MouseEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMouseReleaseEvent(event)) {
        return;
    }
    event.setAccepted(true);
}

auto ImGuiMagnumApplication::mouseMoveEvent(MouseMoveEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMouseMoveEvent(event)) {
        return;
    }
    event.setAccepted(true);

    auto current_arcball_position = position_on_arcball(event.position());

    if (event.buttons() & MouseMoveEvent::Button::Left) {
        //        camera_yaw_and_pitch_ += (previous_position_ - current_position) * 0.005f;
        //        auto prev_pos4 = previous_arcball_transform_ * Magnum::Vector4(previous_arcball_position_, 1.f);
        //        auto curr_pos4
        //            = camera_package_.object.transformation() * Magnum::Vector4(current_arcball_position, 1.f);
        //
        //        auto prev_pos3 = Magnum::Vector3{prev_pos4.x(), prev_pos4.y(), prev_pos4.z()}.normalized();
        //        auto curr_pos3 = Magnum::Vector3{curr_pos4.x(), curr_pos4.y(), curr_pos4.z()}.normalized();

        Vector3 const axis = Math::cross(previous_arcball_position_, current_arcball_position);
        //        Vector3 const axis = Math::cross(prev_pos3, curr_pos3);

        assert(previous_arcball_position_.length() > 0.001f);
        if (axis.length() < 0.001f) {
            return;
        }

        auto angle = -2.f * Math::angle(previous_arcball_position_, current_arcball_position);
        //        auto angle = -2.f * Math::angle(prev_pos3, curr_pos3);

        camera_package_.object.setTransformation(previous_arcball_transform_);
        camera_package_.object.rotate(angle, axis.normalized());

    } else if (event.buttons() & MouseMoveEvent::Button::Middle) {
        // pan camera

        //        auto world_from_mouse_pos = [&](const auto& mouse_pos, float* dist_to_plane) {
        //            auto ray          = camera_package_.get_camera_ray_from_window_pos(mouse_pos);
        //            auto plane_normal = (ray.origin - camera_orbit_point_).normalized();
        //
        //            *dist_to_plane = intersect_plane(ray, camera_orbit_point_, plane_normal);
        //            return ray.origin + ray.direction * (*dist_to_plane);
        //        };
        //
        //        float should_not_be_inf1, should_not_be_inf2;
        //
        //        auto previous_world_pos = world_from_mouse_pos(previous_position_, &should_not_be_inf1);
        //        auto current_world_pos  = world_from_mouse_pos(current_position, &should_not_be_inf2);
        //
        //        if (not std::isinf(should_not_be_inf1) and not std::isinf(should_not_be_inf2)) {
        //            auto diff = current_world_pos - previous_world_pos;
        //            camera_orbit_point_ -= diff;
        //        }
    }

    //    previous_arcball_position_ = current_arcball_position;
    //    update_camera();
}

auto ImGuiMagnumApplication::mouseScrollEvent(MouseScrollEvent& event) -> void {
    reset_draw_counter();
    if (imgui_.handleMouseScrollEvent(event)) {
        return;
    }
    event.setAccepted(true);

    if (event.offset().y() == 0.f) {
        return;
    }

    //    camera_orbit_distance_ += (event.offset().y() > 0.f ? event.offset().y() / 0.85f : event.offset().y() * 0.85f);
    //    camera_orbit_distance_ = std::max(0.f, camera_orbit_distance_);

    update_camera();
}

#define BIG_CIRCLE
auto ImGuiMagnumApplication::position_on_arcball(Vector2i const& position) const -> Vector3 {
#ifndef BIG_CIRCLE
    constexpr auto threshold_radius         = 0.5f;
    constexpr auto threshold_radius_squared = threshold_radius * threshold_radius;
#endif
    constexpr auto sphere_radius         = 1.f;
    constexpr auto sphere_radius_squared = sphere_radius * sphere_radius;

    Vector2 const normalized_pos = Vector2{position} / Vector2{camera_package_.camera->viewport()};
#ifdef BIG_CIRCLE
    Vector2 const clipspace_pos = normalized_pos - Vector2{0.5f}; // [-0.5, 0.5]
#else
    Vector2 const clipspace_pos = (normalized_pos * 2.f) - Vector2{1.f}; // [-1, 1]
#endif
    float const length_squared = dot(clipspace_pos, clipspace_pos);

    float z;
#ifdef BIG_CIRCLE
    z = std::sqrt(sphere_radius_squared - length_squared);
#else
    if (length_squared > threshold_radius_squared) {
        auto const     threshold_z = std::sqrt(sphere_radius_squared - threshold_radius);
        constexpr auto max_length  = 2.f;
        auto const     range       = max_length - threshold_radius;

        auto const t = 1.f - (std::sqrt(length_squared) - threshold_z) / range;

        // Quadratic curve
        z = t * t * threshold_z;
    } else {
        z = std::sqrt(sphere_radius_squared - length_squared);
    }
#endif

    auto const view_space  = Vector3(clipspace_pos.x(), -clipspace_pos.y(), z).normalized();
    auto const world_space = previous_arcball_transform_.transformVector(view_space).normalized();

    Debug() << "World: " << world_space << ", View: " << view_space;

    return world_space;
}

auto ImGuiMagnumApplication::update_camera() -> void {
    //    camera_package_.transformation = Matrix4::translation(camera_orbit_point_)
    //        * Matrix4::rotation(Math::Rad<float>(camera_yaw_and_pitch_.x()), {0.f, 1.f, 0.f})
    //        * Matrix4::rotation(Math::Rad<float>(camera_yaw_and_pitch_.y()), {1.f, 0.f, 0.f})
    //        * Matrix4::translation({0.f, 0.f, camera_orbit_distance_});

    static bool first_time = true;
    if (first_time) {
        auto transformation = Matrix4::translation(camera_orbit_point_)
            * Matrix4::rotation(Math::Deg<float>(/*yaw*/ 30), {0.f, 1.f, 0.f})
            * Matrix4::rotation(Math::Deg<float>(/*pitch*/ -15), {1.f, 0.f, 0.f})
            * Matrix4::translation({0.f, 0.f, /*orbit radius*/ 20.f});
        first_time = false;

        camera_package_.object.setTransformation(transformation);

    } else {
    }
}

} // namespace gvs::vis
