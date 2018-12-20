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

namespace gvs {
namespace vis {

class SceneInterface {
public:
    virtual ~SceneInterface() = 0;

    virtual void update(const Magnum::Vector2i& viewport) = 0;
    virtual void render(const Magnum::Vector2i& viewport) = 0;
    virtual void configure_gui(const Magnum::Vector2i& viewport) = 0;

    virtual void add_item(const proto::SceneItemInfo& info) = 0;
    virtual void reset(const proto::SceneItems& items) = 0;

    virtual void resize(const Magnum::Vector2i& viewport) = 0;
};

inline SceneInterface::~SceneInterface() = default;

} // namespace vis
} // namespace gvs
