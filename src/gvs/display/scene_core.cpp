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
#include "scene_core.hpp"
#include <gvs/util/container_util.hpp>

// project
#include "gvs/scene/scene_update_handler.hpp"
#include "scene_info_helpers.hpp"

// standard
#include <algorithm>
#include <iterator>

namespace gvs::display {

SceneCore::SceneCore(scene::SceneUpdateHandler& update_handler)
    : update_handler_(update_handler), generator_(std::random_device{}()) {
    clear();
}

SceneCore::~SceneCore() = default;

auto SceneCore::add_item(SparseSceneItemInfo&& new_info) -> util::Result<SceneId> {
    auto item_id = generate_scene_id(generator_);

    SceneItemInfo info;

    auto result = replace_if_present(&info, std::move(new_info));
    if (!result) {
        return tl::make_unexpected(result.error());
    }

    items_.at(info.parent).children.emplace_back(item_id);
    update_handler_.updated(info.parent, scene::UpdatedInfo::children_only(), items_.at(info.parent));

    items_.emplace(item_id, std::move(info));
    update_handler_.added(item_id, items_.at(item_id));
    return item_id;
}

auto SceneCore::update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util::Result<void> {
    auto& item = items_.at(item_id);

    if (info.parent && *info.parent != item.parent) {
        // Remove item from the current parent's list of children
        util::remove_all_by_value(items_.at(item.parent).children, item_id);
        update_handler_.updated(item.parent, scene::UpdatedInfo::children_only(), items_.at(item.parent));

        auto const& parent = *info.parent;

        // Add the item to the new parent's list of children
        items_.at(parent).children.emplace_back(item_id);
        update_handler_.updated(parent, scene::UpdatedInfo::children_only(), items_.at(parent));
    }

    auto const& const_info = info;
    auto        updated    = scene::UpdatedInfo(const_info); // info isn't changed here

    auto result = replace_if_present(&item, std::move(info));
    if (!result) {
        return result;
    }

    update_handler_.updated(item_id, updated, items_.at(item_id));

    return util::success();
}

auto SceneCore::append_to_item(SceneId const& /*item_id*/, SparseSceneItemInfo && /*info*/) -> util::Result<void> {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto SceneCore::set_seed(std::random_device::result_type seed) -> SceneCore& {
    generator_ = std::mt19937(seed);
    return *this;
}

auto SceneCore::clear() -> SceneCore& {
    items_.clear();
    {
        SceneItemInfo root;
        items_.emplace(nil_id(), std::move(root));
    }
    update_handler_.reset_items(items_);
    return *this;
}

auto SceneCore::items() const -> SceneItems const& {
    return items_;
}

auto SceneCore::item_ids() const -> std::unordered_set<SceneId> {
    std::unordered_set<SceneId> ids;
    std::transform(items_.begin(), items_.end(), std::inserter(ids, ids.end()), [](auto const& id_and_item) {
        return id_and_item.first;
    });
    return ids;
}

} // namespace gvs::display
