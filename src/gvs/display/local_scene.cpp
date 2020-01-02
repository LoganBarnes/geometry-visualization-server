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
#include "local_scene.hpp"

// project
#include "backends/opengl_backend.hpp"
#include "gvs/util/container_util.hpp"
#include "scene_core.hpp"
#include "scene_info_helpers.hpp"

namespace gvs::display {

LocalScene::LocalScene() {
    auto backend = std::make_unique<backends::OpenglBackend>();
    core_scene_  = std::make_unique<SceneCore>(*backend);
    display_     = std::move(backend);
}

LocalScene::~LocalScene() = default;

auto LocalScene::render(CameraPackage const& camera_package) const -> void {
    display_->render(camera_package);
}

auto LocalScene::resize(Magnum::Vector2i const& viewport) -> void {
    display_->resize(viewport);
}

auto LocalScene::set_seed(std::random_device::result_type seed) -> LocalScene& {
    core_scene_->set_seed(seed);
    return *this;
}

auto LocalScene::clear() -> void {
    core_scene_->clear();
}

auto LocalScene::actually_add_item(SparseSceneItemInfo&& info) -> util11::Result<SceneId> {
    auto result = core_scene_->add_item(std::forward<SparseSceneItemInfo>(info));
    if (!result) {
        return util11::Error{result.error().error_message()};
    }
    return result.value();
}

auto LocalScene::actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error {
    auto result = core_scene_->update_item(item_id, std::forward<SparseSceneItemInfo>(info));
    if (!result) {
        return util11::Error{result.error().error_message()};
    }
    return util11::success();
}

auto LocalScene::actually_append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error {
    auto result = core_scene_->append_to_item(item_id, std::forward<SparseSceneItemInfo>(info));
    if (!result) {
        return util11::Error{result.error().error_message()};
    }
    return util11::success();
}

auto LocalScene::items() const -> SceneItems const& {
    return core_scene_->items();
}

} // namespace gvs::display
