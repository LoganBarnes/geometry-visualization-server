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
#include "scene.hpp"

// project
#include "gvs/display/backends/opengl_backend.hpp"

namespace gvs::display {

Scene::Scene(std::unique_ptr<backends::BackendInterface> backend) : backend_(std::move(backend)) {
    if (!backend_) {
        backend_ = std::make_unique<backends::OpenglBackend>();
    }
}

Scene::~Scene() = default;

auto Scene::update() -> void {}

auto Scene::render(vis::CameraPackage const& camera_package) -> void {
    backend_->render(camera_package);
}

auto Scene::add_item(SceneID const& item_id, SceneItemInfo&& item) -> void {
    items_[item_id] = std::move(item);
    backend_->after_add(item_id, items_);
}

auto Scene::update_item(SceneID const& item_id, SceneItemInfo&& item) -> void {
    backend_->before_update(item_id, item, items_);
    items_[item_id] = std::move(item);
}

auto Scene::remove_item(SceneID const& item_id) -> void {
    backend_->before_delete(item_id, items_);
    items_.erase(item_id);
}

auto Scene::clear() -> void {
    items_.clear();
}

auto Scene::resize(Magnum::Vector2i const& viewport) -> void {
    backend_->resize(viewport);
}

auto Scene::items() const -> SceneItems const& {
    return items_;
}

auto Scene::size() const -> std::size_t {
    return items_.size();
}

auto Scene::empty() const -> bool {
    return items_.empty();
}

} // namespace gvs::display
