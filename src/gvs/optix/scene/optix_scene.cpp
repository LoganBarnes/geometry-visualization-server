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
#include <Magnum/Math/Math.h>
#include <Magnum/Mesh.h>
#include <Magnum/PixelStorage.h>
#include <Magnum/Shaders/Flat.h>
#include <imgui.h>

// This define shouldn't be here. TODO: Figure out how to include the header properly
#define __CUDA_INCLUDE_COMPILER_INTERNAL_HEADERS__
#include <optixu/optixu_math_namespace.h>

#include <iostream>
#include <limits>
#include <sstream>

using namespace Magnum;

namespace gvs {
namespace vis {

constexpr optix::float3 error_color = {1.f, 0.f, 1.f};

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

void build_ray_basis_vectors(const CameraPackage& camera_package, Vector3* u_out, Vector3* v_out, Vector3* w_out) {
    assert(u_out != nullptr);
    assert(v_out != nullptr);
    assert(w_out != nullptr);

    Vector3& u = *u_out;
    Vector3& v = *v_out;
    Vector3& w = *w_out;

    // normalized directions
    w = camera_package.transformation.transformPoint({0.f, 0.f, -1.f});
    u = Math::cross(w, {0.f, 1.f, 0.f}).normalized();
    v = Math::cross(u, w).normalized();

    Matrix4 inv_scale_view = camera_package.transformation * camera_package.inverse_scale;

    // (1, 1, -1) in clip space back to world space
    Vector3 window_space(1.f, 1.f, -1.f);
    Vector3 far_top_right_point = inv_scale_view.transformPoint(window_space);
    Vector3 far_top_right_dir = far_top_right_point - camera_package.transformation.transformPoint({0.f, 0.f, 0.f});

    // scaled to match current camera matrices
    u *= Math::dot(u, far_top_right_dir);
    v *= Math::dot(v, far_top_right_dir);
    w *= Math::dot(w, far_top_right_dir);
}

} // namespace

OptiXScene::OptiXScene(const SceneInitializationInfo& initialization_info)
    : context_(new optix::Context(optix::Context::create()),
               [](auto* p) {
                   static_assert(std::is_same<decltype(p), optix::Context*>::value, "");
                   (*p)->destroy();
                   delete p;
               })
    , buffer_image_(GL::PixelFormat::RGBA, GL::PixelType::Float)
    , screenspace_shader_(Shaders::Flat2D::Flag::Textured)
    , fullscreen_quad_(set_up_fullscreen_quad())
    , root_group_(context())
    , ptx_files_(build_ptx_file_map()) {

    camera_object_.setParent(&scene_);
    camera_ = new SceneGraph::Camera3D(camera_object_); // Memory control is handled elsewhere

    optix::Context& ctx = *context_;

    // Set up context
    ctx->setRayTypeCount(2);
    ctx->setEntryPointCount(1);

    ctx["radiance_ray_type"]->setUint(0u);
    //    ctx["shadow_ray_type"]->setUint(1u);
    ctx["scene_epsilon"]->setFloat(1.e-2f);

    // Starting program to generate rays
    std::string ptx_file = ptx_files_.at("default_programs.ptx");
    ctx->setRayGenerationProgram(0, ctx->createProgramFromPTXFile(ptx_file, "pinhole_camera"));

    // What to do when something messes up
    ctx->setExceptionProgram(0, ctx->createProgramFromPTXFile(ptx_file, "exception"));
    ctx["error_color"]->setFloat(error_color);

    // What to do when rays don't intersect with anything
    ctx->setMissProgram(0, ctx->createProgramFromPTXFile(ptx_file, "miss"));
    ctx["background_color"]->setFloat(initialization_info.background_color.r(),
                                      initialization_info.background_color.g(),
                                      initialization_info.background_color.b());
    ctx["miss_depth"]->setFloat(std::numeric_limits<float>::infinity());

    set_up_fullscreen_quad();

    // Root object
    root_group_()->setAcceleration(ctx->createAcceleration("Bvh"));

    ctx["top_object"]->set(root_group_());

    // Set up output buffer
    optix::Buffer output_buffer(context()->createBufferFromGLBO(RT_BUFFER_OUTPUT, pixel_buffer_.id()));
    output_buffer->setFormat(RT_FORMAT_FLOAT4);

    context()["output_buffer"]->set(output_buffer);

    resize(initialization_info.viewport_size);
}

OptiXScene::~OptiXScene() = default;

void OptiXScene::update(const Vector2i& /*viewport*/) {}

void OptiXScene::render(const CameraPackage& camera_package) {
    camera_object_.setTransformation(camera_package.transformation);
    camera_->setProjectionMatrix(camera_package.camera->projectionMatrix());

    Vector3 eye = camera_package.transformation.transformPoint({0.f, 0.f, 0.f});
    Vector3 u, v, w;

    build_ray_basis_vectors(camera_package, &u, &v, &w);

    context()["eye"]->setFloat(eye.x(), eye.y(), eye.z());
    context()["U"]->setFloat(u.x(), u.y(), u.z());
    context()["V"]->setFloat(v.x(), v.y(), v.z());
    context()["W"]->setFloat(w.x(), w.y(), w.z());

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

void OptiXScene::update_item(const proto::SceneItemInfo& /*info*/) {}

optix::Context& OptiXScene::context() {
    return *context_;
}

void OptiXScene::resize(const Vector2i& viewport) {

    optix::Buffer optix_output_buffer = context()["output_buffer"]->getBuffer();
    optix_output_buffer->setSize(static_cast<unsigned>(viewport.x()), static_cast<unsigned>(viewport.y()));

    optix_output_buffer->unregisterGLBuffer();

    auto total_size = static_cast<std::size_t>(viewport.x()) * static_cast<std::size_t>(viewport.y());

    std::vector<optix::float4> buffer_data(total_size);

    for (int yi = 0; yi < viewport.y(); ++yi) {
        for (int xi = 0; xi < viewport.x(); ++xi) {
            buffer_data[static_cast<std::size_t>(yi) * static_cast<std::size_t>(viewport.x())
                        + static_cast<std::size_t>(xi)]
                = optix::make_float4(xi * 1.f / viewport.x(), yi * 1.f / viewport.y(), 1.f, 1.f);
        }
    }

    pixel_buffer_.setData(buffer_data);

    buffer_image_ = GL::BufferImage2D(GL::PixelFormat::RGBA,
                                      GL::PixelType::Float,
                                      viewport,
                                      GL::Buffer::wrap(pixel_buffer_.id()),
                                      total_size * sizeof(optix::float4));

    // texture is immutable after 'setStorage' call so we have to make a new one every time
    display_texture_ = GL::Texture2D();
    display_texture_.setWrapping(GL::SamplerWrapping::ClampToEdge)
        .setMagnificationFilter(GL::SamplerFilter::Linear)
        .setMinificationFilter(GL::SamplerFilter::Linear)
        .setStorage(1, GL::TextureFormat::RGBA32F, viewport);

    optix_output_buffer->registerGLBuffer();
    context()->validate();
}

} // namespace vis

} // namespace gvs
