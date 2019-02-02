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
#include "imgui_magnum_application.hpp"

#include "gvs/vis-client/app/imgui_theme.hpp"
#include "gvs/vis-client/scene/opengl_scene.hpp"

#include <Corrade/Utility/Resource.h>
#include <Corrade/Utility/Unicode.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Angle.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/SceneGraph/Camera.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

static void initialize_resources() {
    CORRADE_RESOURCE_INITIALIZE(gvs_client_RESOURCES)
}

namespace gvs {
namespace vis {

using namespace Magnum;

namespace {

// 'p' is a point on a plane
float intersect_plane(const Ray& ray, Vector3 p, const Vector3& plane_normal) {
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
    : GlfwApplication(arguments, configuration, GLConfiguration().setSampleCount(4)) {
    initialize_resources();

    this->startTextInput(); // allow for text input callbacks

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    imgui_ = std::shared_ptr<ImGuiContext>(ImGui::CreateContext(), ImGui::DestroyContext);

    ImGuiIO& io = ImGui::GetIO();

    // Use a shared pointer to handle ImGui initialization and destruction via RAII
    auto init_imgui_gl = [this]() {
        bool success = true;
        success &= ImGui_ImplGlfw_InitForOpenGL(this->window(), false);
        success &= ImGui_ImplOpenGL3_Init();
        return new bool(success);
    };

    imgui_gl_ = std::shared_ptr<bool>(init_imgui_gl(), [](const bool* p) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        delete p;
    });

    // Some fonts are loaded in the theme class but we might also want the default so it's loaded here
    io.Fonts->AddFontDefault();
    theme_ = std::make_unique<detail::Theme>();

    theme_->set_style();
    GL::Renderer::setClearColor({theme_->background.Value.x,
                                 theme_->background.Value.y,
                                 theme_->background.Value.z,
                                 theme_->background.Value.w});

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

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushFont(theme_->font);
    configure_gui();
    ImGui::PopFont();

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);

    render(camera_package_);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    swapBuffers();

    if (draw_counter_-- > 0) {
        redraw();
    }
}

void ImGuiMagnumApplication::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.windowSize()});

    camera_package_.update_viewport(event.windowSize());

    resize(event.windowSize());
    reset_draw_counter();
}

void ImGuiMagnumApplication::keyPressEvent(KeyEvent& event) {
    if (not event.isRepeated()) {
        ImGui_ImplGlfw_KeyCallback(this->window(),
                                   static_cast<int>(event.key()),
                                   -1,
                                   GLFW_PRESS,
                                   static_cast<int>(event.modifiers()));
    }
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::keyReleaseEvent(KeyEvent& event) {
    ImGui_ImplGlfw_KeyCallback(this->window(),
                               static_cast<int>(event.key()),
                               -1,
                               GLFW_RELEASE,
                               static_cast<int>(event.modifiers()));

    using KE = KeyEvent;

    // Ctrl + Shift + Q == Exit
    if (event.modifiers() & KE::Modifier::Ctrl and event.modifiers() & KE::Modifier::Shift
        and event.key() == KE::Key::Q) {

        this->exit();
    }

    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::textInputEvent(TextInputEvent& event) {
    char32_t codepoint;
    std::tie(codepoint, std::ignore) = Utility::Unicode::nextChar(event.text(), 0);

    ImGui_ImplGlfw_CharCallback(this->window(), codepoint);

    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::mousePressEvent(MouseEvent& event) {
    ImGui_ImplGlfw_MouseButtonCallback(this->window(),
                                       static_cast<int>(event.button()),
                                       GLFW_PRESS,
                                       static_cast<int>(event.modifiers()));
    event.setAccepted(true);
    reset_draw_counter();

    if (not ImGui::GetIO().WantCaptureMouse) {
        previous_position_ = Vector2(event.position());
    }
}

void ImGuiMagnumApplication::mouseReleaseEvent(MouseEvent& event) {
    ImGui_ImplGlfw_MouseButtonCallback(this->window(),
                                       static_cast<int>(event.button()),
                                       GLFW_RELEASE,
                                       static_cast<int>(event.modifiers()));
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::mouseMoveEvent(MouseMoveEvent& event) {

    event.setAccepted(true);
    reset_draw_counter();

    if (not ImGui::GetIO().WantCaptureMouse) {
        auto current_position = Vector2(event.position());

        if (event.buttons() & MouseMoveEvent::Button::Left) {
            camera_yaw_and_pitch_ += (previous_position_ - current_position) * 0.005f;

        } else if (event.buttons() & MouseMoveEvent::Button::Middle) {
            // pan camera

            auto world_from_mouse_pos = [&](const auto& mouse_pos, float* dist_to_plane) {
                auto ray = camera_package_.get_camera_ray_from_window_pos(mouse_pos);
                auto plane_normal = (ray.origin - camera_orbit_point_).normalized();

                *dist_to_plane = intersect_plane(ray, camera_orbit_point_, plane_normal);
                return ray.origin + ray.direction * (*dist_to_plane);
            };

            float should_not_be_inf1, should_not_be_inf2;

            auto previous_world_pos = world_from_mouse_pos(previous_position_, &should_not_be_inf1);
            auto current_world_pos = world_from_mouse_pos(current_position, &should_not_be_inf2);

            if (not std::isinf(should_not_be_inf1) and not std::isinf(should_not_be_inf2)) {
                auto diff = current_world_pos - previous_world_pos;
                camera_orbit_point_ -= diff;
            }
        }

        previous_position_ = current_position;
        update_camera();
    }
}

void ImGuiMagnumApplication::mouseScrollEvent(MouseScrollEvent& event) {
    ImGui_ImplGlfw_ScrollCallback(this->window(), event.offset().x(), event.offset().y());

    if (event.offset().y() == 0.f) {
        return;
    }

    camera_orbit_distance_ += (event.offset().y() > 0.f ? event.offset().y() / 0.85f : event.offset().y() * 0.85f);
    camera_orbit_distance_ = std::max(0.f, camera_orbit_distance_);

    update_camera();

    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::update_camera() {
    camera_package_.transformation = Matrix4::translation(camera_orbit_point_)
        * Matrix4::rotation(Math::Rad<float>(camera_yaw_and_pitch_.x()), {0.f, 1.f, 0.f})
        * Matrix4::rotation(Math::Rad<float>(camera_yaw_and_pitch_.y()), {1.f, 0.f, 0.f})
        * Matrix4::translation({0.f, 0.f, camera_orbit_distance_});

    camera_package_.object.setTransformation(camera_package_.transformation);
}

} // namespace vis
} // namespace gvs
