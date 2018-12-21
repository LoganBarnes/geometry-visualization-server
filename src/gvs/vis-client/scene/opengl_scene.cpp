// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2018 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#include "opengl_scene.hpp"

#include <gvs/gvs_paths.hpp>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Trade/MeshData3D.h>
#include <imgui.h>

namespace gvs {
namespace vis {

namespace {

Magnum::MeshPrimitive from_proto(gvs::proto::GeometryFormat format) {
    switch (format) {
    case proto::POINTS:
        return Magnum::MeshPrimitive::Points;
    case proto::LINES:
        return Magnum::MeshPrimitive::Lines;
    case proto::LINE_STRIP:
        return Magnum::MeshPrimitive::LineStrip;
    case proto::TRIANGLES:
        return Magnum::MeshPrimitive::Triangles;
    case proto::TRIANGLE_STRIP:
        return Magnum::MeshPrimitive::TriangleStrip;
    case proto::TRIANGLE_FAN:
        return Magnum::MeshPrimitive::TriangleFan;

    /* Safe to ignore */
    case proto::GeometryFormat_INT_MIN_SENTINEL_DO_NOT_USE_:
        break;
    case proto::GeometryFormat_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    throw std::invalid_argument("Invalid GeometryFormat enum provided");
}

} // namespace

using namespace Magnum;
using namespace Math::Literals;

OpenGLScene::OpenGLScene(const SceneInitializationInfo& /*initialization_info*/) {
    camera_object_.setParent(&scene_).translate(Vector3::zAxis(5.0f));

    (*(camera_ = new SceneGraph::Camera3D{camera_object_})) // Memory control is handled elsewhere
        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    root_object_.setParent(&scene_);

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    colored_shader_.setAmbientColor(0x111111_rgbf).setSpecularColor(0xffffff_rgbf).setShininess(80.0f);

    // TMP
    mesh_.setCount(0);
    mesh_.setPrimitive(MeshPrimitive::Points);

    // TODO: Set camera
}

void OpenGLScene::update(const Vector2i& viewport) {

    auto transformation = Matrix4::rotationX(30.0_degf) * Matrix4::rotationY(40.0_degf);
    auto projection = Matrix4::perspectiveProjection(35.0_degf, Vector2{viewport}.aspectRatio(), 0.01f, 100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));
    auto color = Color3::fromHsv(35.0_degf, 1.0f, 1.0f);

    shader_.setLightPosition({7.0f, 5.0f, 2.5f})
        .setLightColor(Color3{1.0f})
        .setDiffuseColor(color)
        .setAmbientColor(Color3::fromHsv(color.hue(), 1.0f, 0.3f))
        .setTransformationMatrix(transformation)
        .setNormalMatrix(transformation.rotationScaling())
        .setProjectionMatrix(projection);
}

void OpenGLScene::render(const Vector2i& /*viewport*/) {
    mesh_.draw(shader_);
}

void OpenGLScene::configure_gui(const Vector2i& /*viewport*/) {
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "TODO: Display Scene Items");
}

void OpenGLScene::reset(const proto::SceneItems& items) {
    for (const auto& item : items.items()) {
        add_item(item.second);
        break; // only one for now while implementing
    }
}

void OpenGLScene::add_item(const proto::SceneItemInfo& info) {
    const Trade::MeshData3D cube = Primitives::cubeSolid();

    assert(info.has_geometry_info());
    const proto::GeometryInfo3D& geometry = info.geometry_info();

    std::vector<float> buffer_data;

    if (geometry.has_positions()) {
        const proto::FloatList& positions = geometry.positions();
        buffer_data.insert(buffer_data.begin(), positions.value().begin(), positions.value().end());
        mesh_.setCount(positions.value_size() / 3);
    }

    vertex_buffer_.setData(buffer_data);

    mesh_.addVertexBuffer(vertex_buffer_, 0, Shaders::Phong::Position{});

    if (info.geometry_info().has_indices() and info.geometry_info().indices().value_size() > 0) {
        std::vector<unsigned> indices{info.geometry_info().indices().value().begin(),
                                      info.geometry_info().indices().value().end()};

        Containers::Array<char> index_data;
        MeshIndexType index_type;
        UnsignedInt index_start, index_end;
        std::tie(index_data, index_type, index_start, index_end) = MeshTools::compressIndices(indices);
        index_buffer_.setData(index_data);

        mesh_.setCount(static_cast<int>(indices.size()))
            .setIndexBuffer(index_buffer_, 0, index_type, index_start, index_end);
    }

    if (info.has_display_info()) {
        const proto::DisplayInfo& display = info.display_info();

        if (display.has_geometry_format()) {
            mesh_.setPrimitive(from_proto(display.geometry_format().value()));
        }
    }
}

void OpenGLScene::resize(const Vector2i& /*viewport*/) {
    // TODO: Reset camera
}

} // namespace vis
} // namespace gvs
