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

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>

namespace gvs {
namespace vis {

class GeneralShader3D : public Magnum::GL::AbstractShaderProgram {
public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector3> Position;
    typedef Magnum::GL::Attribute<1, Magnum::Vector3> Normal;
    typedef Magnum::GL::Attribute<2, Magnum::Vector2> TextureCoordinate;
    typedef Magnum::GL::Attribute<3, Magnum::Vector3> VertexColor;

    explicit GeneralShader3D();

    GeneralShader3D& set_world_from_local_matrix(const Magnum::Matrix4& view_from_local);
    GeneralShader3D& set_world_from_local_normal_matrix(const Magnum::Matrix3& view_from_local_normal);
    GeneralShader3D& set_projection_from_local_matrix(const Magnum::Matrix4& projection_from_local);

    GeneralShader3D& set_coloring(const proto::Coloring& coloring);
    GeneralShader3D& set_uniform_color(const Magnum::Color3& color);

    GeneralShader3D& set_shading(const proto::Shading& shading);

private:
    int projection_from_local_uniform_;
    int world_from_local_uniform_;
    int world_from_local_normals_uniform_;

    int coloring_uniform_;
    int uniform_color_uniform_;

    int shading_uniform_;
    int light_direction_uniform_;
    int light_color_uniform_;
    int ambient_color_uniform_;
};

} // namespace vis
} // namespace gvs
