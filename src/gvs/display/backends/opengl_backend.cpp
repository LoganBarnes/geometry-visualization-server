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
#include "gvs/display/magnum_conversions.hpp"
#include "gvs/util/container_util.hpp"
#include "gvs/util/result.hpp"

// external
#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Mesh.h>
#include <Magnum/MeshTools/CompressIndices.h>

// standard
#include <iostream>

namespace gvs::display::backends {
namespace {

auto update_vbo(OpenglBackend::ObjectMeshPackage* mesh_package, GeometryInfo const& geometry_info) -> void {

    std::vector<float> buffer_data;
    GLintptr           offset = 0;
    mesh_package->vbo_count   = 0;

    auto update_attribute
        = [&mesh_package, &offset, &buffer_data](auto const& attribute, auto const& shader_attribute) {
              buffer_data.insert(buffer_data.end(), attribute.data(), (attribute.data() + attribute.size()));
              mesh_package->mesh.addVertexBuffer(mesh_package->vertex_buffer, offset, shader_attribute);

              auto isize = static_cast<int>(attribute.size());
              offset += isize * static_cast<int>(sizeof(float));
              return isize;
          };

    if (!geometry_info.positions.empty()) {
        auto attribute_size     = update_attribute(geometry_info.positions, GeneralShader3d::Position{});
        mesh_package->vbo_count = attribute_size / 3;
    }

    if (!geometry_info.normals.empty()) {
        update_attribute(geometry_info.normals, GeneralShader3d::Normal{});
    }

    if (!geometry_info.texture_coordinates.empty()) {
        update_attribute(geometry_info.texture_coordinates, GeneralShader3d::TextureCoordinate{});
    }

    if (!geometry_info.vertex_colors.empty()) {
        update_attribute(geometry_info.vertex_colors, GeneralShader3d::VertexColor{});
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

using DrawableGroupUpdateFunc
    = Magnum::SceneGraph::DrawableGroup3D& (Magnum::SceneGraph::DrawableGroup3D::*)(Magnum::SceneGraph::Drawable3D&);

auto do_stuff(Magnum::SceneGraph::DrawableGroup3D* group,
              DrawableGroupUpdateFunc              func,
              OpenglBackend::ObjectMeshPackage&    obj) {
    (group->*func)(*obj.drawable);
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
}

OpenglBackend::~OpenglBackend() = default;

auto OpenglBackend::render(CameraPackage const& camera_package) const -> void {
    camera_object_.setTransformation(camera_package.transformation);
    camera_->setProjectionMatrix(camera_package.camera->projectionMatrix());
    camera_->draw(drawables_);
}

auto OpenglBackend::resize(Magnum::Vector2i const & /*viewport*/) -> void {}

auto OpenglBackend::added(SceneId const& item_id, SceneItemInfo const& item) -> void {
    if (item_id == gvs::nil_id()) {
        objects_.emplace(item_id, std::make_unique<ObjectMeshPackage>(&scene_.addChild<Object3D>(), nullptr, shader_));
    } else {
        objects_.emplace(item_id,
                         std::make_unique<ObjectMeshPackage>(&get_package(item.parent).object->addChild<Object3D>(),
                                                             &drawables_,
                                                             shader_));
    }
    updated(item_id, scene::UpdatedInfo::everything(), item);
}

auto OpenglBackend::updated(SceneId const& item_id, scene::UpdatedInfo const& updated, SceneItemInfo const& item)
    -> void {
    ObjectMeshPackage& mesh_package = *objects_.at(item_id);

    if (updated.geometry) {
        auto const& geometry_info = item.geometry_info;

        if (updated.geometry_vertices) {
            update_vbo(&mesh_package, geometry_info);
        }

        if (updated.geometry_indices) {
            update_ibo(&mesh_package, geometry_info.indices);
        }
    }

    if (updated.display_geometry_format) {
        mesh_package.mesh.setPrimitive(to_magnum(item.display_info.geometry_format));
    }

    // TODO: Handle parent and children updates properly

    if (updated.parent) {
        auto const& parent = item.parent;

        if (!util::has_key(objects_, parent)) {
            objects_.erase(item_id);
            throw std::invalid_argument("Parent id '" + to_string(parent) + "' not found in scene");
        }

        mesh_package.object->setParent(objects_.at(parent)->object);
    }

    if (updated.display) {
        mesh_package.drawable->update_display_info(item.display_info);

        if (updated.display_visible) {
            if (item.display_info.visible) {
                do_stuff(&drawables_, &Magnum::SceneGraph::DrawableGroup3D::add, mesh_package);

            } else {
                do_stuff(&drawables_, &Magnum::SceneGraph::DrawableGroup3D::remove, mesh_package);
            }
        }
    }
}

auto OpenglBackend::removed(SceneId const & /*item_id*/) -> void {}

auto OpenglBackend::reset_items(SceneItems const& items) -> void {
    // Remove all items from the scene
    if (util::has_key(objects_, gvs::nil_id())) {
        scene_.children().erase(get_package(gvs::nil_id()).object);
    }
    objects_.clear();

    while (objects_.size() < items.size()) {
        for (auto const& [item_id, item] : items) {
            if (util::has_key(objects_, item_id)) {
                continue;
            }

            if (util::has_key(objects_, item.parent) || item_id == gvs::nil_id()) {
                added(item_id, item);
            }
        }
    }
}

auto OpenglBackend::get_package(const gvs::SceneId& id) -> ObjectMeshPackage& {
    return *objects_.at(id);
}

auto OpenglBackend::get_package(const gvs::SceneId& id) const -> ObjectMeshPackage const& {
    return *objects_.at(id);
}

} // namespace gvs::display::backends
