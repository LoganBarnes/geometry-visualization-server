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
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
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

} // namespace

OpenglBackend::ObjectMeshPackage::ObjectMeshPackage(SceneId                              id,
                                                    Object3D*                            obj,
                                                    Magnum::SceneGraph::DrawableGroup3D* drawables,
                                                    unsigned                             id_for_intersect,
                                                    GeneralShader3d&                     shader)
    : scene_id(id), intersect_id(id_for_intersect), object(obj) {
    mesh.setCount(0).setPrimitive(to_magnum(default_geometry_format));
    drawable = new OpaqueDrawable(*object, drawables, mesh, intersect_id, shader);
}

OpenglBackend::OpenglBackend() : framebuffer_(Magnum::GL::defaultFramebuffer.viewport()) {
    using namespace Magnum;

    camera_object_.setParent(&scene_);
    camera_ = new Magnum::SceneGraph::Camera3D(camera_object_); // Memory control is handled elsewhere

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    shader_.set_uniform_color({1.f, 0.5f, 0.1f});

    /*
     * Set up the framebuffer for intersection tests
     */
    id_rbo_.setStorage(GL::RenderbufferFormat::R32UI, GL::defaultFramebuffer.viewport().size());
    depth_rbo_.setStorage(GL::RenderbufferFormat::DepthComponent24, GL::defaultFramebuffer.viewport().size());

    framebuffer_.attachRenderbuffer(GL::Framebuffer::ColorAttachment{1}, id_rbo_)
        .attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, depth_rbo_)
        .mapForDraw({{GeneralShader3d::IdOutput, GL::Framebuffer::ColorAttachment{1}}});

    CORRADE_INTERNAL_ASSERT(framebuffer_.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);
}

OpenglBackend::~OpenglBackend() = default;

auto OpenglBackend::render(CameraPackage const& camera_package) const -> void {
    using namespace Magnum;

    camera_object_.setTransformation(camera_package.object.transformation());
    camera_->setProjectionMatrix(camera_package.camera->projectionMatrix());

    if (!opaque_drawables_.isEmpty()) {
        camera_->draw(opaque_drawables_);
    }

    if (!wireframe_drawables_.isEmpty()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        camera_->draw(wireframe_drawables_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (!transparent_drawables_.isEmpty()) {
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        camera_->draw(transparent_drawables_);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    }

    // Don't draw the "non_visible_drawables_" obvs.

    // Draw to custom framebuffer
    framebuffer_.clearColor(GeneralShader3d::ColorOutput, Color3{0.125f})
        .clearColor(GeneralShader3d::IdOutput, Vector4ui{})
        .clearDepth(1.0f)
        .bind();

    if (!opaque_drawables_.isEmpty()) {
        camera_->draw(opaque_drawables_);
    }

    // Bind the main buffer back
    GL::defaultFramebuffer.bind();
}

auto OpenglBackend::resize(Magnum::Vector2i const& viewport) -> void {
    framebuffer_.setViewport({{0u, 0u}, viewport});
}

auto OpenglBackend::added(SceneId const& item_id, SceneItemInfo const& item) -> void {
    if (item_id == gvs::nil_id()) {
        add_package(item_id, &scene_.addChild<Object3D>(), nullptr, shader_);
    } else {
        add_package(item_id, &get_package(item.parent).object->addChild<Object3D>(), nullptr, shader_);
    }
    updated(item_id, scene::UpdatedInfo::everything(), item);
}

auto OpenglBackend::updated(SceneId const& item_id, scene::UpdatedInfo const& updated, SceneItemInfo const& item)
    -> void {
    ObjectMeshPackage& mesh_package = *id_to_pkgs_.at(item_id);

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

        if (!util::has_key(id_to_pkgs_, parent)) {

            throw std::invalid_argument("Parent id '" + to_string(parent) + "' not found in scene");
        }

        mesh_package.object->setParent(id_to_pkgs_.at(parent)->object);
    }

    if (updated.display) {
        mesh_package.drawable->update_display_info(item.display_info);
        mesh_package.visible = item.display_info.visible;

        if (item.display_info.wireframe_only) {
            mesh_package.drawable_group_when_visible = &wireframe_drawables_;

        } else if (item.display_info.opacity < 1.f) {
            mesh_package.drawable_group_when_visible = &transparent_drawables_;

        } else {
            mesh_package.drawable_group_when_visible = &opaque_drawables_;
        }

        auto const& parent_mesh_package = *id_to_pkgs_.at(item.parent);
        auto const  parent_visible      = (parent_mesh_package.drawable->group() != &non_visible_drawables_);

        update_drawable_group(&mesh_package, parent_visible);
    }
}

auto OpenglBackend::removed(SceneId const & /*item_id*/) -> void {}

auto OpenglBackend::reset_items(SceneItems const& items) -> void {
    // Remove all items from the scene
    if (util::has_key(id_to_pkgs_, gvs::nil_id())) {
        scene_.children().erase(get_package(gvs::nil_id()).object);
    }
    id_to_pkgs_.clear();
    obj_to_pkgs_.clear();
    packages_.clear();

    while (id_to_pkgs_.size() < items.size()) {
        for (auto const& [item_id, item] : items) {
            if (util::has_key(id_to_pkgs_, item_id)) {
                continue;
            }

            if (util::has_key(id_to_pkgs_, item.parent) || item_id == gvs::nil_id()) {
                added(item_id, item);
            }
        }
    }
}

auto OpenglBackend::add_package(SceneId                              id,
                                Object3D*                            obj,
                                Magnum::SceneGraph::DrawableGroup3D* drawables,
                                GeneralShader3d&                     shader) -> void {
    packages_.emplace_back(id, obj, drawables, next_intersect_id_++, shader);
    auto iter = std::prev(packages_.end());
    id_to_pkgs_.emplace(iter->scene_id, iter);
    obj_to_pkgs_.emplace(iter->object, iter);
    intersect_id_to_scene_id_.emplace(iter->intersect_id, iter->scene_id);
}

auto OpenglBackend::remove_package(gvs::SceneId const& item_id) -> void {
    auto iter = id_to_pkgs_.at(item_id);
    id_to_pkgs_.erase(item_id);
    obj_to_pkgs_.erase(iter->object);
    packages_.erase(iter);
}

auto OpenglBackend::remove_package(Object3D const* obj) -> void {
    auto iter = obj_to_pkgs_.at(obj);
    obj_to_pkgs_.erase(obj);
    id_to_pkgs_.erase(iter->scene_id);
    packages_.erase(iter);
}

auto OpenglBackend::get_package(gvs::SceneId const& id) -> ObjectMeshPackage& {
    return *id_to_pkgs_.at(id);
}

auto OpenglBackend::get_package(gvs::SceneId const& id) const -> ObjectMeshPackage const& {
    return *id_to_pkgs_.at(id);
}

auto OpenglBackend::update_drawable_group(ObjectMeshPackage* mesh_package, bool parent_visible) -> void {

    auto update_children = [this, &mesh_package](bool parent_visiblity) {
        for (auto const& child : mesh_package->object->children()) {
            auto& child_package = *obj_to_pkgs_.at(&child);
            update_drawable_group(&child_package, parent_visiblity);
        }
    };

    auto currently_visible = (mesh_package->drawable->group() != &non_visible_drawables_);

    if (!parent_visible) {
        if (currently_visible) {
            non_visible_drawables_.add(*mesh_package->drawable);
            update_children(false);
        }
        return;
    }

    // Currently visible but shouldn't be. Hide item and all children.
    if (currently_visible && !mesh_package->visible) {
        non_visible_drawables_.add(*mesh_package->drawable);
        update_children(false);
        return;
    }

    if (mesh_package->visible) {
        mesh_package->drawable_group_when_visible->add(*mesh_package->drawable);
        update_children(true);
    }
}

} // namespace gvs::display::backends
