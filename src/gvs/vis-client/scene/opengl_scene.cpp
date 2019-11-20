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
#include "opengl_scene.hpp"

#include "gvs/util/container_util.hpp"
#include "gvs/vis-client/scene/drawables.hpp"

#include <gvs/gvs_paths.hpp>

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Trade/MeshData3D.h>
#include <imgui.h>

namespace gvs::vis {

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

OpenGLScene::ObjectMeshPackage::ObjectMeshPackage(Object3D* obj,
                                                  Magnum::SceneGraph::DrawableGroup3D* drawables,
                                                  GeneralShader3D& shader)
    : object(obj) {
    mesh.setCount(0).setPrimitive(Magnum::MeshPrimitive::Points);
    drawable = new OpaqueDrawable(*object, drawables, mesh, shader);
}

OpenGLScene::OpenGLScene(const SceneInitializationInfo& /*initialization_info*/) {
    camera_object_.setParent(&scene_);
    camera_ = new SceneGraph::Camera3D(camera_object_); // Memory control is handled elsewhere

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    shader_.set_uniform_color({1.f, 0.5f, 0.1f});

    reset({});
}

void OpenGLScene::update(const Vector2i& /*viewport*/) {}

void OpenGLScene::render(const CameraPackage& camera_package) {
    camera_object_.setTransformation(camera_package.transformation);
    camera_->setProjectionMatrix(camera_package.camera->projectionMatrix());
    camera_->draw(drawables_);
}

void OpenGLScene::configure_gui(const Vector2i& /*viewport*/) {
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "TODO: Display Scene Items");
}

void OpenGLScene::reset(const proto::SceneItems& items) {
    // Remove all items from the scene
    if (root_object_) {
        scene_.children().erase(root_object_);
    }
    objects_.clear();

    // Add root
    root_object_ = &scene_.addChild<Object3D>();
    objects_.emplace("", std::make_unique<ObjectMeshPackage>(root_object_, &drawables_, shader_));

    for (const auto& item : items.items()) {
        objects_.emplace(item.first,
                         std::make_unique<ObjectMeshPackage>(&scene_.addChild<Object3D>(), &drawables_, shader_));
    }

    // Add new items to scene
    for (const auto& item : items.items()) {
        update_item(item.second);
    }
}

void OpenGLScene::add_item(const proto::SceneItemInfo& info) {
    assert(info.has_geometry_info());

    objects_.emplace(info.id().value(),
                     std::make_unique<ObjectMeshPackage>(&scene_.addChild<Object3D>(), &drawables_, shader_));
    update_item(info);
}

void OpenGLScene::update_item(const proto::SceneItemInfo& info) {

    ObjectMeshPackage& mesh_package = *objects_.at(info.id().value());

    // TODO: Make it so individual parts of the geometry can be updated
    if (info.has_geometry_info()) {
        const proto::GeometryInfo3D& geometry = info.geometry_info();

        std::vector<float> buffer_data;
        GLintptr offset = 0;
        mesh_package.mesh.setCount(0);

        if (geometry.has_positions()) {
            const proto::FloatList& positions = geometry.positions();
            buffer_data.insert(buffer_data.end(), positions.value().begin(), positions.value().end());
            mesh_package.mesh.setCount(positions.value_size() / 3);
            mesh_package.mesh.addVertexBuffer(mesh_package.vertex_buffer, offset, GeneralShader3D::Position{});
            offset += positions.value_size() * static_cast<int>(sizeof(float));
        }

        if (geometry.has_normals()) {
            const proto::FloatList& normals = geometry.normals();
            buffer_data.insert(buffer_data.end(), normals.value().begin(), normals.value().end());
            mesh_package.mesh.addVertexBuffer(mesh_package.vertex_buffer, offset, GeneralShader3D::Normal{});
            offset += normals.value_size() * static_cast<int>(sizeof(float));
        }

        if (geometry.has_tex_coords()) {
            const proto::FloatList& tex_coords = geometry.tex_coords();
            buffer_data.insert(buffer_data.end(), tex_coords.value().begin(), tex_coords.value().end());
            mesh_package.mesh.addVertexBuffer(mesh_package.vertex_buffer, offset, GeneralShader3D::TextureCoordinate{});
            offset += tex_coords.value_size() * static_cast<int>(sizeof(float));
        }

        if (geometry.has_vertex_colors()) {
            const proto::FloatList& vertex_colors = geometry.vertex_colors();
            buffer_data.insert(buffer_data.end(), vertex_colors.value().begin(), vertex_colors.value().end());
            mesh_package.mesh.addVertexBuffer(mesh_package.vertex_buffer, offset, GeneralShader3D::VertexColor{});
        }

        mesh_package.vertex_buffer.setData(buffer_data, GL::BufferUsage::StaticDraw);

        if (info.geometry_info().has_indices() and info.geometry_info().indices().value_size() > 0) {
            std::vector<unsigned> indices{info.geometry_info().indices().value().begin(),
                                          info.geometry_info().indices().value().end()};

            Containers::Array<char> index_data;
            MeshIndexType index_type;
            UnsignedInt index_start, index_end;
            std::tie(index_data, index_type, index_start, index_end) = MeshTools::compressIndices(indices);
            mesh_package.index_buffer.setData(index_data, GL::BufferUsage::StaticDraw);

            mesh_package.mesh.setCount(static_cast<int>(indices.size()))
                .setIndexBuffer(mesh_package.index_buffer, 0, index_type, index_start, index_end);
        }
    }

    if (info.has_display_info()) {
        const proto::DisplayInfo& display = info.display_info();

        if (display.has_geometry_format()) {
            mesh_package.mesh.setPrimitive(from_proto(display.geometry_format().value()));
        }
    }

    if (not util::has_key(objects_, info.parent().value())) {
        objects_.erase(info.id().value());
        throw std::invalid_argument("Parent id '" + info.parent().value() + "' not found in scene");
    }

    mesh_package.object->setParent(objects_.at(info.parent().value())->object);

    if (info.has_display_info()) {
        mesh_package.drawable->update_display_info(info.display_info());
    }
}

void OpenGLScene::resize(const Vector2i& /*viewport*/) {}

std::size_t OpenGLScene::items() const {
    // always contains root item
    assert(!objects_.empty());
    return objects_.size() - 1u;
}

bool OpenGLScene::empty() const {
    return items() == 0;
}

} // namespace gvs::vis
