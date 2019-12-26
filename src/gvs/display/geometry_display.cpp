// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2018 Logan Barnes - All Rights Reserved
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
#include "geometry_display.hpp"

// project
#include "gvs/display/display_window.hpp"
#include "gvs/display/geometry_item_stream.hpp"
#include "gvs/display/scene.hpp"

// third party
#include <crossguid/guid.hpp>

namespace gvs::display {

GeometryDisplay::GeometryDisplay() {
    display_thread_ = std::thread([this] {
        display_window_ = std::make_unique<DisplayWindow>(update_queue_);
        display_window_->exec();
        display_window_ = nullptr;
    });
}

GeometryDisplay::~GeometryDisplay() {
    display_thread_.join();
}

auto GeometryDisplay::item_stream() -> log::GeometryItemStream {
    return item_stream(xg::newGuid().str());
}

auto GeometryDisplay::item_stream(const std::string& id) -> log::GeometryItemStream {
    return log::GeometryItemStream(id, *this);
}

auto GeometryDisplay::clear_all_items() -> void {}

auto GeometryDisplay::update_scene(SceneID const& id, SceneItemInfo&& info, log::SendType type) -> util::Result<void> {
    switch (type) {

    case log::SendType::Safe:
    case log::SendType::Replace:
        update_queue_.emplace_back(
            [id, info = std::move(info)](Scene* scene) mutable { scene->add_item(id, std::move(info)); });
        break;

    case log::SendType::Append:
        break;
    }

    return util::success();
}

} // namespace gvs::display
