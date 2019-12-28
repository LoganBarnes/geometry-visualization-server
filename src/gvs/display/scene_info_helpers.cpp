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

auto replace_if_present(SceneItemInfo* info, SceneItemInfoSetter&& new_info) -> util::Result<void> {

    if (new_info.geometry_info) {
        auto& geometry_info     = info->geometry_info;
        auto& new_geometry_info = *new_info.geometry_info;

        if (new_geometry_info.positions) {
            geometry_info.positions = std::move(*new_geometry_info.positions);
        }

        auto positions_size = geometry_info.positions.size();

        auto maybe_replace = [&](auto member, auto new_member, std::string const& name) -> util::Result<void> {
            if (new_geometry_info.*new_member) {
                geometry_info.*member = std::move(*(new_geometry_info.*new_member));
            }

            if (!(geometry_info.*member).empty() && (geometry_info.*member).size() != positions_size) {
                return tl::make_unexpected(
                    MAKE_ERROR(name + " is non-empty and positions.size() != " + name + ".size()"));
            }
            return util::success();
        };

        auto result = maybe_replace(&GeometryInfo::positions, &GeometryInfoSetter::positions, "positions")
                          .and_then(maybe_replace, &GeometryInfo::normals, &GeometryInfoSetter::normals, "normals")
                          .and_then(maybe_replace,
                                    &GeometryInfo::texture_coordinates,
                                    &GeometryInfoSetter::texture_coordinates,
                                    "texture_coordinates")
                          .and_then(maybe_replace,
                                    &GeometryInfo::vertex_colors,
                                    &GeometryInfoSetter::vertex_colors,
                                    "vertex_colors");

        if (!result) {
            return result;
        }

        // indices are handled separately because the size doesn't matter so no validation needs to be performed
        if (new_geometry_info.indices) {
            geometry_info.indices = std::move(*(new_geometry_info.indices));
        }
    }

    if (new_info.display_info) {
        auto& display_info     = info->display_info;
        auto& new_display_info = *new_info.display_info;

        auto maybe_replace = [&](auto member, auto new_member) {
            if (new_display_info.*new_member) {
                display_info.*member = std::move(*(new_display_info.*new_member));
            }
        };

        maybe_replace(&DisplayInfo::readable_id, &DisplayInfoSetter::readable_id);
        maybe_replace(&DisplayInfo::geometry_format, &DisplayInfoSetter::geometry_format);
        maybe_replace(&DisplayInfo::transformation, &DisplayInfoSetter::transformation);
        maybe_replace(&DisplayInfo::uniform_color, &DisplayInfoSetter::uniform_color);
        maybe_replace(&DisplayInfo::coloring, &DisplayInfoSetter::coloring);
        maybe_replace(&DisplayInfo::shading, &DisplayInfoSetter::shading);
        maybe_replace(&DisplayInfo::visible, &DisplayInfoSetter::visible);
        maybe_replace(&DisplayInfo::opacity, &DisplayInfoSetter::opacity);
        maybe_replace(&DisplayInfo::wireframe_only, &DisplayInfoSetter::wireframe_only);
    }

    return util::success();
}

} // namespace gvs
