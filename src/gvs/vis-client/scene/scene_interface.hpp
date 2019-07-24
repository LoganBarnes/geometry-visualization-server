// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2019 CloudNC Ltd - All Rights Reserved                        |  |
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

// gvs
#include "gvs/vis-client/scene/camera_package.hpp"

// generated
#include <types.pb.h>

// external
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Object.h>
#include <imgui.h>

namespace gvs::vis {

struct SceneInitializationInfo {
    Magnum::Vector3 background_color;
    Magnum::Vector2i viewport_size;
};

class SceneInterface {
public:
    virtual ~SceneInterface() = 0;

    virtual void update(const Magnum::Vector2i& viewport) = 0;
    virtual void render(const CameraPackage& camera_package) = 0;
    virtual void configure_gui(const Magnum::Vector2i& viewport) = 0;

    virtual void add_item(const proto::SceneItemInfo& info) = 0;
    virtual void update_item(const proto::SceneItemInfo& info) = 0;
    virtual void reset(const proto::SceneItems& items) = 0;

    virtual void resize(const Magnum::Vector2i& viewport) = 0;
};

inline SceneInterface::~SceneInterface() = default;

SceneInitializationInfo make_scene_init_info(const ImColor& background_color, const Magnum::Vector2i& viewport);

} // namespace gvs::vis
