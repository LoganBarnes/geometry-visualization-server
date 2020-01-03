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
#include "types.hpp"

namespace gvs {
namespace scene {

struct UpdatedInfo {
    bool geometry                = false;
    bool geometry_vertices       = false;
    bool geometry_indices        = false;
    bool display                 = false;
    bool display_geometry_format = false;
    bool display_visible         = false;
    bool parent                  = false;
    bool children                = false;

    explicit UpdatedInfo(SparseSceneItemInfo const& info);

    static auto everything() -> UpdatedInfo;
    static auto children_only() -> UpdatedInfo;

private:
    UpdatedInfo() = default;
};

class SceneUpdateHandler {
public:
    virtual ~SceneUpdateHandler() = 0;

    /**
     * @brief Called when a Scene has added a new item.
     * @param item_id - The id of the added item.
     * @param item - The data of the newly added item.
     */
    virtual auto added(SceneId const& item_id, SceneItemInfo const& item) -> void = 0;

    /**
     * @brief Called when a Scene has updated an item.
     * @param item_id - The id of the updated item.
     * @param updated - The fields that were updated.
     * @param item - The data of the newly updated item.
     */
    virtual auto updated(SceneId const& item_id, UpdatedInfo const& updated, SceneItemInfo const& item) -> void = 0;

    /**
     * @brief Called when a Scene has removed an item.
     * @param item_id - The id of the item to be deleted.
     * @param items - All items in the scene (including the item to be deleted).
     */
    virtual auto removed(SceneId const& item_id) -> void = 0;

    /**
     * @brief Called when a Scene has replaced all its items.
     * @param items - All the new items in the scene.
     */
    virtual auto reset_items(SceneItems const& items) -> void = 0;
};

} // namespace scene
} // namespace gvs
