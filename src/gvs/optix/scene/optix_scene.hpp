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

#include "gvs/vis-client/scene/scene_interface.hpp"

#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/OpenGL.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/Shaders/Flat.h>
#include <optix.h>
#include <optixu/optixpp_namespace.h>

#include <memory>
#include <unordered_map>

namespace gvs {
namespace vis {

class OptiXScene : public SceneInterface {
public:
    OptiXScene();
    ~OptiXScene() override;

    void update(const Magnum::Vector2i& viewport) override;
    void render(const Magnum::Vector2i& viewport) override;
    void configure_gui(const Magnum::Vector2i& viewport) override;

    void reset(const proto::SceneItems& items) override;
    void add_item(const proto::SceneItemInfo& info) override;

private:
    std::shared_ptr<optix::Context> context_;

    Magnum::GL::BufferImage2D buffer_image_;
    Magnum::GL::Texture2D display_texture_;
    Magnum::Shaders::Flat2D screenspace_shader_;
    Magnum::GL::Mesh fullscreen_quad_;

    std::unordered_map<std::string, std::string> ptx_files_;

    optix::Context& context();
};

} // namespace vis
} // namespace gvs
