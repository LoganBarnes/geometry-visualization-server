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
#pragma once

// gvs
#include "display_backend.hpp"
#include "drawables.hpp"
#include "general_shader_3d.hpp"

// external
#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/FeatureGroup.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

// standard
#include <list>
#include <memory>

namespace gvs::display::backends {

class OpenglBackend : public DisplayBackend {
public:
    explicit OpenglBackend();
    ~OpenglBackend() override;

    auto render(CameraPackage const& camera_package) const -> void override;
    auto resize(Magnum::Vector2i const& viewport) -> void override;

    auto added(SceneId const& item_id, SceneItemInfo const& item) -> void override;
    auto updated(SceneId const& item_id, scene::UpdatedInfo const& updated, SceneItemInfo const& item) -> void override;
    auto removed(SceneId const& item_id) -> void override;

    auto reset_items(SceneItems const& items) -> void override;

    using Scene3D  = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
    using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;

    struct ObjectMeshPackage {
        SceneId  scene_id     = gvs::nil_id();
        unsigned intersect_id = 0u;

        Magnum::GL::Buffer vertex_buffer;
        int                vbo_count = 0;

        Magnum::GL::Buffer index_buffer;
        int                ibo_count = 0;

        Magnum::GL::Mesh mesh;
        Object3D*        object   = nullptr;
        OpaqueDrawable*  drawable = nullptr;

        Magnum::SceneGraph::DrawableGroup3D* drawable_group_when_visible = nullptr;
        bool                                 visible                     = true;

        explicit ObjectMeshPackage(SceneId                              id,
                                   Object3D*                            obj,
                                   Magnum::SceneGraph::DrawableGroup3D* drawable_group,
                                   unsigned                             id_for_intersect,
                                   GeneralShader3d&                     shader);
    };

private:
    GeneralShader3d shader_;

    using ObjectList = std::list<ObjectMeshPackage>;

    ObjectList                                                packages_; // TODO: make items deletable
    std::unordered_map<SceneId, ObjectList::iterator>         id_to_pkgs_;
    std::unordered_map<Object3D const*, ObjectList::iterator> obj_to_pkgs_;

    std::unordered_map<unsigned, SceneId> intersect_id_to_scene_id_;
    unsigned                              next_intersect_id_ = 1u;

    Scene3D scene_;

    mutable Object3D                      camera_object_;
    mutable Magnum::SceneGraph::Camera3D* camera_;

    mutable Magnum::SceneGraph::DrawableGroup3D opaque_drawables_;
    mutable Magnum::SceneGraph::DrawableGroup3D wireframe_drawables_;
    mutable Magnum::SceneGraph::DrawableGroup3D transparent_drawables_;
    mutable Magnum::SceneGraph::DrawableGroup3D non_visible_drawables_;

    mutable Magnum::GL::Framebuffer framebuffer_;
    Magnum::GL::Renderbuffer        id_rbo_;
    Magnum::GL::Renderbuffer        depth_rbo_;

    auto add_package(SceneId id, Object3D* obj, Magnum::SceneGraph::DrawableGroup3D* drawables, GeneralShader3d& shader)
        -> void;

    auto remove_package(SceneId const& item_id) -> void;
    auto remove_package(Object3D const* obj) -> void;

    auto get_package(SceneId const& id) -> ObjectMeshPackage&;
    auto get_package(SceneId const& id) const -> ObjectMeshPackage const&;

    auto update_drawable_group(ObjectMeshPackage* mesh_package, bool parent_visible) -> void;
};

} // namespace gvs::display::backends
