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
#include "gvs/scene/forward_declarations.hpp"
#include "gvs/scene/types.hpp"
#include "gvs/util/result.hpp"

// standard
#include <unordered_set>

namespace gvs::display {

class SceneCore {
public:
    explicit SceneCore(scene::SceneUpdateHandler& update_handler);
    ~SceneCore();

    /// \brief Adds the new item to the scene
    auto add_item(SparseSceneItemInfo&& new_info) -> util::Result<SceneId>;

    /// \brief Updates the specified item by replacing existing fields with the new ones
    auto update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util::Result<void>;

    /// \brief Updates the specified item by appending all new geometry
    auto append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util::Result<void>;

    /// \brief Sets the seed used to generate SceneIds
    auto set_seed(std::random_device::result_type seed) -> SceneCore&;

    /// \brief Clears all items in the scene
    auto clear() -> SceneCore&;

    /// \brief The map of all items in the scene
    auto items() const -> SceneItems const&;

    /// \brief The ids of all items in the scene
    auto item_ids() const -> std::unordered_set<SceneId>;

private:
    scene::SceneUpdateHandler& update_handler_; ///< Handles scene updates in an implementation specific way
    std::mt19937               generator_; ///< Used to generate SceneIDs
    SceneItems                 items_; ///< The map of all the items in the scene
};

} // namespace gvs::display
