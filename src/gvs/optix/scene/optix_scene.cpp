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
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Image.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelStorage.h>
#include <Magnum/Shaders/Flat.h>
#include <imgui.h>

// This define shouldn't be here. TODO: Figure out how to include the header properly
#define __CUDA_INCLUDE_COMPILER_INTERNAL_HEADERS__
#include <optixu/optixu_math_namespace.h>

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

GL::Mesh set_up_fullscreen_quad() {
    struct Vertex {
        Vector2 position;
        Vector2 texture_coordinates;
    };
    std::vector<Vertex> vertex_data_2d = {
        {{-1.f, -1.f}, {0.f, 1.f}}, // Lower left
        {{+1.f, -1.f}, {1.f, 1.f}}, // Lower right
        {{-1.f, +1.f}, {0.f, 0.f}}, // Upper left
        {{+1.f, +1.f}, {1.f, 0.f}}, // Upper right
    };

    GL::Buffer vertices;
    vertices.setData(vertex_data_2d);

    GL::Mesh quad_mesh;
    quad_mesh.setCount(4)
        .setPrimitive(MeshPrimitive::TriangleStrip)
        .addVertexBuffer(vertices, 0, Shaders::Flat2D::Position{}, Shaders::Flat2D::TextureCoordinates{});

    return quad_mesh;
}

} // namespace

OptiXScene::OptiXScene()
    : context_(new optix::Context(optix::Context::create()),
               [](auto* p) {
                   static_assert(std::is_same<decltype(p), optix::Context*>::value, "");
                   (*p)->destroy();
                   delete p;
               })
    , buffer_image_(GL::PixelFormat::RGBA, GL::PixelType::Float)
    , screenspace_shader_(Shaders::Flat2D::Flag::Textured)
    , fullscreen_quad_(set_up_fullscreen_quad())
    , ptx_files_(build_ptx_file_map()) {

    display_texture_.setStorage(0, GL::TextureFormat::RGBA32F, {0, 0});

    for (const auto& file_pair : ptx_files_) {
        std::cout << file_pair.first << ": " << file_pair.second << std::endl;
    }

    optix::Context& ctx = *context_;

    // Set up context
    ctx->setRayTypeCount(2);
    ctx->setEntryPointCount(1);

    ctx["radiance_ray_type"]->setUint(0u);
    //    ctx["shadow_ray_type"]->setUint(1u);
    ctx["scene_epsilon"]->setFloat(1.e-2f);

    // Starting program to generate rays
    std::string ptx_file = ptx_files_.at("default_programs.ptx");
    ctx->setRayGenerationProgram(0, ctx->createProgramFromPTXFile(ptx_file, "debug_test"));

    set_up_fullscreen_quad();
}

OptiXScene::~OptiXScene() = default;

void OptiXScene::update(const Vector2i& viewport) {
    if (viewport != display_texture_.imageSize(0)) {

        display_texture_ = GL::Texture2D();

        {
            auto total_size = static_cast<std::size_t>(viewport.x()) * static_cast<std::size_t>(viewport.y());

            std::vector<optix::float4> buffer_data(total_size);

            for (int yi = 0; yi < viewport.y(); ++yi) {
                for (int xi = 0; xi < viewport.x(); ++xi) {
                    buffer_data[static_cast<std::size_t>(yi) * static_cast<std::size_t>(viewport.x())
                                + static_cast<std::size_t>(xi)]
                        = optix::make_float4(xi * 1.f / viewport.x(), yi * 1.f / viewport.y(), 1.f, 1.f);
                }
            }

            GL::Buffer buffer;
            buffer.setData(buffer_data);

            buffer_image_ = GL::BufferImage2D(GL::PixelFormat::RGBA,
                                              GL::PixelType::Float,
                                              viewport,
                                              std::move(buffer),
                                              total_size * sizeof(optix::float4));

            display_texture_.setWrapping(GL::SamplerWrapping::ClampToEdge)
                .setMagnificationFilter(GL::SamplerFilter::Linear)
                .setMinificationFilter(GL::SamplerFilter::Linear)
                .setStorage(1, GL::TextureFormat::RGBA32F, viewport);

            optix::Buffer output_buffer(context()->createBufferFromGLBO(RT_BUFFER_OUTPUT, buffer_image_.buffer().id()));

            output_buffer->setFormat(RT_FORMAT_FLOAT4);
            output_buffer->setSize(static_cast<unsigned>(viewport.x()), static_cast<unsigned>(viewport.y()));

            context()["output_buffer"]->set(output_buffer);

            context()->validate();
        }

        std::cout << "Resize: " << display_texture_.imageSize(0).x() << ", " << display_texture_.imageSize(0).y()
                  << std::endl;
    }
}

void OptiXScene::render(const Vector2i& /*viewport*/) {
    // Get buffer size for ray tracing call
    optix::Buffer buffer = context()["output_buffer"]->getBuffer();
    RTsize buffer_width, buffer_height;
    buffer->getSize(buffer_width, buffer_height);

    // Ray trace the scene
    context()->launch(0, buffer_width, buffer_height);

    // Bind ray traced buffer to texture for displaying
    display_texture_.setSubImage(0, {}, buffer_image_);

    // Display the texture
    Matrix3 identity(Math::IdentityInit);
    screenspace_shader_.setTransformationProjectionMatrix(identity).bindTexture(display_texture_);
    fullscreen_quad_.draw(screenspace_shader_);
}

void OptiXScene::configure_gui(const Vector2i& /*viewport*/) {
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "TODO: Display Scene Items");
}

void OptiXScene::reset(const proto::SceneItems& /*items*/) {}

void OptiXScene::add_item(const proto::SceneItemInfo& /*info*/) {}
optix::Context& OptiXScene::context() {
    return *context_;
}

} // namespace vis

} // namespace gvs
