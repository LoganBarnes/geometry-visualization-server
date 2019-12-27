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
    std::string readable_id;
    GeometryFormat geometry_format;
    mat4 transformation;
    vec3 uniform_color;
    Coloring coloring;
    Shading shading;
    bool visible;
    float opacity;

    bool has_geometry;
    bool has_children;

    auto result = scene->get_item_info(item_id,
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
    if (!result) {
        throw std::runtime_error(result.error().debug_error_message());
    }

    auto const id_str = to_string(item_id);
    imgui::ScopedID scoped_id(id_str.c_str());

    ImGui::Checkbox("###visible", &visible);
    ImGui::SameLine();

    if (ImGui::TreeNode(readable_id.c_str())) {
        ImGui::Indent();
        ImGui::Separator();

        //        if (visible) {
        //            if (has_geometry) {
        //                auto display_mode = static_cast<int>(shader_display_mode);
        //                if (ImGui::Combo("Display Mode",
        //                                 &display_mode,
        //                                 " POSITION \0"
        //                                 " NORMALS \0"
        //                                 " TEX_COORDS \0"
        //                                 " COLOR \0"
        //                                 " TEXTURE \0"
        //                                 " SIMPLE_SHADING \0"
        //                                 " ADVANCED_SHADING \0"
        //                                 " WHITE \0"
        //                                 "\0")) {
        //                    shader_display_mode = static_cast<DisplayMode>(display_mode);
        //                }
        //
        //                switch (shader_display_mode) {
        //                case DisplayMode::COLOR:
        //                case DisplayMode::SIMPLE_SHADING:
        //                case DisplayMode::ADVANCED_SHADING:
        //                    ImGui::ColorEdit3("Global Color", glm::value_ptr(global_color));
        //                    break;
        //                default:
        //                    break;
        //                }
        //            }
        //
        //            for (const uid& child : children) {
        //                configure_scene_gui(scene, child);
        //            }
        //            if (children.empty()) {
        //                ImGui::TextColored(ImVec4(1.f, 1.f, 0.1f, 1.f), "No Children");
        //            }
        //        }
        //
        ImGui::Unindent();
        ImGui::TreePop();
    }

    //    scene.update_item(id,
    //                      DisplayType(shader_display_mode),
    //                      Transform(transform),
    //                      ReadableID(readable_id),
    //                      Visible(visible),
    //                      GlobalColor(global_color));

    return false;
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
    auto result = scene->get_item_info(nil_id, GetChildren(&children));
    if (!result) {
        throw std::runtime_error(result.error().debug_error_message());
    }

    if (!children) {
        return false;
    }

    bool scene_changed = false;

    for (SceneID const& child_id : children.value()) {
        scene_changed |= configure_scene_gui(child_id, scene);
    }

    return scene_changed;
}

} // namespace gvs::scene
