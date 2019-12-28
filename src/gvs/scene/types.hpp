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
#include "gvs/util/make_unique_11.hpp"
#include "primitive_types.hpp"
#include "scene_id.hpp"

// external
#include <mapbox/variant.hpp> // C++11 variant

// standard
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

namespace gvs {

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

    auto data() const -> float const* { return data_.data(); }
    auto data() -> float* { return data_.data(); }
    auto size() const -> std::vector<float>::size_type { return data_.size(); }
    auto size() -> std::vector<float>::size_type { return data_.size(); }
    auto empty() const -> bool { return data_.empty(); }
    auto empty() -> bool { return data_.empty(); }

private:
    std::vector<float> data_;
};

enum class GeometryFormat : int32_t {
    Points = 0,
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

enum class Shading : int32_t {
    UniformColor = 0,
    Lambertian,
    CookTorrance,
};

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
constexpr auto default_uniform_color  = vec3{1.f, 0.9f, 0.7f};
constexpr auto default_coloring       = Coloring::UniformColor;
constexpr auto default_shading        = Shading::CookTorrance;
constexpr auto default_visible        = true;
constexpr auto default_opacity        = 1.f;
constexpr auto default_wireframe_only = false;

struct DisplayInfo {
    std::unique_ptr<std::string>    readable_id;
    std::unique_ptr<GeometryFormat> geometry_format;
    std::unique_ptr<mat4>           transformation;
    std::unique_ptr<vec3>           uniform_color;
    std::unique_ptr<Coloring>       coloring;
    std::unique_ptr<Shading>        shading;
    std::unique_ptr<bool>           visible;
    std::unique_ptr<float>          opacity;
    std::unique_ptr<bool>           wireframe_only;
};

struct GeometryInfo {
    std::unique_ptr<AttributeVector<3>>    positions;
    std::unique_ptr<AttributeVector<3>>    normals;
    std::unique_ptr<AttributeVector<2>>    texture_coordinates;
    std::unique_ptr<AttributeVector<3>>    vertex_colors;
    std::unique_ptr<std::vector<unsigned>> indices;
};

//namespace detail {
//using GeometryBase = mapbox::util::variant<GeometryInfo, Primitive>;
//}
//
//struct Geometry : detail::GeometryBase {
//    using detail::GeometryBase::variant;
//};

struct SceneItemInfo {
    std::unique_ptr<GeometryInfo>         geometry_info;
    std::unique_ptr<DisplayInfo>          display_info;
    std::unique_ptr<SceneID>              parent;
    std::unique_ptr<std::vector<SceneID>> children;
};

using SceneItems = std::unordered_map<SceneID, SceneItemInfo>;

} // namespace gvs
