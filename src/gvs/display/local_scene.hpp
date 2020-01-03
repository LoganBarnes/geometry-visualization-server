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
#include "forward_declarations.hpp"
#include "gvs/display/backends/display_backend.hpp"
#include "gvs/scene/scene.hpp"

namespace gvs::display {

class LocalScene : public scene::Scene, public SceneDisplay {
public:
    explicit LocalScene();
    ~LocalScene() override;

    /*
     * Start `SceneDisplay` functions
     */
    auto render(CameraPackage const& camera_package) const -> void override;
    auto resize(Magnum::Vector2i const& viewport) -> void override;
    /*
     * End `SceneDisplay` functions
     */

    /*
     * Start `Scene` functions
     */
    [[nodiscard]] auto item_ids() const -> std::unordered_set<SceneId> override;
    auto               clear() -> LocalScene& override;
    auto               set_seed(unsigned seed) -> LocalScene& override;

private:
    auto actually_add_item(SparseSceneItemInfo&& info) -> util11::Result<SceneId> override;
    auto actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error override;
    auto actually_append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error override;

    auto actually_get_item_info(SceneId const& item_id, scene::InfoGetterFunc info_getter) const
        -> util11::Error override;
    /*
     * End `Scene` functions
     */

    std::unique_ptr<SceneDisplay> display_; ///< Used to do the actual rendering of the scene
    std::unique_ptr<SceneCore>    core_scene_; ///< Handles all the scene logic
};

} // namespace gvs::display
