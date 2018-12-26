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
#include "camera_package.hpp"

#include <Magnum/Math/Functions.h>
#include <Magnum/Math/Math.h>

namespace gvs {
namespace vis {

using namespace Magnum;
using namespace Math::Literals;

constexpr float camera_near_dist = 0.01f;
constexpr float camera_far_dist = 1000.f;
constexpr Math::Deg<float> camera_fovy = 35.0_degf;

void CameraPackage::set_camera(Magnum::SceneGraph::Camera3D* cam, const Vector2i& viewport) {
    camera = cam;
    camera->setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(camera_fovy, 1.0f, camera_near_dist, camera_far_dist));

    update_viewport(viewport);
}

void CameraPackage::update_viewport(const Vector2i& viewport) {
    if (not camera) {
        return;
    }

    camera->setViewport(viewport);

    float far_height = camera_far_dist * Math::tan(Math::Rad<float>(camera_fovy) * 0.5f);
    float far_width = Vector2(viewport).aspectRatio() * far_height;
    inverse_scale.setRow(0u, {far_width, 0.f, 0.f, 0.f});
    inverse_scale.setRow(1u, {0.f, far_height, 0.f, 0.f});
    inverse_scale.setRow(2u, {0.f, 0.f, camera_far_dist, 0.f});
    inverse_scale.setRow(3u, {0.f, 0.f, 0.f, 1.f});
}

} // namespace vis
} // namespace gvs
