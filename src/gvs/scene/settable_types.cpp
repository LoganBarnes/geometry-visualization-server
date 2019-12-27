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
#include "settable_types.hpp"

namespace gvs {

auto set_defaults_on_empty_fields(SceneItemInfo* info) -> void {
    if (!info->parent) {
        info->parent = nil_id;
    }

    if (!info->children) {
        info->children = std::vector<SceneID>{};
    }

    if (!info->display_info) {
        info->display_info = DisplayInfo{};
    }

    auto& display_info = info->display_info.value();

    if (!display_info.readable_id) {
        display_info.readable_id = default_readable_id;
    }
    if (!display_info.geometry_format) {
        display_info.geometry_format = default_geometry_format;
    }
    if (!display_info.transformation) {
        display_info.transformation = default_transformation;
    }
    if (!display_info.uniform_color) {
        display_info.uniform_color = default_uniform_color;
    }
    if (!display_info.coloring) {
        display_info.coloring = default_coloring;
    }
    if (!display_info.shading) {
        display_info.shading = default_shading;
    }
    if (!display_info.visible) {
        display_info.visible = default_visible;
    }
    if (!display_info.opacity) {
        display_info.opacity = default_opacity;
    }
}

} // namespace gvs
