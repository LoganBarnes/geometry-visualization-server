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
#include "gvs/server/scene_server.hpp"
#include "gvs/vis-client/app/imgui_theme.hpp"
#include "gvs/vis-client/scene/opengl_scene.hpp"

// third-party
#include <Magnum/GL/Context.h>
#include <imgui.h>

// standard
#include <limits>

using namespace Magnum;

namespace gvs::display {
namespace {
int fake_argc = 1;
char fake_arg[4] = {'t', 'e', 's', 't'};
char* fake_argv[1] = {fake_arg};

constexpr auto poison_pill = nullptr;

} // namespace

DisplayWindow::DisplayWindow(util::BlockingQueue<SceneUpdateFunc>& update_queue)
    : vis::ImGuiMagnumApplication(Arguments{fake_argc, fake_argv},
                                  Configuration{}
                                      .setTitle("Geometry Visualisation Client")
                                      .setSize({1280, 720})
                                      .setWindowFlags(Configuration::WindowFlag::Resizable)),
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()),
      external_update_queue_(update_queue) {

    // scene_ = std::make_unique<vis::OpenGLScene>(vis::make_scene_init_info(theme_->background, this->windowSize()));

    update_thread_ = std::thread([this] {
        while (auto&& update = external_update_queue_.pop_front()) {
            internal_update_queue_.push_back(std::move(update));
            this->reset_draw_counter();
        }
    });
}

DisplayWindow::~DisplayWindow() {
    external_update_queue_.emplace_back(poison_pill);
    update_thread_.join();
}

void DisplayWindow::update() {
    if (!internal_update_queue_.empty()) {
        auto update = internal_update_queue_.pop_front();
        // update(scene_.get());
    }
    // scene_->update(this->windowSize());
}

void DisplayWindow::render(const vis::CameraPackage& /*camera_package*/) const {
    // if (!scene_->empty()) {
    //     scene_->render(camera_package);
    // }
}

void DisplayWindow::configure_gui() {

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

    ImGui::Checkbox("Run app as fast as possible", &run_as_fast_as_possible_);

    if (run_as_fast_as_possible_) {
        ImGui::TextColored({0.f, 1.f, 0.f, 1.f}, "FPS: %.3f", static_cast<double>(ImGui::GetIO().Framerate));
        reset_draw_counter();
    }

    add_three_line_separator();

    // scene_->configure_gui(this->windowSize());

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

void DisplayWindow::resize(const Vector2i& /*viewport*/) {
    // scene_->resize(viewport);
}

} // namespace gvs::display
