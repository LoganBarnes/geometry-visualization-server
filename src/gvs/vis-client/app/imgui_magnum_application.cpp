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

// 'p' is a point on a plane
float intersect_plane(display::Ray const& ray, Vector3 p, const Vector3& plane_normal) {
    // plane ray intersection
    float const denom = Math::dot(ray.direction, plane_normal);

    p -= ray.origin;
    float const t = Math::dot(p, plane_normal) / denom;

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

    camera_package_.object.setParent(&camera_scene_);
    camera_package_.zoom_object.translate(Vector3::zAxis(5.0f));
    camera_package_.update_object();

    // Memory ownership is handled elsewhere
    camera_package_.set_camera(new SceneGraph::Camera3D(camera_package_.object),
                               GL::defaultFramebuffer.viewport().size());

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
        camera_package_.translation_object.resetTransformation();
        camera_package_.update_object();
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

    previous_mouse_data_.zoom_transform        = camera_package_.zoom_object.transformation();
    previous_mouse_data_.rotation_transform    = camera_package_.rotation_object.transformation();
    previous_mouse_data_.translation_transform = camera_package_.translation_object.transformation();

    previous_mouse_data_.arcball = arcball_info(Vector2(event.position()), previous_mouse_data_.rotation_transform);

    pan_pos_ = world_position(Vector2(event.position()), camera_package_.rotation_object.transformation(), 0.f);
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

    // make these editable settings
    auto orbiting = (event.buttons() & MouseMoveEvent::Button::Left);
    auto panning  = (event.buttons() & MouseMoveEvent::Button::Middle);
    auto zooming  = (event.buttons() & MouseMoveEvent::Button::Right);

    if (!(orbiting || panning || zooming)) {
        return;
    }

    if (orbiting) {

        auto const current_arcball = arcball_info(Vector2(event.position()), previous_mouse_data_.rotation_transform);

        Vector3 const& previous_arcball_position = previous_mouse_data_.arcball.world_space_position;
        Vector3 const& current_arcball_position  = current_arcball.world_space_position;

        Vector3 const axis = Math::cross(previous_arcball_position, current_arcball_position);

        if (axis.length() < 0.001f) {
            return;
        }

        // TODO: scale this depending on screen size so it doesn't feel different if you change the window
        constexpr auto scale = 4.f; // make this a setting
        auto           angle = -scale * Math::angle(previous_arcball_position, current_arcball_position);

        camera_package_.rotation_object.setTransformation(previous_mouse_data_.rotation_transform);
        camera_package_.rotation_object.rotate(angle, axis.normalized());
        camera_package_.update_object();

    } else if (panning) {
        // TODO: clean this mess up
        auto const orbit_point
            = camera_package_.translation_object.transformation().transformPoint(Magnum::Vector3{0.f});

        auto world_from_mouse_pos = [&](const auto& mouse_pos, float* dist_to_plane) {
            auto ray          = camera_package_.get_camera_ray_from_window_pos(mouse_pos);
            auto plane_normal = (ray.origin - orbit_point).normalized();

            *dist_to_plane = intersect_plane(ray, orbit_point, plane_normal);
            return ray.origin + ray.direction * (*dist_to_plane);
        };

        float should_not_be_inf1, should_not_be_inf2;

        auto previous_world_pos
            = world_from_mouse_pos(previous_mouse_data_.arcball.screen_space_mouse_position, &should_not_be_inf1);
        auto current_world_pos = world_from_mouse_pos(Vector2(event.position()), &should_not_be_inf2);

        if (not std::isinf(should_not_be_inf1) and not std::isinf(should_not_be_inf2)) {
            auto diff = previous_world_pos - current_world_pos;

            camera_package_.translation_object.setTransformation(previous_mouse_data_.translation_transform);
            camera_package_.translation_object.translate(diff);
            camera_package_.update_object();
        }

    } else if (zooming) {
        constexpr auto drag_zoom_scale = 0.5f; // make this an editable setting

        zoom(event.relativePosition().y() * drag_zoom_scale);
    }
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

    constexpr auto wheel_zoom_scale = 10.f; // make this an editable setting

    zoom(event.offset().y() * wheel_zoom_scale);
}

auto ImGuiMagnumApplication::arcball_info(Magnum::Vector2 const& screen_space_mouse_position,
                                          Magnum::Matrix4 const& rotation_transform) const -> ArcballData {
    constexpr auto sphere_radius         = 2.f;
    constexpr auto sphere_radius_squared = sphere_radius * sphere_radius;

    Vector2 const normalized_pos = screen_space_mouse_position / Vector2{camera_package_.camera->viewport()};
    Vector2 const clipspace_pos  = (normalized_pos * 2.f) - Vector2{1.f}; // [-1, 1]
    float const   length_squared = dot(clipspace_pos, clipspace_pos);

    float z = (length_squared > sphere_radius_squared ? 0.f : std::sqrt(sphere_radius_squared - length_squared));

    auto const view_space_position  = Vector3(clipspace_pos.x(), -clipspace_pos.y(), z).normalized();
    auto const world_space_position = rotation_transform.transformVector(view_space_position).normalized();

    return {
        screen_space_mouse_position,
        view_space_position,
        world_space_position,
    };
}

auto ImGuiMagnumApplication::world_position(Magnum::Vector2 const& screen_space_mouse_position,
                                            Magnum::Matrix4 const& camera_transform,
                                            float                  clip_space_z) const -> Vector3 {

    Vector2 const normalized_pos = screen_space_mouse_position / Vector2{camera_package_.camera->viewport()};
    Vector2 const clipspace_pos  = (normalized_pos * 2.f) - Vector2{1.f}; // [-1, 1]

    auto const view_space_position  = Vector3(clipspace_pos.x(), -clipspace_pos.y(), clip_space_z);
    auto const world_space_position = camera_transform.transformVector(view_space_position);

    return world_space_position;
}

auto ImGuiMagnumApplication::zoom(float amount) -> void {
    auto const camera_pos = camera_package_.zoom_object.transformation().transformPoint(Magnum::Vector3{0.f});
    auto const length     = camera_pos.length();

    // if camera pos is too small use look
    auto direction = -camera_pos;
    if (length < 0.5f) {
        direction = camera_package_.zoom_object.transformation().transformPoint(Magnum::Vector3{0.f, 0.f, -1.f});
    }

    if (length < 0.001f && amount > 0.f) {
        return;
    }

    auto const move_dist   = std::max(length, 0.01f) * amount * 0.01f;
    auto const translation = direction.normalized() * std::min(move_dist, length);

    camera_package_.zoom_object.translate(translation);
    camera_package_.update_object();
}

} // namespace gvs::vis
