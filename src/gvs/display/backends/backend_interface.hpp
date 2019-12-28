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
#include "gvs/display/camera_package.hpp"
#include "gvs/scene/types.hpp"

// external
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/SceneGraph/Camera.h>

namespace gvs::display::backends {

struct UpdatedInfo {
    bool geometry                = false;
    bool geometry_vertices       = false;
    bool geometry_indices        = false;
    bool display                 = false;
    bool display_geometry_format = false;
    bool parent                  = false;

    explicit UpdatedInfo(SceneItemInfoSetter const& info) {
        if (info.geometry_info) {
            auto const& geom = info.geometry_info;

            geometry          = true;
            geometry_vertices = (geom->positions || geom->normals || geom->texture_coordinates || geom->vertex_colors);
            geometry_indices  = (!!geom->indices);
        }

        if (info.display_info) {
            display                 = true;
            display_geometry_format = (!!info.display_info->geometry_format);
        }

        parent = (!!info.parent);
    }

    static auto everything() -> UpdatedInfo {
        UpdatedInfo info;
        info.geometry                = true;
        info.geometry_vertices       = true;
        info.geometry_indices        = true;
        info.display                 = true;
        info.display_geometry_format = true;
        info.parent                  = true;
        return info;
    }

private:
    UpdatedInfo() = default;
};

class BackendInterface {
public:
    virtual ~BackendInterface() = 0;

    /**
     * @brief Renders all the visible items in the scene.
     */
    virtual auto render(CameraPackage const& camera_package) -> void = 0;

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
    virtual auto after_update(SceneID const& item_id, UpdatedInfo const& updated, SceneItems const& items) -> void = 0;

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
