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

// standard
#include <iostream>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace gvs {

using SceneID = std::string;
const std::string nil_id;

// TODO: make vec and matrix types that can be implicitly converted from other common library types
using vec2 = std::array<float, 2>;
using vec3 = std::array<float, 3>;
using vec4 = std::array<float, 4>;
using mat2 = std::array<float, 4>;
using mat3 = std::array<float, 9>;
using mat4 = std::array<float, 16>;

template <std::size_t N>
struct AttributeVector {
public:
    template <typename V = std::array<float, N>>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    AttributeVector(std::initializer_list<V> data) : AttributeVector(std::vector<V>(std::move(data))) {}

    template <typename V>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    AttributeVector(std::vector<V> const& data) {
        std::copy(reinterpret_cast<float const*>(data.data()),
                  reinterpret_cast<float const*>(data.data() + data.size()),
                  std::back_inserter(data_));
    }

    template <typename V>
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    AttributeVector(std::vector<V>&& data) {
        std::move(reinterpret_cast<float*>(data.data()),
                  reinterpret_cast<float*>(data.data() + data.size()),
                  std::back_inserter(data_));
    }

    auto data() const { return data_.data(); }
    auto data() { return data_.data(); }
    auto size() const { return data_.size(); }
    auto size() { return data_.size(); }
    auto empty() const { return data_.empty(); }
    auto empty() { return data_.empty(); }

private:
    std::vector<float> data_;
};

enum class GeometryFormat {
    Points,
    Lines,
    LineStrip,
    Triangles,
    TriangleStrip,
    TriangleFan,
};

enum class Coloring : int32_t {
    Positions = 0,
    Normals,
    TextureCoordinates,
    VertexColors,
    UniformColor,
    Texture,
    White,
};

struct UniformColor {};

struct LambertianShading {
    vec3 light_direction = {-1.f, -1.f, -1.f};
    vec3 light_color     = {1.f, 1.f, 1.f};
    vec3 ambient_color   = {0.15f, 0.15f, 0.15f};
};

using Shading = std::variant<UniformColor, LambertianShading>;

constexpr auto default_readable_id     = "Scene Item";
constexpr auto default_geometry_format = GeometryFormat::Points;
// clang-format off
constexpr auto default_transformation = mat4{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
};
// clang-format on
constexpr auto default_uniform_color = vec3{1.f, 0.9f, 0.7f};
constexpr auto default_coloring      = Coloring::UniformColor;
constexpr auto default_shading       = Shading{UniformColor{}};
constexpr auto default_visible       = true;
constexpr auto default_opacity       = 1.f;

struct GeometryInfo {
    std::optional<AttributeVector<3>>    positions;
    std::optional<AttributeVector<3>>    normals;
    std::optional<AttributeVector<2>>    texture_coordinates;
    std::optional<AttributeVector<3>>    vertex_colors;
    std::optional<std::vector<unsigned>> indices;
};

struct DisplayInfo {
    std::optional<std::string>    readable_id;
    std::optional<GeometryFormat> geometry_format;
    std::optional<mat4>           transformation;
    std::optional<vec3>           uniform_color;
    std::optional<Coloring>       coloring;
    std::optional<Shading>        shading;
    std::optional<bool>           visible;
    std::optional<float>          opacity;
};

struct SceneItemInfo {
    std::optional<GeometryInfo>         geometry_info;
    std::optional<DisplayInfo>          display_info;
    std::optional<SceneID>              parent;
    std::optional<std::vector<SceneID>> children;
};

} // namespace gvs
