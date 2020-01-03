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
#include "backends/empty_backend.hpp"
#include "backends/opengl_backend.hpp"
#include "gvs/util/container_util.hpp"
#include "scene_core.hpp"
#include "scene_info_helpers.hpp"

// standard
#include <algorithm>

namespace gvs::display {

LocalScene::LocalScene(BackendType backend_type) {
    switch (backend_type) {

    case BackendType::Empty: {
        auto backend = std::make_unique<backends::EmptyBackend>();
        core_scene_  = std::make_unique<SceneCore>(*backend);
        display_     = std::move(backend);
        break;
    }

    case BackendType::OpenGL: {
        auto backend = std::make_unique<backends::OpenglBackend>();
        core_scene_  = std::make_unique<SceneCore>(*backend);
        display_     = std::move(backend);
        break;
    }

    } // end switch
}

LocalScene::~LocalScene() = default;

auto LocalScene::render(CameraPackage const& camera_package) const -> void {
    display_->render(camera_package);
}

auto LocalScene::resize(Magnum::Vector2i const& viewport) -> void {
    display_->resize(viewport);
}

auto LocalScene::item_ids() const -> std::unordered_set<SceneId> {
    return core_scene_->item_ids();
}

auto LocalScene::clear() -> LocalScene& {
    core_scene_->clear();
    return *this;
}

auto LocalScene::set_seed(unsigned seed) -> LocalScene& {
    core_scene_->set_seed(seed);
    return *this;
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

auto LocalScene::actually_get_item_info(const gvs::SceneId& item_id, gvs::scene::InfoGetterFunc info_getter) const
    -> util11::Error {
    if (core_scene_->items().find(item_id) == core_scene_->items().end()) {
        return util11::Error{"Item '" + gvs::to_string(item_id) + "' does not exist in the scene"};
    }
    info_getter(core_scene_->items().at(item_id));
    return util11::success();
}

} // namespace gvs::display
