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

enum class ParamType : uint32_t {
    positions_3d,
    normals_3d,
    tex_coords_3d,
    vertex_colors,
    indices,
};

template <ParamType PT, typename Func>
struct Param {
    Func move_func;
};

//template <ParamType PT, typename Data, void (*data_func_)(const Data& data, gvs::proto::SceneItemInfo*)>
//struct Param {
//    Data data_;
//
//    void move_func(gvs::proto::SceneItemInfo* info) { data_func_(data_, info); }
//};

template <ParamType PT, typename Func>
Param<PT, Func> make_param(Func&& func) {
    return Param<PT, Func>{std::forward<Func>(func)};
}

namespace detail {

inline void positions_3d(const std::vector<float>& data, gvs::proto::SceneItemInfo* info) {
    *(info->mutable_geometry_info()->mutable_positions()) = {data.begin(), data.end()};
}

} // namespace detail

inline auto positions_3d(std::vector<float> data) {
    return [data{std::move(data)}](gvs::proto::SceneItemInfo* info, ParamType* type) {
        *type = ParamType::positions_3d;
        *(info->mutable_geometry_info()->mutable_positions()) = {data.begin(), data.end()};
    };
}

//using Positions3D = Param<ParamType::positions_3d, std::vector<float>, &detail::positions_3d>;
//using Normals3D
//    = Param<ParamType::positions_3d, std::vector<float>, [](const std::vector<float>&, gvs::proto::SceneItemInfo*) {}>;

} // namespace gvs
