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
#include "gvs/scene/scene.hpp"
#include "scene_display.hpp"

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

    auto set_backend(std::unique_ptr<display::backends::BackendInterface> backend) -> LocalScene& override;
    /*
     * End `SceneDisplay` functions
     */

    /*
     * Start `Scene` functions
     */
    auto set_seed(std::random_device::result_type seed) -> LocalScene& override;
    auto clear() -> void override;

private:
    auto actually_add_item(SceneItemInfo&& info) -> SceneID override;
    auto actually_update_item(SceneID const& item_id, SceneItemInfo&& info) -> void override;
    auto actually_append_to_item(SceneID const& item_id, SceneItemInfo&& info) -> void override;

    auto items() const -> SceneItems const& override;
    /*
     * End `Scene` functions
     */

    std::mt19937                                generator_; ///< Used to generate SceneIDs
    SceneItems                                  items_; ///< The map of all the items in the scene
    std::unique_ptr<backends::BackendInterface> backend_; ///< Used to do the actual rendering of the scene
};

} // namespace gvs::display
