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
#include "display_scene.hpp"

// project
#include "display_window.hpp"

namespace gvs::display {

DisplayScene::DisplayScene() {

    util::BlockingQueue<int> tmp_queue;

    display_thread_ = std::thread([this, &tmp_queue] {
        display_window_ = std::make_unique<DisplayWindow>(*this);
        tmp_queue.emplace_back(42);
        display_window_->exec();
        display_window_ = nullptr;
    });

    tmp_queue.pop_front();
    core_scene_ = std::make_unique<util::AtomicData<SceneCore>>(*this);
}

DisplayScene::~DisplayScene() {
    block_until_window_is_closed();
}

auto DisplayScene::block_until_window_is_closed() -> void {
    if (display_thread_.joinable()) {
        display_thread_.join();
    }
}

auto DisplayScene::item_ids() const -> std::unordered_set<SceneId> {
    return core_scene_->use_safely([](auto const& core_scene) { return core_scene.item_ids(); });
}

auto DisplayScene::clear() -> DisplayScene& {
    core_scene_->use_safely([](auto& core_scene) mutable { core_scene.clear(); });
    return *this;
}

auto DisplayScene::set_seed(unsigned seed) -> DisplayScene& {
    core_scene_->use_safely([&seed](auto& core_scene) mutable { core_scene.set_seed(seed); });
    return *this;
}

auto DisplayScene::actually_add_item(SparseSceneItemInfo&& info) -> util11::Result<SceneId> {
    return core_scene_->use_safely(
        [info = std::forward<SparseSceneItemInfo>(info)](auto& core_scene) mutable -> util11::Result<SceneId> {
            auto result = core_scene.add_item(std::forward<SparseSceneItemInfo>(info));
            if (!result) {
                return util11::Error{result.error().error_message()};
            }
            return result.value();
        });
}

auto DisplayScene::actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error {
    return core_scene_->use_safely([item_id, info = std::forward<SparseSceneItemInfo>(info)](auto& core_scene) mutable {
        auto result = core_scene.update_item(item_id, std::forward<SparseSceneItemInfo>(info));
        if (!result) {
            return util11::Error{result.error().error_message()};
        }
        return util11::success();
    });
}

auto DisplayScene::actually_append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error {
    return core_scene_->use_safely([item_id, info = std::forward<SparseSceneItemInfo>(info)](auto& core_scene) mutable {
        auto result = core_scene.append_to_item(item_id, std::forward<SparseSceneItemInfo>(info));
        if (!result) {
            return util11::Error{result.error().error_message()};
        }
        return util11::success();
    });
}

auto DisplayScene::actually_get_item_info(SceneId const& item_id, scene::InfoGetterFunc info_getter) const
    -> util11::Error {
    return core_scene_->use_safely([item_id, info_getter = std::move(info_getter)](auto const& core_scene) {
        if (core_scene.items().find(item_id) == core_scene.items().end()) {
            return util11::Error{"Item '" + gvs::to_string(item_id) + "' does not exist in the scene"};
        }
        info_getter(core_scene.items().at(item_id));
        return util11::success();
    });
}

auto DisplayScene::added(SceneId const& item_id, SceneItemInfo const& item) -> void {
    display_window_->thread_safe_update(
        [item_id, item](scene::SceneUpdateHandler* handler) { handler->added(item_id, item); });
}

auto DisplayScene::updated(SceneId const& item_id, scene::UpdatedInfo const& updated, SceneItemInfo const& item)
    -> void {
    display_window_->thread_safe_update(
        [item_id, updated, item](scene::SceneUpdateHandler* handler) { handler->updated(item_id, updated, item); });
}

auto DisplayScene::removed(SceneId const& item_id) -> void {
    display_window_->thread_safe_update([item_id](scene::SceneUpdateHandler* handler) { handler->removed(item_id); });
}

auto DisplayScene::reset_items(SceneItems const& items) -> void {
    display_window_->thread_safe_update([items](scene::SceneUpdateHandler* handler) { handler->reset_items(items); });
}

} // namespace gvs::display
