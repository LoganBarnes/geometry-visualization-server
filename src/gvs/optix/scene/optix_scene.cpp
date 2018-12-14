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
#include "optix_scene.hpp"

#include <gvs/optix/ptx_files.hpp>

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/ArrayView.h>
#include <Magnum/Array.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/BufferImage.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/PixelStorage.h>

#include <iostream>
#include <sstream>

using namespace Magnum;

namespace gvs {
namespace vis {

namespace {

std::unordered_map<std::string, std::string> build_ptx_file_map() {
    std::unordered_map<std::string, std::string> ptx_files;

    std::istringstream path_stream(ptx::ptx_files);
    std::string path;
    while (std::getline(path_stream, path, ';')) {
        auto path_end = path.rfind('/');
        ptx_files.emplace(path.substr(path_end + 1), path);
    }

    return ptx_files;
}

} // namespace

OptiXScene::OptiXScene()
    : context_(new optix::Context(optix::Context::create()),
               [](auto* p) {
                   static_assert(std::is_same<decltype(p), optix::Context*>::value, "");
                   (*p)->destroy();
                   delete p;
               })
    , display_texture_(std::make_unique<Magnum::GL::Texture2D>())
    , ptx_files_(build_ptx_file_map()) {

    pbo_id_ = std::shared_ptr<GLuint>(new GLuint, [](auto* id) {
        glDeleteBuffers(1, id);
        delete id;
    });
    glGenBuffers(1, pbo_id_.get());

    display_texture_->setStorage(0, GL::TextureFormat::RGBA32F, {0, 0});

    for (const auto& file_pair : ptx_files_) {
        std::cout << file_pair.first << ": " << file_pair.second << std::endl;
    }

    optix::Context& ctx = *context_;

    // Set up context
    ctx->setRayTypeCount(2);
    ctx->setEntryPointCount(1);

    ctx["radiance_ray_type"]->setUint(0u);
    ctx["shadow_ray_type"]->setUint(1u);
    ctx["scene_epsilon"]->setFloat(1.e-2f);
}

OptiXScene::~OptiXScene() = default;

void OptiXScene::update(const Magnum::Vector2i& viewport) {
    if (viewport != display_texture_->imageSize(0)) {
        //        display_texture_->setStorage(0, GL::TextureFormat::RGB32F, {viewport.x(), viewport.y()});

        display_texture_ = std::make_unique<Magnum::GL::Texture2D>();

        {
            auto total_size = static_cast<std::size_t>(viewport.x()) * static_cast<std::size_t>(viewport.y());

            GL::Buffer buffer;
            buffer.setData(std::vector<optix::float4>(total_size));

            GL::BufferImage2D buffer_image(GL::PixelFormat::RGBA,
                                           GL::PixelType::Float,
                                           viewport,
                                           std::move(buffer),
                                           total_size * sizeof(optix::float4));

            display_texture_->setImage(0, GL::TextureFormat::RGBA32F, buffer_image);

            optix::Buffer output_buffer(context()->createBufferFromGLBO(RT_BUFFER_OUTPUT, buffer_image.buffer().id()));

            output_buffer->setFormat(RT_FORMAT_FLOAT4);
            output_buffer->setSize(static_cast<unsigned>(viewport.x()), static_cast<unsigned>(viewport.y()));

            context()["output_buffer"]->set(output_buffer);
        }

        std::cout << "Size: " << display_texture_->imageSize(0).x() << ", " << display_texture_->imageSize(0).y()
                  << std::endl;
    }
}

void OptiXScene::render(const Magnum::Vector2i& /*viewport*/) {}

void OptiXScene::configure_gui(const Magnum::Vector2i& /*viewport*/) {}

void OptiXScene::reset(const proto::SceneItems& /*items*/) {}

void OptiXScene::add_item(const proto::SceneItemInfo& /*info*/) {}
optix::Context& OptiXScene::context() {
    return *context_;
}

} // namespace vis

} // namespace gvs
