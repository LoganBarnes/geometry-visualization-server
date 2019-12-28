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

// project
#include "gvs/scene/types.hpp"
#include "gvs/vis-client/scene/camera_package.hpp"

// external
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/SceneGraph/Camera.h>

namespace gvs::display::backends {

class BackendInterface {
public:
    virtual ~BackendInterface() = 0;

    /**
     * @brief Renders all the visible items in the scene.
     */
    virtual auto render(vis::CameraPackage const& camera_package) -> void = 0;

    /**
     * @brief Called when a Scene has added a new item.
     * @param item_id - The id of the added item.
     * @param items - All items in the scene (including the added item).
     */
    virtual auto after_add(SceneID const& item_id, SceneItems const& items) -> void = 0;

    /**
     * @brief Called when a Scene is about to update an item.
     * @param item_id - The id of the updated item.
     * @param items - All items in the scene (including the item before it was updated).
     */
    virtual auto before_update(SceneID const& item_id, SceneItemInfo const& changes, SceneItems const& items) -> void
        = 0;

    /**
     * @brief Called when a scene is about to remove an item.
     * @param item_id - The id of the item to be deleted.
     * @param items - All items in the scene (including the item to be deleted).
     */
    virtual auto before_delete(SceneID const& item_id, SceneItems const& items) -> void = 0;

    /**
     * @brief Called when a Scene has replaced all its items.
     * @param items - All the new items in the scene.
     */
    virtual auto reset_items(SceneItems const& items) -> void = 0;

    /**
     * @brief Called when a Scene's viewport has changed.
     * @param viewport - The new viewport dimensions.
     */
    virtual auto resize(Magnum::Vector2i const& viewport) -> void = 0;
};

inline BackendInterface::~BackendInterface() = default;

} // namespace gvs::display::backends
