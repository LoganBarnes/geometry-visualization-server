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
#include "main_window.hpp"

// project
#include "gvs/display/magnum_conversions.hpp"
#include "gvs/gui/scene_gui.hpp"
#include "gvs/scene/log_params.hpp"

// external
#include <Magnum/GL/Context.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Trade/MeshData3D.h>
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
      gl_renderer_str_(GL::Context::current().rendererString()),
      error_alert_("Error Popup") {

    {
        axes_.scene_id = scene_.add_item(gvs::SetReadableId("Axes"),
                                         gvs::SetPositions3d(),
                                         gvs::SetColoring(gvs::Coloring::VertexColors),
                                         gvs::SetShading(gvs::Shading::UniformColor));
        reset_axes();
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +3.0f; // x
        translation[13]  = +0.0f; // y
        translation[14]  = +0.0f; // z

        cone_.scene_id = scene_.add_item(gvs::SetReadableId("Cone"),
                                         gvs::SetPositions3d(),
                                         gvs::SetTransformation(translation),
                                         gvs::SetShading(gvs::Shading::Lambertian));
        reset_cone();
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = -3.0f; // x
        translation[13]  = +0.0f; // y
        translation[14]  = +0.0f; // z

        cube_.scene_id = scene_.add_item(gvs::SetReadableId("Cube"),
                                         gvs::SetPositions3d(),
                                         gvs::SetTransformation(translation),
                                         gvs::SetShading(gvs::Shading::Lambertian));
        reset_cube();
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +0.0f; // x
        translation[13]  = +3.0f; // y
        translation[14]  = +0.0f; // z

        cylinder_.scene_id = scene_.add_item(gvs::SetReadableId("Cylinder"),
                                             gvs::SetPositions3d(),
                                             gvs::SetTransformation(translation),
                                             gvs::SetShading(gvs::Shading::Lambertian));
        reset_cylinder();
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +0.0f; // x
        translation[13]  = +0.0f; // y
        translation[14]  = -3.0f; // z

        plane_.scene_id = scene_.add_item(gvs::SetReadableId("Quad"),
                                          gvs::SetPositions3d(),
                                          gvs::SetTransformation(translation),
                                          gvs::SetShading(gvs::Shading::Lambertian));
        reset_plane();
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +0.0f; // x
        translation[13]  = -3.0f; // y
        translation[14]  = +0.0f; // z

        sphere_.scene_id = scene_.add_item(gvs::SetReadableId("Sphere"),
                                           gvs::SetPositions3d(),
                                           gvs::SetTransformation(translation),
                                           gvs::SetShading(gvs::Shading::Lambertian));
        reset_sphere();
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::update() {
    // Nothing to do
}

void MainWindow::render(const gvs::display::CameraPackage& camera_package) const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    scene_.render(camera_package);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void MainWindow::configure_gui() {
    const ImVec4 gray = {0.5f, 0.5f, 0.5f, 1.f};

    auto add_three_line_separator = [] {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Separator();
        ImGui::Spacing();
    };

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

    if (ImGui::CollapsingHeader("Primitive Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
        constexpr unsigned min_val  = 3u;
        constexpr unsigned max_val  = 250u;
        constexpr float    min_valf = 0.1f;
        constexpr float    max_valf = 5.0f;

        /*
         * Cone
         */
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Cone")) {

            bool cone_updated = false;
            cone_updated |= ImGui::DragScalar("Rings", ImGuiDataType_U32, &cone_.rings, 0.5f, &min_val, &max_val);
            cone_updated |= ImGui::DragScalar("Segments", ImGuiDataType_U32, &cone_.segments, 0.5f, &min_val, &max_val);
            cone_updated |= ImGui::DragScalar("Half Length",
                                              ImGuiDataType_Float,
                                              &cone_.half_length,
                                              0.1f,
                                              &min_valf,
                                              &max_valf);

            if (cone_updated) {
                reset_cone();
            }
            ImGui::TreePop();
        }

        /*
         * Cube
         */
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Cube")) {
            ImGui::TextColored(gray, "Nothing to configure");
            ImGui::TreePop();
        }

        /*
         * Cylinder
         */
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Cylinder")) {

            bool cylinder_updated = false;
            cylinder_updated
                |= ImGui::DragScalar("Rings", ImGuiDataType_U32, &cylinder_.rings, 0.5f, &min_val, &max_val);
            cylinder_updated
                |= ImGui::DragScalar("Segments", ImGuiDataType_U32, &cylinder_.segments, 0.5f, &min_val, &max_val);
            cylinder_updated |= ImGui::DragScalar("Half Length",
                                                  ImGuiDataType_Float,
                                                  &cylinder_.half_length,
                                                  0.1f,
                                                  &min_valf,
                                                  &max_valf);

            if (cylinder_updated) {
                reset_cylinder();
            }
            ImGui::TreePop();
        }

        /*
         * Plane
         */
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Plane")) {
            ImGui::TextColored(gray, "Nothing to configure");
            ImGui::TreePop();
        }

        /*
         * Sphere
         */
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
        if (ImGui::TreeNode("Sphere")) {

            bool sphere_updated = false;
            sphere_updated |= ImGui::DragScalar("Rings", ImGuiDataType_U32, &sphere_.rings, 0.5f, &min_val, &max_val);
            sphere_updated
                |= ImGui::DragScalar("Segments", ImGuiDataType_U32, &sphere_.segments, 0.5f, &min_val, &max_val);

            if (sphere_updated) {
                reset_sphere();
            }
            ImGui::TreePop();
        }
    }

    if (ImGui::CollapsingHeader("Scene")) {
        gvs::gui::configure_gui(&scene_);
    }

    ImGui::End();

    error_alert_.display_next_error();
}

void MainWindow::resize(const Vector2i& viewport) {
    scene_.resize(viewport);
}

auto MainWindow::reset_axes() -> void {
    Trade::MeshData3D const axes = Primitives::axis3D();

    auto const& colors = axes.colors(0);

    std::vector<Magnum::Color3> vertex_colors;
    vertex_colors.reserve(colors.size());
    for (auto const& color4 : colors) {
        vertex_colors.emplace_back(color4.r(), color4.g(), color4.b());
    }

    scene_.update_item(axes_.scene_id,
                       gvs::SetPositions3d(axes.positions(0)),
                       gvs::SetVertexColors3d(vertex_colors),
                       gvs::SetIndices(axes.indices()),
                       gvs::SetGeometryFormat(gvs::display::from_magnum(axes.primitive())));
}

auto MainWindow::reset_cone() -> void {
    scene_.update_item(cone_.scene_id, gvs::SetPrimitive(gvs::Cone{cone_.half_length, cone_.rings, cone_.segments}));
}

auto MainWindow::reset_cube() -> void {
    scene_.update_item(cube_.scene_id, gvs::SetPrimitive(gvs::Cube{}));
}

auto MainWindow::reset_cylinder() -> void {
    scene_.update_item(cylinder_.scene_id,
                       gvs::SetPrimitive(gvs::Cylinder{cylinder_.half_length, cylinder_.rings, cylinder_.segments}));
}

auto MainWindow::reset_plane() -> void {
    scene_.update_item(plane_.scene_id, gvs::SetPrimitive(gvs::Plane{}));
}

auto MainWindow::reset_sphere() -> void {
    scene_.update_item(sphere_.scene_id, gvs::SetPrimitive(gvs::Sphere{sphere_.rings, sphere_.segments}));
}

} // namespace example

auto main(int argc, char* argv[]) -> int {
    example::MainWindow app({argc, argv});
    return app.exec();
}
