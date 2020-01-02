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
#include "scene_info_helpers.hpp"

namespace gvs::display {

LocalScene::LocalScene() : generator_(std::random_device{}()), backend_(std::make_unique<backends::OpenglBackend>()) {
    {
        SceneItemInfo root;
        items_.emplace(nil_id(), std::move(root));
    }
    backend_->reset_items(items_);
}

LocalScene::~LocalScene() = default;

auto LocalScene::render(CameraPackage const& camera_package) const -> void {
    backend_->render(camera_package);
}

auto LocalScene::resize(Magnum::Vector2i const& viewport) -> void {
    backend_->resize(viewport);
}

//auto LocalScene::set_backend(std::unique_ptr<backends::BackendInterface> backend) -> LocalScene& {
//    backend_ = std::move(backend);
//    return *this;
//}

auto LocalScene::set_seed(std::random_device::result_type seed) -> LocalScene& {
    generator_ = std::mt19937(seed);
    return *this;
}

auto LocalScene::clear() -> void {
    items_.clear();
    {
        SceneItemInfo root;
        items_.emplace(nil_id(), std::move(root));
    }
    backend_->reset_items(items_);
}

auto LocalScene::actually_add_item(SparseSceneItemInfo&& new_info) -> util11::Result<SceneId> {
    auto item_id = generate_scene_id(generator_);

    SceneItemInfo info;

    auto result = replace_if_present(&info, std::move(new_info));
    if (!result) {
        return util11::Error{result.error().error_message()};
    }

    items_.at(info.parent).children.emplace_back(item_id);

    items_.emplace(item_id, std::move(info));
    backend_->added(item_id, items_.at(item_id));
    return item_id;
}

auto LocalScene::actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error {
    scene::UpdatedInfo updated{info};

    auto& item = items_.at(item_id);

    if (info.parent && *info.parent != item.parent) {
        // Remove item from the current parent's list of children
        util::remove_all_by_value(items_.at(item.parent).children, item_id);

        // Add the item to the new parent's list of children
        items_.at(*info.parent).children.emplace_back(item_id);
    }

    auto result = replace_if_present(&item, std::move(info));
    if (!result) {
        return {result.error().error_message()};
    }

    backend_->updated(item_id, updated, items_.at(item_id));

    return util11::success();
}

auto LocalScene::actually_append_to_item(SceneId const& /*item_id*/, SparseSceneItemInfo && /*info*/) -> util11::Error {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto LocalScene::items() const -> SceneItems const& {
    return items_;
}

} // namespace gvs::display
