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
#include "make_primitive.hpp"

// project
#include "magnum_conversions.hpp"

// external
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/Primitives/Cone.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Trade/MeshData3D.h>

namespace gvs::display {
namespace {

struct PrimitiveVisitor {
    SceneItemInfo* info;

    auto operator()(Axes const&) const -> void {
        auto axes_mesh = Magnum::Primitives::axis3D();

        auto const& colors4 = axes_mesh.colors(0);

        std::vector<Magnum::Color3> colors3;
        colors3.reserve(colors4.size());
        for (auto const& color4 : colors4) {
            colors3.emplace_back(color4.r(), color4.g(), color4.b());
        }

        info->geometry_info                = {};
        info->geometry_info.positions      = std::move(axes_mesh.positions(0));
        info->geometry_info.vertex_colors  = std::move(colors3);
        info->geometry_info.indices        = std::move(axes_mesh.indices());
        info->display_info.geometry_format = from_magnum(axes_mesh.primitive());
        info->display_info.coloring        = gvs::Coloring::VertexColors;
        info->display_info.shading         = gvs::Shading::UniformColor;
    }

    auto operator()(Cone const& cone) const -> void {
        auto cone_mesh = Magnum::Primitives::coneSolid(cone.rings, cone.segments, cone.half_length);

        info->geometry_info                = {};
        info->geometry_info.positions      = std::move(cone_mesh.positions(0));
        info->geometry_info.normals        = std::move(cone_mesh.normals(0));
        info->geometry_info.indices        = std::move(cone_mesh.indices());
        info->display_info.geometry_format = from_magnum(cone_mesh.primitive());
    }

    auto operator()(Cube const&) const -> void {
        auto cube_mesh = Magnum::Primitives::cubeSolid();

        info->geometry_info                = {};
        info->geometry_info.positions      = std::move(cube_mesh.positions(0));
        info->geometry_info.normals        = std::move(cube_mesh.normals(0));
        info->geometry_info.indices        = std::move(cube_mesh.indices());
        info->display_info.geometry_format = from_magnum(cube_mesh.primitive());
    }

    auto operator()(Cylinder const& cylinder) const -> void {
        auto cylinder_mesh = Magnum::Primitives::cylinderSolid(cylinder.rings, cylinder.segments, cylinder.half_length);

        info->geometry_info                = {};
        info->geometry_info.positions      = std::move(cylinder_mesh.positions(0));
        info->geometry_info.normals        = std::move(cylinder_mesh.normals(0));
        info->geometry_info.indices        = std::move(cylinder_mesh.indices());
        info->display_info.geometry_format = from_magnum(cylinder_mesh.primitive());
    }

    auto operator()(Plane const&) const -> void {
        auto plane_mesh = Magnum::Primitives::planeSolid();

        info->geometry_info                = {};
        info->geometry_info.positions      = std::move(plane_mesh.positions(0));
        info->geometry_info.normals        = std::move(plane_mesh.normals(0));
        info->display_info.geometry_format = from_magnum(plane_mesh.primitive());
    }

    auto operator()(Sphere const& sphere) const -> void {
        auto sphere_mesh = Magnum::Primitives::uvSphereSolid(sphere.rings, sphere.segments);

        info->geometry_info                = {};
        info->geometry_info.positions      = std::move(sphere_mesh.positions(0));
        info->geometry_info.normals        = std::move(sphere_mesh.normals(0));
        info->geometry_info.indices        = std::move(sphere_mesh.indices());
        info->display_info.geometry_format = from_magnum(sphere_mesh.primitive());
    }
};

} // namespace

auto make_primitive(SceneItemInfo* info, Primitive const& primitive) -> void {
    return mapbox::util::apply_visitor(PrimitiveVisitor{info}, primitive);
}

} // namespace gvs::display
