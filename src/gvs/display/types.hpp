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

// external
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>

// standard
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

namespace gvs {

using vec2 = std::array<float, 2>;
using vec3 = std::array<float, 3>;
using vec4 = std::array<float, 4>;
using mat2 = std::array<vec2, 2>;
using mat3 = std::array<vec3, 3>;
using mat4 = std::array<vec4, 4>;

enum class GeometryFormat {
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan,
};

enum class Coloring {
    Positions,
    Normals,
    TextureCoordinates,
    VertexColors,
    UniformColor,
    Texture,
    White,
};

struct UniformColor {};

struct LambertianShading {
    vec3 light_direction;
    vec3 light_color;
    vec3 ambient_color;
};

using Shading = std::variant<UniformColor, LambertianShading>;

struct GeometryInfo {
    std::unique_ptr<std::vector<vec3>> positions;
    std::unique_ptr<std::vector<vec3>> normals;
    std::unique_ptr<std::vector<vec2>> tex_coords;
    std::unique_ptr<std::vector<vec3>> vertex_colors;
    std::unique_ptr<std::vector<unsigned>> indices;
};

struct DisplayInfo {
    std::unique_ptr<std::string> readable_id;
    std::unique_ptr<GeometryFormat> geometry_format;
    std::unique_ptr<mat4> transformation;
    std::unique_ptr<vec3> uniform_color;
    std::unique_ptr<Coloring> coloring;
    std::unique_ptr<Shading> shading;
    std::unique_ptr<bool> visible;
    std::unique_ptr<float> opacity;
};

struct SceneItemInfo {
    std::unique_ptr<std::string> id;
    std::unique_ptr<GeometryInfo> geometry_info;
    std::unique_ptr<DisplayInfo> display_info;
    std::unique_ptr<std::string> parent;
    std::unique_ptr<std::vector<std::string>> children;
};

using SceneItems = std::unordered_map<std::string, SceneItemInfo>;

} // namespace gvs
