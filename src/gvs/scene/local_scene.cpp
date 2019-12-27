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

namespace gvs::scene {

LocalScene::LocalScene() : generator_(std::random_device{}()), backend_(std::make_unique<backends::OpenglBackend>()) {
    {
        SceneItemInfo root;
        set_defaults_on_empty_fields(&root);
        items_.emplace(nil_id, std::move(root));
    }
    backend_->reset_items(items_);
}

LocalScene::~LocalScene() = default;

auto LocalScene::render(vis::CameraPackage const& camera_package) const -> void {
    backend_->render(camera_package);
}

auto LocalScene::resize(Magnum::Vector2i const& viewport) -> void {
    backend_->resize(viewport);
}

auto LocalScene::set_backend(std::unique_ptr<backends::BackendInterface> backend) -> LocalScene& {
    backend_ = std::move(backend);
    return *this;
}

auto LocalScene::set_seed(std::random_device::result_type seed) -> LocalScene& {
    generator_ = std::mt19937(seed);
    return *this;
}

auto LocalScene::clear() -> void {
    items_.clear();
    {
        SceneItemInfo root;
        set_defaults_on_empty_fields(&root);
        items_.emplace(nil_id, std::move(root));
    }
    backend_->reset_items(items_);
}

auto LocalScene::actually_add_item(SceneItemInfo&& info) -> util::Result<SceneID> {
    auto item_id = uuids::uuid_random_generator{generator_}();
    auto parent = info.parent;
    items_.emplace(item_id, std::move(info));

    if (parent) {
        items_.at(parent.value()).children->emplace_back(item_id);
    }

    backend_->after_add(item_id, items_);
    return item_id;
}

auto LocalScene::actually_update_item(SceneID const& /*item_id*/, SceneItemInfo && /*info*/) -> util::Result<void> {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto LocalScene::actually_append_to_item(SceneID const& /*item_id*/, SceneItemInfo && /*info*/) -> util::Result<void> {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto LocalScene::items() const -> SceneItems const& {
    return items_;
}

} // namespace gvs::scene
