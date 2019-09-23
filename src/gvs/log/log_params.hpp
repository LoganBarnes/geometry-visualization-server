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

#include "gvs/log/float_types.hpp"

#include <types.pb.h>

#include <cstdint>
#include <vector>

namespace gvs {

inline auto positions_3d(std::vector<float> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_positions()) {
            return "positions_3d";
        }
        *(info->mutable_geometry_info()->mutable_positions()->mutable_value()) = {data.begin(), data.end()};
        return "";
    };
}

inline auto normals_3d(std::vector<float> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_normals()) {
            return "normals_3d";
        }
        *(info->mutable_geometry_info()->mutable_normals()->mutable_value()) = {data.begin(), data.end()};
        return "";
    };
}

inline auto tex_coords_3d(std::vector<float> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_tex_coords()) {
            return "tex_coords_3d";
        }
        *(info->mutable_geometry_info()->mutable_tex_coords()->mutable_value()) = {data.begin(), data.end()};
        return "";
    };
}

inline auto vertex_colors_3d(std::vector<float> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_vertex_colors()) {
            return "vertex_colors_3d";
        }
        *(info->mutable_geometry_info()->mutable_vertex_colors()->mutable_value()) = {data.begin(), data.end()};
        return "";
    };
}

template <proto::GeometryFormat format>
auto indices(std::vector<unsigned> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_indices()) {
            return "indices";
        }
        *(info->mutable_geometry_info()->mutable_indices()->mutable_value()) = {data.begin(), data.end()};
        info->mutable_display_info()->mutable_geometry_format()->set_value(format);
        return "";
    };
}

constexpr auto points = indices<proto::GeometryFormat::POINTS>;
constexpr auto lines = indices<proto::GeometryFormat::LINES>;
constexpr auto line_strip = indices<proto::GeometryFormat::LINE_STRIP>;
constexpr auto triangles = indices<proto::GeometryFormat::TRIANGLES>;
constexpr auto triangle_strip = indices<proto::GeometryFormat::TRIANGLE_STRIP>;
constexpr auto triangle_fan = indices<proto::GeometryFormat::TRIANGLE_FAN>;

inline auto geometry_format(proto::GeometryFormat data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_geometry_format()) {
            return "geometry_format";
        }
        info->mutable_display_info()->mutable_geometry_format()->set_value(data);
        return "";
    };
}

inline auto coloring(proto::Coloring data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_coloring()) {
            return "coloring";
        }
        info->mutable_display_info()->mutable_coloring()->set_value(data);
        return "";
    };
}

/// \TODO: create a better input type that handles pointers, vectors, matrices, iterators, etc.
inline auto transformation(std::array<float, 16> data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_transformation()) {
            return "transformation";
        }
        proto::Mat4* transformation = info->mutable_display_info()->mutable_transformation();
        *(transformation->mutable_data())
            = {reinterpret_cast<const float*>(data.data()), reinterpret_cast<const float*>(data.data() + data.size())};
        return "";
    };
}

/// \TODO: create a better input type that handles pointers, vectors, iterators, etc.
inline auto uniform_color(std::array<float, 3> data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_uniform_color()) {
            return "uniform_color";
        }
        proto::Vec3* uniform_color = info->mutable_display_info()->mutable_uniform_color();
        uniform_color->set_x(data[0]);
        uniform_color->set_y(data[1]);
        uniform_color->set_z(data[2]);
        return "";
    };
}

inline auto parent(std::string data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->has_parent()) {
            return "parent";
        }
        info->mutable_parent()->set_value(data);
        return "";
    };
}

struct UniformColorShading {
    explicit UniformColorShading() = default;
};

struct LambertianShading {
    std::array<float, 3> light_direction;
    std::array<float, 3> light_color;
    std::array<float, 3> ambient_color;

    explicit LambertianShading(std::array<float, 3> light_dir = {-1.f, -1.f, -1.f},
                               std::array<float, 3> light_colour = {1.f, 1.f, 1.f},
                               std::array<float, 3> ambient_colour = {0.15f, 0.15f, 0.15f})
        : light_direction(light_dir), light_color(light_colour), ambient_color(ambient_colour) {}
};

inline auto shading(const UniformColorShading&) {
    return [](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_shading()) {
            return "shading";
        }
        info->mutable_display_info()->mutable_shading()->uniform_color();
        return "";
    };
}

inline auto shading(LambertianShading data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_shading()) {
            return "shading";
        }
        proto::LambertianShading* shading = info->mutable_display_info()->mutable_shading()->mutable_lambertian();
        shading->mutable_light_direction()->set_x(data.light_direction[0]);
        shading->mutable_light_direction()->set_y(data.light_direction[1]);
        shading->mutable_light_direction()->set_z(data.light_direction[2]);

        shading->mutable_light_color()->set_x(data.light_color[0]);
        shading->mutable_light_color()->set_y(data.light_color[1]);
        shading->mutable_light_color()->set_z(data.light_color[2]);

        shading->mutable_ambient_color()->set_x(data.ambient_color[0]);
        shading->mutable_ambient_color()->set_y(data.ambient_color[1]);
        shading->mutable_ambient_color()->set_z(data.ambient_color[2]);
        return "";
    };
}

#if __cplusplus >= 201703L

template <template <typename, auto...> class Vec3,
          auto... Ts,
          typename = std::enable_if_t<detail::is_vec3<Vec3<float, Ts...>>()>>
inline auto positions_3d(std::vector<Vec3<float, Ts...>> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_positions()) {
            return "positions_3d";
        }
        *(info->mutable_geometry_info()->mutable_positions()->mutable_value())
            = {reinterpret_cast<const float*>(data.data()), reinterpret_cast<const float*>(data.data() + data.size())};
        return "";
    };
}

template <template <typename, auto...> class Vec3,
          auto... Ts,
          typename = std::enable_if_t<detail::is_vec3<Vec3<float, Ts...>>()>>
inline auto normals_3d(std::vector<Vec3<float, Ts...>> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_normals()) {
            return "normals_3d";
        }
        *(info->mutable_geometry_info()->mutable_normals()->mutable_value())
            = {reinterpret_cast<const float*>(data.data()), reinterpret_cast<const float*>(data.data() + data.size())};
        return "";
    };
}

template <template <typename, auto...> class Vec2,
          auto... Ts,
          typename = std::enable_if_t<detail::is_vec2<Vec2<float, Ts...>>()>>
inline auto tex_coords_3d(std::vector<Vec2<float, Ts...>> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_tex_coords()) {
            return "tex_coords_3d";
        }
        *(info->mutable_geometry_info()->mutable_tex_coords()->mutable_value())
            = {reinterpret_cast<const float*>(data.data()), reinterpret_cast<const float*>(data.data() + data.size())};
        return "";
    };
}

template <template <typename, auto...> class Vec3,
          auto... Ts,
          typename = std::enable_if_t<detail::is_vec3<Vec3<float, Ts...>>()>>
inline auto vertex_colors_3d(std::vector<Vec3<float, Ts...>> data) {
    return [data{std::move(data)}](proto::SceneItemInfo* info) {
        if (info->mutable_geometry_info()->has_vertex_colors()) {
            return "vertex_colors_3d";
        }
        *(info->mutable_geometry_info()->mutable_vertex_colors()->mutable_value())
            = {reinterpret_cast<const float*>(data.data()), reinterpret_cast<const float*>(data.data() + data.size())};
        return "";
    };
}

/// \TODO: create a better input type that handles pointers, vectors, matrices, iterators, etc.
template <template <typename, auto...> class Mat4,
          auto... Ts,
          typename = std::enable_if_t<detail::is_mat4<Mat4<float, Ts...>>()>>
auto transformation(Mat4<float, Ts...> data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_transformation()) {
            return "transformation";
        }
        proto::Mat4* transformation = info->mutable_display_info()->mutable_transformation();
        *(transformation->mutable_data())
            = {reinterpret_cast<const float*>(data[0]), reinterpret_cast<const float*>(data[0] + 16)};
        return "";
    };
}

/// \TODO: create a better input type that handles pointers, vectors, iterators, etc.
template <template <typename, auto...> class Vec3,
          auto... Ts,
          typename = std::enable_if_t<detail::is_vec3<Vec3<float, Ts...>>()>>
auto uniform_color(Vec3<float, Ts...> data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_uniform_color()) {
            return "uniform_color";
        }
        proto::Vec3* uniform_color = info->mutable_display_info()->mutable_uniform_color();
        uniform_color->set_x(data[0]);
        uniform_color->set_y(data[1]);
        uniform_color->set_z(data[2]);
        return "";
    };
}

#endif

} // namespace gvs
