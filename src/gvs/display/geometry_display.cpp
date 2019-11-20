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

// third party
#include <crossguid/guid.hpp>

namespace gvs::display {

GeometryDisplay::GeometryDisplay() {
    //    display_thread_ = std::thread([this] {
    display_window_ = std::make_unique<DisplayWindow>(update_queue_);
    display_window_->exec();
    display_window_ = nullptr;
    //    });
}

GeometryDisplay::~GeometryDisplay() {
    //    display_thread_.join();
}

auto GeometryDisplay::item_stream() const -> log::GeometryItemStream {
    return item_stream(xg::newGuid().str());
}

auto GeometryDisplay::item_stream(const std::string& id) const -> log::GeometryItemStream {
    return log::GeometryItemStream(id);
}

auto GeometryDisplay::clear_all_items() -> void {}

} // namespace gvs::display
