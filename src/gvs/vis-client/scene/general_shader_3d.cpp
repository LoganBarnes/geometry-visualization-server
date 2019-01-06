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
#include "general_shader_3d.hpp"

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

namespace gvs {
namespace vis {

GeneralShader3D::GeneralShader3D() {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(Magnum::GL::Version::GL450);

    const Corrade::Utility::Resource rs{"gvs-resource-data"};

    Magnum::GL::Shader vert{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Vertex};
    Magnum::GL::Shader frag{Magnum::GL::Version::GL450, Magnum::GL::Shader::Type::Fragment};

    vert.addSource(rs.get("general_shader.vert"));
    frag.addSource(rs.get("general_shader.frag"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(Magnum::GL::Shader::compile({vert, frag}));

    attachShaders({vert, frag});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());

    view_from_local_uniform_ = uniformLocation("view_from_local");
    view_from_local_normals_uniform_ = uniformLocation("view_from_local_normals");
    projection_from_view_uniform_ = uniformLocation("projection_from_view");

    coloring_uniform_ = uniformLocation("coloring");
    uniform_color_uniform_ = uniformLocation("uniform_color");
}

GeneralShader3D& GeneralShader3D::set_transformation_matrix(const Magnum::Matrix4& view_from_local) {
    setUniform(view_from_local_uniform_, view_from_local);
    return *this;
}

GeneralShader3D& GeneralShader3D::set_normal_matrix(const Magnum::Matrix3& view_from_local_normals) {
    setUniform(view_from_local_normals_uniform_, view_from_local_normals);
    return *this;
}

GeneralShader3D& GeneralShader3D::set_projection_matrix(const Magnum::Matrix4& projection_from_view) {
    setUniform(projection_from_view_uniform_, projection_from_view);
    return *this;
}

GeneralShader3D& GeneralShader3D::set_coloring(const proto::Coloring& coloring) {
    setUniform(coloring_uniform_, coloring);
    return *this;
}

GeneralShader3D& GeneralShader3D::set_uniform_color(const Magnum::Color3& color) {
    setUniform(uniform_color_uniform_, color);
    return *this;
}

} // namespace vis
} // namespace gvs
