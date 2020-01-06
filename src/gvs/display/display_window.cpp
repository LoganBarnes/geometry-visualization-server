#include <utility>

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
#include "display_window.hpp"

// project
#include "display_scene.hpp"
#include "gvs/display/backends/opengl_backend.hpp"
#include "gvs/gui/scene_gui.hpp"
#include "gvs/vis-client/app/imgui_theme.hpp"

// external
#include <Magnum/GL/Context.h>
#include <imgui.h>

// standard
#include <limits>

using namespace Magnum;

namespace gvs::display {
namespace {

int   fake_argc    = 1;
char  fake_arg[4]  = {'t', 'e', 's', 't'};
char* fake_argv[1] = {fake_arg};

} // namespace

DisplayWindow::DisplayWindow(DisplayScene& parent_scene)
    : vis::ImGuiMagnumApplication(Arguments{fake_argc, fake_argv},
                                  Configuration{}
                                      .setTitle("Geometry Visualisation Client")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()),
      parent_scene_(parent_scene),
      scene_backend_(std::make_unique<backends::OpenglBackend>()) {

    camera_package_.zoom_object.translate({0.f, 0.f, 20.f});
    camera_package_.rotation_object.rotateX(Math::Deg<float>(/*pitch*/ -15));
    camera_package_.rotation_object.rotateY(Math::Deg<float>(/*yaw*/ 30));

    camera_package_.update_object();
}

DisplayWindow::~DisplayWindow() = default;

auto DisplayWindow::thread_safe_update(SceneUpdateFunc update_func) -> void {
    std::lock_guard lock(update_lock_);
    update_functions_.emplace_back(std::move(update_func));
    this->reset_draw_counter();
}

void DisplayWindow::update() {
    std::lock_guard lock(update_lock_);

    // Process an update
    constexpr auto max_updates_per_step = 10u;
    for (auto update = 0u; !update_functions_.empty() && update < max_updates_per_step; ++update) {
        auto update_func = std::move(update_functions_.front());
        update_functions_.pop_front();
        update_func(scene_backend_.get());
    }

    // Trigger another update if more updates are needed still
    if (!update_functions_.empty()) {
        this->reset_draw_counter();
    }
}

void DisplayWindow::render(CameraPackage const& camera_package) const {
    scene_backend_->render(camera_package);
}

void DisplayWindow::configure_gui() {

    auto add_three_line_separator = [] {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    };

    auto height = static_cast<float>(this->windowSize().y());
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints(ImVec2(0.f, height), ImVec2(std::numeric_limits<float>::infinity(), height));
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("GL Version:   ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_version_str_.c_str());

    ImGui::Text("GL Renderer:  ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_renderer_str_.c_str());

    add_three_line_separator();

    gui::configure_gui(&parent_scene_);

    ImGui::End();

    if (not error_message_.empty()) {
        int w = this->windowSize().x();
        ImGui::SetNextWindowPos({static_cast<float>(w) * 0.5f, 0.f}, 0, {0.5f, 0.f});

        bool open = true;
        ImGui::Begin("Errors", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        ImGui::TextColored({1.f, 0.f, 0.f, 1.f}, "%s", error_message_.c_str());
        ImGui::End();

        if (not open) {
            error_message_ = "";
        }
    }
}

void DisplayWindow::resize(const Vector2i& viewport) {
    scene_backend_->resize(viewport);
}

} // namespace gvs::display
