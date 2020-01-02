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

// project
#include "make_primitive.hpp"

namespace gvs {

auto replace_if_present(SceneItemInfo* info, SparseSceneItemInfo&& new_info) -> util::Result<void> {

    if (new_info.parent) {
        info->parent = *new_info.parent;
    }

    if (new_info.geometry) {
        auto& geometry_info = info->geometry_info;

        if (new_info.geometry->is<Primitive>()) {
            display::make_primitive(info, new_info.geometry->get<Primitive>());

        } else {
            assert(new_info.geometry->is<SparseGeometryInfo>());

            auto& new_geometry_info = new_info.geometry->get<SparseGeometryInfo>();

            if (new_geometry_info.positions) {
                geometry_info.positions = std::move(*new_geometry_info.positions);
            }

            auto positions_size = geometry_info.positions.size();

            auto maybe_replace
                = [positions_size](auto& data, auto&& new_data, std::string const& name) -> util::Result<void> {
                if (new_data) {
                    data = std::move(*(new_data));
                }

                if (!data.empty() && data.size() != positions_size) {
                    return tl::make_unexpected(
                        MAKE_ERROR(name + " is non-empty and positions.size() != " + name + ".size()"));
                }
                return util::success();
            };

            auto result = maybe_replace(geometry_info.normals, std::move(new_geometry_info.normals), "normals")
                              .and_then(maybe_replace,
                                        geometry_info.texture_coordinates,
                                        std::move(new_geometry_info.texture_coordinates),
                                        "texture_coordinates")
                              .and_then(maybe_replace,
                                        geometry_info.vertex_colors,
                                        std::move(new_geometry_info.vertex_colors),
                                        "vertex_colors");

            if (!result) {
                return result;
            }

            // indices are handled separately because the size doesn't matter so no validation needs to be performed
            if (new_geometry_info.indices) {
                geometry_info.indices = std::move(*new_geometry_info.indices);
            }
        }
    }

    if (new_info.display_info) {
        auto& display_info     = info->display_info;
        auto& new_display_info = *new_info.display_info;

        auto maybe_replace = [](auto& data, auto&& new_data) {
            if (new_data) {
                data = std::move(*(new_data));
            }
        };

        maybe_replace(display_info.readable_id, std::move(new_display_info.readable_id));
        maybe_replace(display_info.geometry_format, std::move(new_display_info.geometry_format));
        maybe_replace(display_info.transformation, std::move(new_display_info.transformation));
        maybe_replace(display_info.uniform_color, std::move(new_display_info.uniform_color));
        maybe_replace(display_info.coloring, std::move(new_display_info.coloring));
        maybe_replace(display_info.shading, std::move(new_display_info.shading));
        maybe_replace(display_info.visible, std::move(new_display_info.visible));
        maybe_replace(display_info.opacity, std::move(new_display_info.opacity));
        maybe_replace(display_info.wireframe_only, std::move(new_display_info.wireframe_only));
    }

    return util::success();
}

} // namespace gvs
