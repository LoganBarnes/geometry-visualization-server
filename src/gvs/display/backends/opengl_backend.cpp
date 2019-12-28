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
#include "opengl_backend.hpp"

// project
#include "gvs/util/container_util.hpp"
#include "gvs/util/result.hpp"
#include "to_magnum.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>

// standard
#include <iostream>

namespace gvs::scene::backends {
namespace {

auto update_vbo(OpenglBackend::ObjectMeshPackage* mesh_package,
                GeometryInfo const&               new_geom,
                GeometryInfo const&               old_geom) -> void {

    std::vector<float> buffer_data;
    GLintptr           offset = 0;
    mesh_package->vbo_count   = 0;

    auto update_attribute
        = [&mesh_package, &offset, &buffer_data](auto const& optional_attribute, auto const& shader_attribute) {
              const auto& attribute = *optional_attribute;
              buffer_data.insert(buffer_data.end(),
                                 reinterpret_cast<float const*>(attribute.data()),
                                 reinterpret_cast<float const*>(attribute.data() + attribute.size()));
              mesh_package->mesh.addVertexBuffer(mesh_package->vertex_buffer, offset, shader_attribute);
              return static_cast<int>(attribute.size());
          };

    auto const& optional_positions = (new_geom.positions ? new_geom.positions : old_geom.positions);
    if (optional_positions) {
        auto attribute_size = update_attribute(optional_positions, GeneralShader3d::Position{});
        offset += attribute_size * static_cast<int>(sizeof(float));
        mesh_package->vbo_count = attribute_size / 3;
    }

    auto const& optional_normals = (new_geom.normals ? new_geom.normals : old_geom.normals);
    if (optional_normals) {
        auto attribute_size = update_attribute(optional_normals, GeneralShader3d::Normal{});
        offset += attribute_size * static_cast<int>(sizeof(vec3));
    }

    auto const& optional_texture_coordinates
        = (new_geom.texture_coordinates ? new_geom.texture_coordinates : old_geom.texture_coordinates);

    if (optional_texture_coordinates) {
        auto attribute_size = update_attribute(optional_texture_coordinates, GeneralShader3d::TextureCoordinate{});
        offset += attribute_size * static_cast<int>(sizeof(float));
    }

    auto const& optional_vertex_colors = (new_geom.vertex_colors ? new_geom.vertex_colors : old_geom.vertex_colors);
    if (optional_vertex_colors) {
        update_attribute(optional_vertex_colors, GeneralShader3d::VertexColor{});
    }

    mesh_package->vertex_buffer.setData(buffer_data, Magnum::GL::BufferUsage::StaticDraw);
    mesh_package->mesh.setCount(mesh_package->vbo_count);
}

auto update_ibo(OpenglBackend::ObjectMeshPackage* mesh_package, std::vector<unsigned> const& indices) {
    if (!indices.empty()) {
        Corrade::Containers::Array<char> index_data;
        Magnum::MeshIndexType            index_type;
        Magnum::UnsignedInt              index_start, index_end;
        std::tie(index_data, index_type, index_start, index_end) = Magnum::MeshTools::compressIndices(indices);
        mesh_package->index_buffer.setData(index_data, Magnum::GL::BufferUsage::StaticDraw);

        mesh_package->ibo_count = static_cast<int>(indices.size());

        mesh_package->mesh.setCount(mesh_package->ibo_count)
            .setIndexBuffer(mesh_package->index_buffer, 0, index_type, index_start, index_end);
    }
}

} // namespace

OpenglBackend::ObjectMeshPackage::ObjectMeshPackage(Object3D*                            obj,
                                                    Magnum::SceneGraph::DrawableGroup3D* drawables,
                                                    GeneralShader3d&                     shader)
    : object(obj) {
    mesh.setCount(0).setPrimitive(to_magnum(default_geometry_format));
    drawable = new OpaqueDrawable(*object, drawables, mesh, shader);
}

OpenglBackend::OpenglBackend() {
    camera_object_.setParent(&scene_);
    camera_ = new Magnum::SceneGraph::Camera3D(camera_object_); // Memory control is handled elsewhere

    /* Setup renderer and shader defaults */
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);

    shader_.set_uniform_color({1.f, 0.5f, 0.1f});

    // Add root
    root_object_ = &scene_.addChild<Object3D>();
}

OpenglBackend::~OpenglBackend() = default;

auto OpenglBackend::render(vis::CameraPackage const& camera_package) -> void {
    camera_object_.setTransformation(camera_package.transformation);
    camera_->setProjectionMatrix(camera_package.camera->projectionMatrix());
    camera_->draw(drawables_);
}

auto OpenglBackend::after_add(SceneID const& item_id, SceneItems const& items) -> void {
    objects_.emplace(item_id, std::make_unique<ObjectMeshPackage>(&scene_.addChild<Object3D>(), &drawables_, shader_));

    auto const& item = items.at(item_id);
    before_update(item_id, item, items);
}

auto OpenglBackend::before_update(SceneID const& item_id, SceneItemInfo const& new_info, SceneItems const& items)
    -> void {

    ObjectMeshPackage& mesh_package = *objects_.at(item_id);
    auto const&        old_info     = items.at(item_id);

    if (new_info.geometry_info) {
        GeometryInfo const& new_geom = *new_info.geometry_info;
        GeometryInfo const& old_geom = *old_info.geometry_info;

        if (new_geom.positions || new_geom.normals || new_geom.texture_coordinates || new_geom.vertex_colors) {
            update_vbo(&mesh_package, new_geom, old_geom);
        }

        if (new_geom.indices) {
            update_ibo(&mesh_package, *new_geom.indices);
        }
    }

    if (new_info.display_info) {
        const DisplayInfo& display = *new_info.display_info;

        if (display.geometry_format) {
            mesh_package.mesh.setPrimitive(to_magnum(*display.geometry_format));
        }
    }

    // TODO: Handle parent and children updates

    if (new_info.parent) {
        auto const& parent = *new_info.parent;

        if (!util::has_key(objects_, parent)) {
            objects_.erase(item_id);
            throw std::invalid_argument("Parent id '" + to_string(parent) + "' not found in scene");
        }

        mesh_package.object->setParent(objects_.at(parent)->object);
    } else {
        mesh_package.object->setParent(root_object_);
    }

    if (new_info.display_info) {
        mesh_package.drawable->update_display_info(*new_info.display_info);
    }
}

auto OpenglBackend::before_delete(SceneID const& /*item_id*/, SceneItems const & /*items*/) -> void {}

auto OpenglBackend::reset_items(SceneItems const& items) -> void {
    // Remove all items from the scene
    if (root_object_) {
        scene_.children().erase(root_object_);
    }
    objects_.clear();

    // Add root
    root_object_ = &scene_.addChild<Object3D>();

    for (auto const& [item_id, item] : items) {
        util::ignore(item);
        objects_.emplace(item_id,
                         std::make_unique<ObjectMeshPackage>(&scene_.addChild<Object3D>(), &drawables_, shader_));
    }

    // Add new items to scene
    for (auto const& [item_id, item] : items) {
        before_update(item_id, item, items);
    }
}

auto OpenglBackend::resize(Magnum::Vector2i const & /*viewport*/) -> void {}

} // namespace gvs::scene::backends
