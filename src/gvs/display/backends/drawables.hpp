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
#include "general_shader_3d.hpp"
#include "gvs/scene/types.hpp"

// external
#include <Magnum/SceneGraph/Drawable.h>

namespace gvs::display::backends {

class OpaqueDrawable : public Magnum::SceneGraph::Drawable3D {
public:
    explicit OpaqueDrawable(Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>& object,
                            Magnum::SceneGraph::DrawableGroup3D*                                    group,
                            Magnum::GL::Mesh&                                                       mesh,
                            unsigned                                                                intersect_id,
                            GeneralShader3d&                                                        shader);

    ~OpaqueDrawable() override = default;

    auto update_display_info(const DisplayInfo& display_info) -> void;

private:
    void draw(Magnum::Matrix4 const& transformation_matrix, Magnum::SceneGraph::Camera3D& camera) override;

    Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>& object_;
    Magnum::GL::Mesh&                                                       mesh_;

    Coloring       coloring_      = default_coloring;
    Magnum::Color3 uniform_color_ = {default_uniform_color[0], default_uniform_color[1], default_uniform_color[2]};
    Shading        shading_       = default_shading;
    unsigned       intersect_id_  = 0u;

    GeneralShader3d& shader_;
};

} // namespace gvs::display::backends
