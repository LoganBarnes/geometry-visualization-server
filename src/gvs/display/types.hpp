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
#pragma once

// project
#include "backends/to_magnum.hpp"

// external
#include <Magnum/Magnum.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>

namespace gvs {

//struct GeometryInfo {
//    std::optional<std::vector<Magnum::Vector3>> positions = {};
//    std::optional<std::vector<Magnum::Vector3>> normals = {};
//    std::optional<std::vector<Magnum::Vector2>> tex_coords = {};
//    std::optional<std::vector<Magnum::Vector3>> vertex_colors = {};
//    std::optional<std::vector<unsigned>> indices = {};
//};
//
//struct DisplayInfo {
//    std::string readable_id = default_readable_id;
//    GeometryFormat geometry_format = default_geometry_format;
//    Magnum::Matrix4 transformation = display::backends::to_magnum(default_transformation);
//    Magnum::Vector3 uniform_color = display::backends::to_magnum(default_uniform_color);
//    Coloring coloring = default_coloring;
//    Shading shading = default_shading;
//    bool visible = default_visible;
//    float opacity = default_opacity;
//};
//
//struct SceneItemInfo {
//    GeometryInfo geometry_info = {};
//    DisplayInfo display_info = {};
//    SceneID parent = nil_id;
//    std::vector<SceneID> children = {};
//};

using SceneItems = std::unordered_map<SceneID, SceneItemInfo>;

} // namespace gvs
