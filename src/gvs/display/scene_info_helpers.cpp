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
#include "scene_info_helpers.hpp"

namespace gvs {

auto set_defaults_on_empty_fields(SceneItemInfo* info) -> void {
    if (!info->parent) {
        info->parent = std::make_unique<SceneID>(nil_id());
    }

    if (!info->children) {
        info->children = std::make_unique<std::vector<SceneID>>();
    }

    if (!info->display_info) {
        info->display_info = std::make_unique<DisplayInfo>();
    }

    auto& display_info = *info->display_info;

    if (!display_info.readable_id) {
        display_info.readable_id = std::make_unique<std::string>(default_readable_id);
    }
    if (!display_info.geometry_format) {
        display_info.geometry_format = std::make_unique<GeometryFormat>(default_geometry_format);
    }
    if (!display_info.transformation) {
        display_info.transformation = std::make_unique<mat4>(default_transformation);
    }
    if (!display_info.uniform_color) {
        display_info.uniform_color = std::make_unique<vec3>(default_uniform_color);
    }
    if (!display_info.coloring) {
        display_info.coloring = std::make_unique<Coloring>(default_coloring);
    }
    if (!display_info.shading) {
        display_info.shading = std::make_unique<Shading>(default_shading);
    }
    if (!display_info.visible) {
        display_info.visible = std::make_unique<bool>(default_visible);
    }
    if (!display_info.opacity) {
        display_info.opacity = std::make_unique<float>(default_opacity);
    }
    if (!display_info.wireframe_only) {
        display_info.wireframe_only = std::make_unique<bool>(default_wireframe_only);
    }
}

auto replace_if_present(SceneItemInfo* info, SceneItemInfo&& new_info) -> util::Result<void> {

    if (new_info.geometry_info) {
        auto& geometry_info     = *info->geometry_info;
        auto& new_geometry_info = *new_info.geometry_info;

        if (new_geometry_info.positions) {
            geometry_info.positions = std::move(new_geometry_info.positions);
        }

        auto positions_size = geometry_info.positions->size();

        auto maybe_replace = [&](auto member, std::string const& name) -> util::Result<void> {
            if (new_geometry_info.*member) {
                geometry_info.*member = std::move(new_geometry_info.*member);
            }

            bool non_empty = (geometry_info.*member && !(geometry_info.*member)->empty());
            if (non_empty && (geometry_info.*member)->size() != positions_size) {
                return tl::make_unexpected(
                    MAKE_ERROR(name + " is non-empty and positions.size() != " + name + ".size()"));
            }
            return util::success();
        };

        auto result = maybe_replace(&GeometryInfo::positions, "positions")
                          .and_then(maybe_replace, &GeometryInfo::normals, "normals")
                          .and_then(maybe_replace, &GeometryInfo::texture_coordinates, "texture_coordinates")
                          .and_then(maybe_replace, &GeometryInfo::vertex_colors, "vertex_colors")
                          .and_then(maybe_replace, &GeometryInfo::indices, "indices");

        if (!result) {
            return result;
        }
    }

    if (new_info.display_info) {
        auto& display_info     = *info->display_info;
        auto& new_display_info = *new_info.display_info;

        auto maybe_replace = [&](auto member) {
            if (new_display_info.*member) {
                display_info.*member = std::move(new_display_info.*member);
            }
        };

        maybe_replace(&DisplayInfo::readable_id);
        maybe_replace(&DisplayInfo::geometry_format);
        maybe_replace(&DisplayInfo::transformation);
        maybe_replace(&DisplayInfo::uniform_color);
        maybe_replace(&DisplayInfo::coloring);
        maybe_replace(&DisplayInfo::shading);
        maybe_replace(&DisplayInfo::visible);
        maybe_replace(&DisplayInfo::opacity);
        maybe_replace(&DisplayInfo::wireframe_only);
    }

    return util::success();
}

} // namespace gvs
