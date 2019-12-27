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
#include "scene_util.hpp"

// project
#include "log_params.hpp"
#include "scene.hpp"

// external
#include <gvs/vis-client/imgui_utils.hpp>
#include <imgui.h>

namespace gvs::scene {
namespace {

auto configure_scene_gui(SceneID const& item_id, Scene* scene) -> bool {
    std::string    readable_id;
    GeometryFormat geometry_format;
    mat4           transformation;
    vec3           uniform_color;
    Coloring       coloring;
    Shading        shading;
    bool           visible;
    float          opacity;

    bool has_geometry;
    bool has_children;

    scene->get_item_info(item_id,
                         GetReadableId(&readable_id),
                         GetGeometryFormat(&geometry_format),
                         GetTransformation(&transformation),
                         GetUniformColor(&uniform_color),
                         GetColoring(&coloring),
                         GetShading(&shading),
                         IsVisible(&visible),
                         GetOpacity(&opacity),
                         HasGeometry(&has_geometry),
                         HasChildren(&has_children));

    auto const      id_str = to_string(item_id);
    imgui::ScopedID scoped_id(id_str.c_str());

    bool item_changed     = false;
    bool children_changed = false;

    item_changed |= ImGui::Checkbox("###visible", &visible);
    ImGui::SameLine();

    if (ImGui::TreeNode(readable_id.c_str())) {
        ImGui::Indent();
        ImGui::Separator();

        if (visible) {
            if (has_geometry) {
                auto icoloring = std::underlying_type_t<Coloring>(coloring);
                if (ImGui::Combo("Display Mode",
                                 &icoloring,
                                 " Positions \0"
                                 " Normals \0"
                                 " Texture Coordinates \0"
                                 " Vertex Colors \0"
                                 " Uniform Color \0"
                                 " Texture \0"
                                 " White \0"
                                 "\0")) {
                    item_changed = true;
                    coloring     = static_cast<Coloring>(icoloring);
                }

                auto igeometry_format = std::underlying_type_t<GeometryFormat>(geometry_format);
                if (ImGui::Combo("Geometry Format",
                                 &igeometry_format,
                                 " Points \0"
                                 " Lines \0"
                                 " Line Strip \0"
                                 " Triangles \0"
                                 " Triangle Strip \0"
                                 " Triangle Fan \0"
                                 "\0")) {
                    item_changed    = true;
                    geometry_format = static_cast<GeometryFormat>(igeometry_format);
                }

                if (coloring == Coloring::UniformColor) {
                    item_changed |= ImGui::ColorEdit3("Global Color", uniform_color.data());
                }
            }

            if (has_children) {
                std::optional<std::vector<SceneID>> children;
                scene->get_item_info(item_id, GetChildren(&children));

                for (auto const& child_id : children.value()) {
                    children_changed |= configure_scene_gui(child_id, scene);
                }

            } else {
                ImGui::TextColored(ImVec4(1.f, 1.f, 0.1f, 1.f), "No Children");
            }
        }

        ImGui::Unindent();
        ImGui::TreePop();
    }

    if (item_changed) {
        scene->update_item(item_id,
                           SetReadableId(readable_id),
                           SetGeometryFormat(geometry_format),
                           SetTransformation(transformation),
                           SetUniformColor(uniform_color),
                           SetColoring(coloring),
                           SetShading(shading),
                           SetVisible(visible),
                           SetOpacity(opacity));
    }

    return item_changed | children_changed;
}

} // namespace

auto SceneUtil::configure_gui(Scene* scene) -> bool {
    ImGui::Text("Scene Items:");

    if (scene->empty()) {
        ImGui::SameLine();
        ImGui::Text(" (empty)");
        return false;
    }

    std::optional<std::vector<SceneID>> children;
    scene->get_item_info(nil_id, GetChildren(&children));

    if (!children) {
        return false;
    }

    bool scene_changed = false;

    for (auto const& child_id : children.value()) {
        scene_changed |= configure_scene_gui(child_id, scene);
    }

    return scene_changed;
}

} // namespace gvs::scene
