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

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Math/Matrix4.h>

namespace gvs::vis {

GeneralShader3D::GeneralShader3D() {
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

    world_from_local_uniform_ = uniformLocation("world_from_local");
    world_from_local_normals_uniform_ = uniformLocation("world_from_local_normals");
    projection_from_local_uniform_ = uniformLocation("projection_from_local");

    coloring_uniform_ = uniformLocation("coloring");
    uniform_color_uniform_ = uniformLocation("uniform_color");

    shading_uniform_ = uniformLocation("shading");
    light_direction_uniform_ = uniformLocation("light_direction");
    light_color_uniform_ = uniformLocation("light_color");
    ambient_color_uniform_ = uniformLocation("ambient_color");
}

GeneralShader3D& GeneralShader3D::set_world_from_local_matrix(const Magnum::Matrix4& world_from_local) {
    setUniform(world_from_local_uniform_, world_from_local);
    return *this;
}

GeneralShader3D& GeneralShader3D::set_world_from_local_normal_matrix(const Magnum::Matrix3& world_from_local_normals) {
    setUniform(world_from_local_normals_uniform_, world_from_local_normals);
    return *this;
}

GeneralShader3D& GeneralShader3D::set_projection_from_local_matrix(const Magnum::Matrix4& projection_from_local) {
    setUniform(projection_from_local_uniform_, projection_from_local);
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

GeneralShader3D& GeneralShader3D::set_shading(const proto::Shading& shading) {
    switch (shading.value_case()) {
    case proto::Shading::VALUE_NOT_SET:
    case proto::Shading::kUniformColor:
        setUniform(shading_uniform_, 0);
        break;

    case proto::Shading::kLambertian:
        setUniform(shading_uniform_, 1);

        const proto::LambertianShading& lambertian = shading.lambertian();

        Magnum::Vector3 light_direction{-1.f, -1.f, -1.f};
        Magnum::Vector3 light_color{1.f, 1.f, 1.f};
        Magnum::Vector3 ambient_color{0.15f, 0.15f, 0.15f};

        if (lambertian.has_light_direction()) {
            light_direction = {lambertian.light_direction().x(),
                               lambertian.light_direction().y(),
                               lambertian.light_direction().z()};
        }

        if (lambertian.has_light_color()) {
            light_color = {lambertian.light_color().x(), lambertian.light_color().y(), lambertian.light_color().z()};
        }

        if (lambertian.has_ambient_color()) {
            ambient_color
                = {lambertian.ambient_color().x(), lambertian.ambient_color().y(), lambertian.ambient_color().z()};
        }

        setUniform(light_direction_uniform_, light_direction);
        setUniform(light_color_uniform_, light_color);
        setUniform(ambient_color_uniform_, ambient_color);
        break;
    }
    return *this;
}

} // namespace gvs::vis
