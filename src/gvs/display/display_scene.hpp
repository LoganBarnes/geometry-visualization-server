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
#include "display_window.hpp"
#include "gvs/scene/scene.hpp"
#include "gvs/scene/scene_update_handler.hpp"
#include "gvs/util/atomic_data.hpp"
#include "scene_core.hpp"

// standard
#include <mutex>
#include <thread>

namespace gvs::display {

class DisplayScene : public scene::Scene, public scene::SceneUpdateHandler {
public:
    explicit DisplayScene();
    ~DisplayScene() override;

    auto block_until_window_is_closed() -> void;

    /*
     * Start `Scene` functions
     */
    [[nodiscard]] auto item_ids() const -> std::unordered_set<SceneId> override;

    auto clear() -> DisplayScene& override;
    auto set_seed(unsigned seed) -> DisplayScene& override;

    auto actually_add_item(SparseSceneItemInfo&& info) -> util11::Result<SceneId> override;
    auto actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error override;
    auto actually_append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error override;

    auto actually_get_item_info(SceneId const& item_id, scene::InfoGetterFunc info_getter) const
        -> util11::Error override;
    /*
     * End `Scene` functions
     */

private:
    /*
     * Start `SceneUpdater` functions
     */
    void added(SceneId const& item_id, SceneItemInfo const& item) override;
    void updated(SceneId const& item_id, scene::UpdatedInfo const& updated, SceneItemInfo const& item) override;
    void removed(SceneId const& item_id) override;
    void reset_items(SceneItems const& items) override;
    /*
     * End `SceneUpdater` functions
     */

    std::unique_ptr<gvs::display::DisplayWindow> display_window_; ///< Used to display the scene in a window
    std::unique_ptr<util::AtomicData<SceneCore>> core_scene_; ///< Handles all the scene logic

    std::thread display_thread_; ///< Runs the display window
};

} // namespace gvs::display
