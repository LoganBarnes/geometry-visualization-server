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

#include "gvs/display/types.hpp"

// standard
#include <functional>
#include <type_traits>
#include <vector>

template <typename C>
struct has_data {
private:
    template <typename T>
    static constexpr auto check(const T* t) -> typename std::is_same<decltype(t->data()), float const*>::type;

    template <typename>
    static constexpr std::false_type check(...);

    typedef decltype(check<C>(nullptr)) type;

public:
    static constexpr bool value = type::value;
};

template <typename T, typename = typename std::enable_if<has_data<T>::value>::type>
auto data_ptr(const T& t) -> float const* {
    return t.data();
}

namespace gvs {

template <typename Vec3 = ::gvs::vec3>
inline auto positions_3d(std::vector<Vec3> const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->geometry_info && info->geometry_info->positions) {
            return "positions_3d";
        }
        static_assert(std::is_same<float const*, decltype(data_ptr(std::declval<Vec3>()))>::value,
                      "data must be convertible to a float const*");
        static_assert(sizeof(Vec3) == sizeof(float) * 3, "Vec3 type must be unpadded");

        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }
        info->geometry_info->positions = std::vector<vec3>(reinterpret_cast<const vec3*>(data.data()),
                                                           reinterpret_cast<const vec3*>(data.data() + data.size()));
        return "";
    };
}

template <typename Vec3 = ::gvs::vec3>
inline auto normals_3d(std::vector<Vec3> const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->geometry_info && info->geometry_info->normals) {
            return "normals_3d";
        }
        static_assert(std::is_same<float const*, decltype(data_ptr(std::declval<Vec3>()))>::value,
                      "data must be convertible to a float const*");
        static_assert(sizeof(Vec3) == sizeof(float) * 3, "Vec3 type must be unpadded");

        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }
        info->geometry_info->normals = std::vector<vec3>(reinterpret_cast<const vec3*>(data.data()),
                                                         reinterpret_cast<const vec3*>(data.data() + data.size()));
        return "";
    };
}

template <typename Vec2 = ::gvs::vec2>
inline auto tex_coords_3d(std::vector<Vec2> const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->geometry_info && info->geometry_info->tex_coords) {
            return "tex_coords_3d";
        }
        static_assert(std::is_same<float const*, decltype(data_ptr(std::declval<Vec2>()))>::value,
                      "data must be convertible to a float const*");
        static_assert(sizeof(Vec2) == sizeof(float) * 2, "Vec2 type must be unpadded");

        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }
        info->geometry_info->tex_coords = std::vector<vec2>(reinterpret_cast<const vec2*>(data.data()),
                                                            reinterpret_cast<const vec2*>(data.data() + data.size()));
        return "";
    };
}

template <typename Vec3 = ::gvs::vec3>
inline auto vertex_colors_3d(std::vector<Vec3> const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->geometry_info && info->geometry_info->vertex_colors) {
            return "vertex_colors_3d";
        }
        static_assert(std::is_same<float const*, decltype(data_ptr(std::declval<Vec3>()))>::value,
                      "data must be convertible to a float const*");
        static_assert(sizeof(Vec3) == sizeof(float) * 3, "Vec3 type must be unpadded");

        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }
        info->geometry_info->vertex_colors
            = std::vector<vec3>(reinterpret_cast<const vec3*>(data.data()),
                                reinterpret_cast<const vec3*>(data.data() + data.size()));
        return "";
    };
}

template <GeometryFormat Format = GeometryFormat::Points>
inline auto indices(std::vector<unsigned> const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->geometry_info && info->geometry_info->indices) {
            return "indices_3d";
        }

        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }
        info->geometry_info->indices = data;

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->geometry_format = Format;
        return "";
    };
}

constexpr auto points = indices<GeometryFormat::Points>;
constexpr auto lines = indices<GeometryFormat::Lines>;
constexpr auto line_strip = indices<GeometryFormat::LineStrip>;
constexpr auto triangles = indices<GeometryFormat::Triangles>;
constexpr auto triangle_strip = indices<GeometryFormat::TriangleStrip>;
constexpr auto triangle_fan = indices<GeometryFormat::TriangleFan>;

inline auto geometry_format(GeometryFormat const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->geometry_format) {
            return "geometry_format";
        }

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->geometry_format = data;
        return "";
    };
}

inline auto readable_id(std::string const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->readable_id) {
            return "readable_id";
        }

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->readable_id = data;
        return "";
    };
}

template <typename Mat4 = ::gvs::mat4>
auto transformation(Mat4 const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->transformation) {
            return "transformation";
        }
        static_assert(sizeof(Mat4) == sizeof(float) * 16, "Mat4 type must be unpadded");

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->transformation = reinterpret_cast<mat4 const&>(data);
        return "";
    };
}

template <typename Vec3 = ::gvs::vec3>
auto uniform_color(Vec3 const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->uniform_color) {
            return "uniform_color";
        }
        static_assert(sizeof(Vec3) == sizeof(float) * 3, "Vec3 type must be unpadded");

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->uniform_color = reinterpret_cast<vec3 const&>(data);
        return "";
    };
}

inline auto coloring(Coloring const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->coloring) {
            return "coloring";
        }

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->coloring = data;
        return "";
    };
}

inline auto shading(Shading const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->shading) {
            return "shading";
        }

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->shading = data;
        return "";
    };
}

inline auto visible(bool const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->visible) {
            return "visible";
        }

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->visible = data;
        return "";
    };
}

inline auto opacity(float const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->display_info && info->display_info->opacity) {
            return "opacity";
        }

        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }
        info->display_info->opacity = data;
        return "";
    };
}

inline auto parent(SceneID const& data) -> std::function<std::string(SceneItemInfo*)> {
    return [data](SceneItemInfo* info) {
        if (info->parent) {
            return "parent";
        }

        info->parent = data;
        return "";
    };
}

} // namespace gvs
