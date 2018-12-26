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

#include "gvs/vis-client/scene/general_shader_3d.hpp"

#include <gvs/types.grpc.pb.h>

#include <Magnum/SceneGraph/Drawable.h>

namespace gvs {
namespace vis {

class OpaqueDrawable : public Magnum::SceneGraph::Drawable3D {
public:
    explicit OpaqueDrawable(Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>& object,
                            Magnum::SceneGraph::DrawableGroup3D* group,
                            Magnum::GL::Mesh& mesh,
                            const proto::DisplayInfo* display_info,
                            GeneralShader3D& shader);

    ~OpaqueDrawable() override = default;

private:
    void draw(const Magnum::Matrix4& transformation_matrix, Magnum::SceneGraph::Camera3D& camera) override;

    Magnum::GL::Mesh& mesh_;
    proto::DisplayMode display_mode_ = proto::DisplayMode::GLOBAL_COLOR;
    Magnum::Color3 global_color_ = {1.f, 0.9f, 0.7f};
    GeneralShader3D& shader_;
};

} // namespace vis
} // namespace gvs
