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

#include "gvs/vis-client/scene/drawables.hpp"

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
    camera_object_.setParent(&scene_);
    camera_ = new SceneGraph::Camera3D(camera_object_); // Memory control is handled elsewhere

    root_object_ = &scene_.addChild<Object3D>();

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    shader_.set_global_color({1.f, 0.5f, 0.1f});
}

void OpenGLScene::update(const Vector2i& /*viewport*/) {}

void OpenGLScene::render(const Magnum::Matrix4& camera_transformation, Magnum::SceneGraph::Camera3D* camera) {
    camera_object_.setTransformation(camera_transformation);
    camera_->setProjectionMatrix(camera->projectionMatrix());
    camera_->draw(drawables_);
}

void OpenGLScene::configure_gui(const Vector2i& /*viewport*/) {
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "TODO: Display Scene Items");
}

void OpenGLScene::reset(const proto::SceneItems& items) {
    // Remove all items from the scene
    scene_.children().erase(root_object_);
    root_object_ = &scene_.addChild<Object3D>();

    // Add new items to scene
    for (const auto& item : items.items()) {
        add_item(item.second);
    }
}

void OpenGLScene::add_item(const proto::SceneItemInfo& info) {
    const Trade::MeshData3D cube = Primitives::cubeSolid();

    assert(info.has_geometry_info());
    const proto::GeometryInfo3D& geometry = info.geometry_info();

    std::vector<float> buffer_data;

    Magnum::GL::Buffer index_buffer, vertex_buffer;
    GL::Mesh mesh;
    mesh.setPrimitive(MeshPrimitive::Points);

    if (geometry.has_positions()) {
        const proto::FloatList& positions = geometry.positions();
        buffer_data.insert(buffer_data.begin(), positions.value().begin(), positions.value().end());
        mesh.setCount(positions.value_size() / 3);
    }

    vertex_buffer.setData(buffer_data);

    mesh.addVertexBuffer(vertex_buffer, 0, GeneralShader3D::Position{});

    if (info.geometry_info().has_indices() and info.geometry_info().indices().value_size() > 0) {
        std::vector<unsigned> indices{info.geometry_info().indices().value().begin(),
                                      info.geometry_info().indices().value().end()};

        Containers::Array<char> index_data;
        MeshIndexType index_type;
        UnsignedInt index_start, index_end;
        std::tie(index_data, index_type, index_start, index_end) = MeshTools::compressIndices(indices);
        index_buffer.setData(index_data);

        mesh.setCount(static_cast<int>(indices.size()))
            .setIndexBuffer(index_buffer, 0, index_type, index_start, index_end);
    }

    if (info.has_display_info()) {
        const proto::DisplayInfo& display = info.display_info();

        if (display.has_geometry_format()) {
            mesh.setPrimitive(from_proto(display.geometry_format().value()));
        }
    }

    meshes_.emplace_back(std::make_unique<GL::Mesh>(std::move(mesh)));

    // Self deleting
    new OpaqueDrawable(*root_object_, &drawables_, *meshes_.back(), shader_);
}

void OpenGLScene::resize(const Vector2i& /*viewport*/) {}

} // namespace vis
} // namespace gvs
