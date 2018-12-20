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
#include <Magnum/Shaders/Phong.h>

namespace gvs {
namespace vis {

class OpenGLScene : public SceneInterface {
public:
    explicit OpenGLScene(const Magnum::Vector2i& viewport);
    ~OpenGLScene() override = default;

    void update(const Magnum::Vector2i& viewport) override;
    void render(const Magnum::Vector2i& viewport) override;
    void configure_gui(const Magnum::Vector2i& viewport) override;

    void add_item(const proto::SceneItemInfo& info) override;
    void reset(const proto::SceneItems& items) override;

    void resize(const Magnum::Vector2i& viewport) override;

private:
    using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
    using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;

    Magnum::Shaders::Phong colored_shader_;
    Magnum::Shaders::Phong shader_;
    Corrade::Containers::Array<Corrade::Containers::Optional<Magnum::GL::Mesh>> meshes_;

    // initial test_mesh
    Magnum::GL::Buffer index_buffer_, vertex_buffer_;
    Magnum::GL::Mesh mesh_;

    Scene3D scene_;
    Object3D root_object_, camera_object_;
    Magnum::SceneGraph::Camera3D* camera_;
    Magnum::SceneGraph::DrawableGroup3D drawables_;
    Magnum::Vector3 previous_position_;
};

} // namespace vis
} // namespace gvs
