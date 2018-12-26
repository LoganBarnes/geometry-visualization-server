// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2018 Logan Barnes - All Rights Reserved
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

#include <gvs/types.pb.h>

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
auto indices(std::vector<float> data) {
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

inline auto display_mode(proto::DisplayMode data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_shader_display_mode()) {
            return "shader_display_mode";
        }
        info->mutable_display_info()->mutable_shader_display_mode()->set_value(data);
        return "";
    };
}

// TODO: create a better input type that handles pointers, vectors, matrices, iterators, etc.
inline auto transformation(std::array<float, 16> data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_transformation()) {
            return "transformation";
        }
        proto::Mat4* transformation = info->mutable_display_info()->mutable_transformation();
        *(transformation->mutable_data()) = {std::begin(data), std::end(data)};
        return "";
    };
}

// TODO: create a better input type that handles pointers, vectors, iterators, etc.
inline auto global_color(std::array<float, 3> data) {
    return [data](proto::SceneItemInfo* info) {
        if (info->mutable_display_info()->has_global_color()) {
            return "global_color";
        }
        proto::Vec3* global_color = info->mutable_display_info()->mutable_global_color();
        global_color->set_x(data[0]);
        global_color->set_y(data[1]);
        global_color->set_z(data[2]);
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

} // namespace gvs
