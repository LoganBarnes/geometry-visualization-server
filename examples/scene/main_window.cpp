// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2019 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#include "main_window.h"

// external
#include <Magnum/GL/Context.h>
//#include <gvs/vis-client/imgui_utils.hpp>
#include <imgui.h>

// standard
#include <random>

using namespace Magnum;

namespace example {

MainWindow::MainWindow(const Arguments& arguments)
    : gvs::vis::ImGuiMagnumApplication(arguments,
                                       Configuration{}
                                           .setTitle("Machine Emulator")
                                           .setSize({1280, 720})
                                           .setWindowFlags(Configuration::WindowFlag::Resizable)),
      // Device info
      gl_version_str_(GL::Context::current().versionString()),
      gl_renderer_str_(GL::Context::current().rendererString()) {

    std::vector<gvs::vec3> sphere;

    {
        float u, theta, coeff;
        std::mt19937 gen{std::random_device{}()};
        std::uniform_real_distribution<float> u_dist(-1.f, 1.f);
        std::uniform_real_distribution<float> theta_dist(0.f, 2.f * M_PIf32);

        for (int i = 0; i < 5000; ++i) {
            u = u_dist(gen);
            theta = theta_dist(gen);
            coeff = std::sqrt(1.f - u * u);
            sphere.push_back({coeff * std::cos(theta), coeff * std::sin(theta), u});
        }
    }

    {
        auto stream = scene_.item_stream("sphere").send(gvs::SetPositions3d(sphere),
                                                        gvs::SetNormals3d(sphere),
                                                        gvs::SetShading(gvs::LambertianShading()),
                                                        gvs::SetColoring(gvs::Coloring::Normals));

        if (!stream.success()) {
            error_message_ += stream.error_message();
        }
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {
    // Nothing to do
}

void MainWindow::render(const gvs::vis::CameraPackage& camera_package) const {
    scene_.render(camera_package);
}

void MainWindow::configure_gui() {
    auto add_three_line_separator = [] {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    };

    auto width = static_cast<float>(this->windowSize().x());
    auto height = static_cast<float>(this->windowSize().y());
    ImGui::SetNextWindowPos({0.f, 0.f});
    ImGui::SetNextWindowSizeConstraints({0.f, height}, {std::numeric_limits<float>::infinity(), height});
    ImGui::Begin("Settings", nullptr, {350.f, height});

    ImGui::Text("GL Version:   ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_version_str_.c_str());

    ImGui::Text("GL Renderer:  ");
    ImGui::SameLine();
    ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, "%s\t", gl_renderer_str_.c_str());

    add_three_line_separator();

    // SceneUtil::configure_gui(&scene_);

    ImGui::End();

    if (not error_message_.empty()) {
        ImGui::SetNextWindowPos({width * 0.5f, 0.f}, 0, {0.5f, 0.f});

        bool open = true;
        ImGui::Begin("Errors", &open, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
        ImGui::TextColored({1.f, 0.f, 0.f, 1.f}, "%s", error_message_.c_str());
        ImGui::End();

        if (not open) {
            error_message_ = "";
        }
    }
}

void MainWindow::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

} // namespace example

auto main(int argc, char* argv[]) -> int {
    example::MainWindow app({argc, argv});
    return app.exec();
}
