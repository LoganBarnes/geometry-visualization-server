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
#include "general_shader_3d.hpp"

// project
#include "gvs/util/variant_utils.hpp"

// external
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

namespace gvs::display::backends {

GeneralShader3d::GeneralShader3d() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL450);

    const Corrade::Utility::Resource rs{"gvs-resource-data"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader frag{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.get("general_shader.vert"));
    frag.addSource(rs.get("general_shader.frag"));

    auto vert_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(vert);
    auto frag_ref = Corrade::Containers::Reference<Magnum::GL::Shader>(frag);

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert_ref, frag_ref}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    world_from_local_uniform_location_         = uniformLocation("world_from_local");
    world_from_local_normals_uniform_location_ = uniformLocation("world_from_local_normals");
    projection_from_local_uniform_location_    = uniformLocation("projection_from_local");

    coloring_uniform_location_      = uniformLocation("coloring");
    uniform_color_uniform_location_ = uniformLocation("uniform_color");

    shading_uniform_location_         = uniformLocation("shading");
    light_direction_uniform_location_ = uniformLocation("light_direction");
    light_color_uniform_location_     = uniformLocation("light_color");
    ambient_scale_uniform_location_   = uniformLocation("ambient_scale");

    id_uniform_location_ = uniformLocation("id");
}

auto GeneralShader3d::set_world_from_local_matrix(Magnum::Matrix4 const& world_from_local) -> GeneralShader3d& {
    setUniform(world_from_local_uniform_location_, world_from_local);
    return *this;
}

auto GeneralShader3d::set_world_from_local_normal_matrix(Magnum::Matrix3 const& world_from_local_normals)
    -> GeneralShader3d& {
    setUniform(world_from_local_normals_uniform_location_, world_from_local_normals);
    return *this;
}

auto GeneralShader3d::set_projection_from_local_matrix(Magnum::Matrix4 const& projection_from_local)
    -> GeneralShader3d& {
    setUniform(projection_from_local_uniform_location_, projection_from_local);
    return *this;
}

auto GeneralShader3d::set_coloring(Coloring const& coloring) -> GeneralShader3d& {
    setUniform(coloring_uniform_location_, std::underlying_type_t<Coloring>(coloring));
    return *this;
}

auto GeneralShader3d::set_uniform_color(Magnum::Color3 const& color) -> GeneralShader3d& {
    setUniform(uniform_color_uniform_location_, color);
    return *this;
}

auto GeneralShader3d::set_shading(Shading const& shading) -> GeneralShader3d& {
    setUniform(shading_uniform_location_, std::underlying_type_t<Shading>(shading));
    return *this;
}

auto GeneralShader3d::set_id(unsigned const& id) -> GeneralShader3d& {
    setUniform(id_uniform_location_, id);
    return *this;
}

} // namespace gvs::display::backends
