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

// project
#include "gvs/scene/types.hpp"

// external
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Math/Color.h>

namespace gvs::display::backends {

class GeneralShader3d : public Magnum::GL::AbstractShaderProgram {
public:
    typedef Magnum::GL::Attribute<0, Magnum::Vector3> Position;
    typedef Magnum::GL::Attribute<1, Magnum::Vector3> Normal;
    typedef Magnum::GL::Attribute<2, Magnum::Vector2> TextureCoordinate;
    typedef Magnum::GL::Attribute<3, Magnum::Vector3> VertexColor;

    enum : Magnum::UnsignedInt { ColorOutput = 0, IdOutput = 1 };

    explicit GeneralShader3d();

    auto set_world_from_local_matrix(Magnum::Matrix4 const& view_from_local) -> GeneralShader3d&;
    auto set_world_from_local_normal_matrix(Magnum::Matrix3 const& view_from_local_normal) -> GeneralShader3d&;
    auto set_projection_from_local_matrix(Magnum::Matrix4 const& projection_from_local) -> GeneralShader3d&;

    auto set_coloring(Coloring const& coloring) -> GeneralShader3d&;
    auto set_uniform_color(Magnum::Color3 const& color) -> GeneralShader3d&;

    auto set_shading(Shading const& shading) -> GeneralShader3d&;
    auto set_id(unsigned const& id) -> GeneralShader3d&;

private:
    int projection_from_local_uniform_location_    = -1;
    int world_from_local_uniform_location_         = -1;
    int world_from_local_normals_uniform_location_ = -1;

    int coloring_uniform_location_      = -1;
    int uniform_color_uniform_location_ = -1;

    int shading_uniform_location_         = -1;
    int light_direction_uniform_location_ = -1;
    int light_color_uniform_location_     = -1;
    int ambient_scale_uniform_location_   = -1;

    int id_uniform_location_ = -1;
};

} // namespace gvs::display::backends
