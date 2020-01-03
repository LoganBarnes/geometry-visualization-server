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
#include "scene_gui.hpp"

// project
#include "gvs/scene/log_params.hpp"
#include "gvs/scene/scene.hpp"

// external
#include <gvs/gui/imgui_utils.hpp>
#include <imgui.h>

namespace gvs::gui {
namespace {

auto configure_scene_gui(SceneId const& item_id, scene::Scene* scene) -> bool {
    const ImVec4 gray = {0.5f, 0.5f, 0.5f, 1.f};

    std::string    readable_id;
    GeometryFormat geometry_format;
    mat4           transformation;
    vec3           uniform_color;
    Coloring       coloring;
    Shading        shading;
    bool           visible;
    float          opacity;
    bool           wireframe_only;

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
                         IsWireframeOnly(&wireframe_only),
                         HasGeometry(&has_geometry),
                         HasChildren(&has_children));

    auto const id_str = to_string(item_id);
    ScopedID   scoped_id(id_str.c_str());

    bool item_changed     = false;
    bool children_changed = false;

    item_changed |= ImGui::Checkbox("###visible", &visible);
    ImGui::SameLine();

    auto tree_label = readable_id + "###" + id_str;

    if (ImGui::TreeNode(tree_label.c_str())) {
        ScopedIndent scoped_indent;
        ImGui::Separator();

        item_changed |= configure_gui("###readable_id", &readable_id);

        if (visible) {
            if (has_geometry) {
                item_changed |= ImGui::Checkbox("Wireframe", &wireframe_only);

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

                auto ishading = std::underlying_type_t<Shading>(shading);
                if (ImGui::Combo("Shading",
                                 &ishading,
                                 " Uniform Color \0"
                                 " Lambertian \0"
                                 " Cook-Torrance \0"
                                 "\0")) {
                    item_changed = true;
                    shading      = static_cast<Shading>(ishading);
                }

                if (coloring == Coloring::UniformColor) {
                    item_changed |= ImGui::ColorEdit3("Global Color", uniform_color.data());
                }

                {
                    Disable::Guard disable_opacity(true /*wireframe_only*/);
                    item_changed |= ImGui::DragFloat("Opacity", &opacity, 0.01f, 0.f, 1.f);
                }
            }

            if (has_children) {
                ImGui::TextColored(gray, "Children:");

                std::vector<SceneId> children;
                scene->get_item_info(item_id, GetChildren(&children));

                for (auto const& child_id : children) {
                    children_changed |= configure_scene_gui(child_id, scene);
                }

            } else {
                ImGui::TextColored(ImVec4(1.f, 1.f, 0.1f, 1.f), "No Children");
            }
        }

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
                           SetOpacity(opacity),
                           SetWireframeOnly(wireframe_only));
    }

    return item_changed | children_changed;
}

} // namespace

auto configure_gui(scene::Scene* scene) -> bool {
    std::vector<SceneId> children;
    scene->get_item_info(nil_id(), GetChildren(&children));

    ImGui::Text("Scene Items:");

    if (children.empty()) {
        ImGui::SameLine();
        ImGui::TextColored({0.5f, 0.5f, 0.5f, 1.f}, " (empty)");
        return false;
    }

    bool scene_changed = false;

    for (auto const& child_id : children) {
        scene_changed |= configure_scene_gui(child_id, scene);
    }

    return scene_changed;
}

} // namespace gvs::gui
