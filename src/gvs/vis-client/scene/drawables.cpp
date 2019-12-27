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
#include "drawables.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/SceneGraph.h>

using namespace Magnum;

namespace gvs::vis {

OpaqueDrawable::OpaqueDrawable(SceneGraph::Object<SceneGraph::MatrixTransformation3D>& object,
                               SceneGraph::DrawableGroup3D*                            group,
                               GL::Mesh&                                               mesh,
                               GeneralShader3D&                                        shader)
    : SceneGraph::Drawable3D{object, group}, object_(object), mesh_(mesh), shader_(shader) {}

void OpaqueDrawable::update_display_info(const gvs::proto::DisplayInfo& display_info) {

    if (display_info.has_transformation()) {
        assert(display_info.transformation().data_size() == 16);
        const float* transform_data = display_info.transformation().data().data();

        Matrix4 transform{};
        std::copy(transform_data, transform_data + 16, transform.data());
        object_.setTransformation(transform);
    }

    if (display_info.has_coloring()) {
        coloring_ = display_info.coloring().value();
    }

    if (display_info.has_uniform_color()) {
        uniform_color_ = Magnum::Color3{display_info.uniform_color().x(),
                                        display_info.uniform_color().y(),
                                        display_info.uniform_color().z()};
    }

    if (display_info.has_shading()) {
        shading_.CopyFrom(display_info.shading());
    }
}

void OpaqueDrawable::draw(const Matrix4& transformation_matrix, SceneGraph::Camera3D& camera) {
    // unnecessarily expensive doing the inverse every time.
    auto world_transform = camera.cameraMatrix().inverted() * transformation_matrix;
    // TODO: figure out how to cache the variable since the camera won't changer during a render
    //  (OR) firgure out how to get the transformation_matrix without the camera transform part

    shader_.set_world_from_local_matrix(world_transform)
        .set_world_from_local_normal_matrix(world_transform.rotationScaling())
        .set_projection_from_local_matrix(camera.projectionMatrix() * transformation_matrix)
        .set_coloring(coloring_)
        .set_uniform_color(uniform_color_)
        .set_shading(shading_);

    mesh_.draw(shader_);
}

} // namespace gvs::vis
