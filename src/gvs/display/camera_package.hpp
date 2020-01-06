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

// external
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>

namespace gvs::display {

struct Ray {
    Magnum::Vector3 origin;
    Magnum::Vector3 direction;
};

struct CameraPackage {
    virtual ~CameraPackage() = default;

    Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> object;
    Magnum::SceneGraph::Camera3D*                                          camera = nullptr;

    void set_camera(Magnum::SceneGraph::Camera3D* cam, const Magnum::Vector2i& viewport);

    void update_viewport(const Magnum::Vector2i& viewport);

    Ray get_camera_ray_from_window_pos(const Magnum::Vector2& mouse_position);
};

struct OrbitCameraPackage : CameraPackage {
    ~OrbitCameraPackage() override = default;

    Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> zoom_object;
    Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> rotation_object;
    Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> translation_object;

    void update_object();
};

} // namespace gvs::display
