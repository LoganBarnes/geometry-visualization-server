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

// project
#include "scene_update_handler.hpp"

namespace gvs {
namespace scene {

SceneCore::SceneCore(SceneUpdateHandler& update_handler)
    : update_handler_(update_handler), generator_(std::random_device{}()) {
    clear();
}

SceneCore::~SceneCore() = default;

auto SceneCore::add_item(SceneItemInfoSetter && /*info*/) -> util11::Result<SceneId> {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto SceneCore::update_item(SceneId const& /*item_id*/, SceneItemInfoSetter && /*info*/) -> util11::Error {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto SceneCore::append_to_item(SceneId const& /*item_id*/, SceneItemInfoSetter && /*info*/) -> util11::Error {
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

} // namespace scene
} // namespace gvs
