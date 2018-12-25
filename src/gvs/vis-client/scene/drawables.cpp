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
#include "drawables.hpp"

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/SceneGraph.h>

using namespace Magnum;

namespace gvs {
namespace vis {

OpaqueDrawable::OpaqueDrawable(SceneGraph::Object<SceneGraph::MatrixTransformation3D>& object,
                               SceneGraph::DrawableGroup3D* group,
                               GL::Mesh& mesh,
                               GeneralShader3D& shader)
    : SceneGraph::Drawable3D{object, group}, mesh_(mesh), shader_(shader) {}

void OpaqueDrawable::draw(const Matrix4& transformation_matrix, SceneGraph::Camera3D& camera) {
    shader_.set_transformation_matrix(transformation_matrix)
        .set_normal_matrix(transformation_matrix.rotationScaling())
        .set_projection_matrix(camera.projectionMatrix());

    mesh_.draw(shader_);
}

} // namespace vis
} // namespace gvs
