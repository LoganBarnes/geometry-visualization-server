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
#pragma once

#include "gvs/vis-client/scene/general_shader_3d.hpp"
#include "gvs/vis-client/scene/scene_interface.hpp"

#include <gvs/types.pb.h>

#include <Corrade/Containers/Array.h>
#include <Corrade/Containers/Optional.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/FeatureGroup.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/SceneGraph/SceneGraph.h>

namespace gvs {
namespace vis {

class OpenGLScene : public SceneInterface {
public:
    explicit OpenGLScene(const SceneInitializationInfo& initialization_info);
    ~OpenGLScene() override = default;

    void update(const Magnum::Vector2i& viewport) override;
    void render() override;
    void configure_gui(const Magnum::Vector2i& viewport) override;

    void add_item(const proto::SceneItemInfo& info) override;
    void reset(const proto::SceneItems& items) override;

    void resize(const Magnum::Vector2i& viewport) override;

    Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>& camera_object() override;
    Magnum::SceneGraph::Camera3D& camera() override;

private:
    using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
    using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;

    mutable GeneralShader3D shader_;
    std::vector<std::unique_ptr<Magnum::GL::Mesh>> meshes_;

    Scene3D scene_;
    Object3D* root_object_;
    Object3D camera_object_;
    Magnum::SceneGraph::Camera3D* camera_;
    Magnum::SceneGraph::DrawableGroup3D drawables_;
};

} // namespace vis
} // namespace gvs
