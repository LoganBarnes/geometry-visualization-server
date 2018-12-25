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

#include <gvs/types.pb.h>

#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Object.h>
#include <imgui.h>

namespace gvs {
namespace vis {

struct SceneInitializationInfo {
    Magnum::Vector3 background_color;
    Magnum::Vector2i viewport_size;
};

class SceneInterface {
public:
    virtual ~SceneInterface() = 0;

    virtual void update(const Magnum::Vector2i& viewport) = 0;
    virtual void render(const Magnum::Matrix4& camera_transformation, Magnum::SceneGraph::Camera3D* camera) = 0;
    virtual void configure_gui(const Magnum::Vector2i& viewport) = 0;

    virtual void add_item(const proto::SceneItemInfo& info) = 0;
    virtual void reset(const proto::SceneItems& items) = 0;

    virtual void resize(const Magnum::Vector2i& viewport) = 0;
};

inline SceneInterface::~SceneInterface() = default;

SceneInitializationInfo make_scene_init_info(const ImColor& background_color, const Magnum::Vector2i& viewport);

} // namespace vis
} // namespace gvs
