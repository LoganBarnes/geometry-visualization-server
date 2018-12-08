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

//enum class ParamType : uint32_t {
//    positions_3d,
//    normals_3d,
//    tex_coords_3d,
//    vertex_colors,
//    indices,
//};
//
//template <ParamType PT, typename Data, proto::GeometryFormat format = proto::GeometryFormat::DEFAULT>
//class Param {
//public:
//    explicit Param(Data data) : data_(std::move(data)) {}
//    std::string move_func(proto::SceneItemInfo* info);
//
//private:
//    Data data_;
//};

//using Positions3D = Param<ParamType::positions_3d, std::vector<float>>;
//template <>
//void Positions3D::move_func(proto::SceneItemInfo* info) {
//    *(info->mutable_geometry_info()->mutable_positions()) = {data_.begin(), data_.end()};
//}
//
//using Points3D = Param<ParamType::positions_3d, std::vector<float>, proto::GeometryFormat::POINTS>;
//template <>
//void Points3D::move_func(proto::SceneItemInfo* info) {
//    info->mutable_display_info()->mutable_geometry_format()->set_value(proto::GeometryFormat::POINTS);
//    *(info->mutable_geometry_info()->mutable_positions()) = {data_.begin(), data_.end()};
//}

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

} // namespace gvs
