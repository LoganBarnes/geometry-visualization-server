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
#include "camera_package.hpp"

// external
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Math/Functions.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

namespace gvs::display {

using namespace Magnum;
using namespace Math::Literals;

constexpr float            camera_near_dist = 1.f;
constexpr float            camera_far_dist  = 1000.f;
constexpr Math::Rad<Float> camera_fovy      = 35.0_degf;

void CameraPackage::set_camera(Magnum::SceneGraph::Camera3D* cam, const Vector2i& viewport) {
    camera = cam;
    camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(camera_fovy, 1.0f, camera_near_dist, camera_far_dist));
    //        .setProjectionMatrix(Matrix4::orthographicProjection({100.f, 100.f}, camera_near_dist, camera_far_dist));

    update_viewport(viewport);
}

void CameraPackage::update_viewport(const Vector2i& viewport) {
    if (not camera) {
        return;
    }
    camera->setViewport(viewport);
}

Ray CameraPackage::get_camera_ray_from_window_pos(const Vector2& mouse_position) {

    Vector2 near_pos_xy = (Vector2{mouse_position} / Vector2{GL::defaultFramebuffer.viewport().size()} - Vector2{0.5f})
        * Vector2::yScale(-1.0f) * camera->projectionSize();

    Vector3 near_point = object.absoluteTransformation().transformPoint({near_pos_xy, -1.f});
    Vector3 eye_point  = object.absoluteTransformation().transformPoint({0.f, 0.f, 0.f});

    Vector3 direction = (near_point - eye_point);
    direction         = direction.normalized();

    return {eye_point, direction};
}
void OrbitCameraPackage::update_object() {
    object.setTransformation(translation_object.transformation() * rotation_object.transformation()
                             * zoom_object.transformation());
}

} // namespace gvs::display
