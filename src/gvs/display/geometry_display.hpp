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
#pragma once

// project
#include "gvs/display/geometry_logger.hpp"
#include "gvs/util/blocking_queue.hpp"

// standard
#include <thread>

namespace gvs::display {

class GeometryDisplay : public log::GeometryLogger {
public:
    /// \brief Handles the server connection for geometry streams
    ///
    ///        If 'server_address' is an empty string, no connection attempt will be made.
    ///        Defaults to a maximum 4 second wait time when attempting to connect.
    ///
    /// \param server_address - the logger will attempt to connect to this address
    explicit GeometryDisplay();
    ~GeometryDisplay() override;

    auto item_stream() const -> log::GeometryItemStream override;
    auto item_stream(const std::string& id) const -> log::GeometryItemStream override;
    auto clear_all_items() -> void override;

private:
    std::unique_ptr<DisplayWindow> display_window_;
    std::thread display_thread_;

    util::BlockingQueue<SceneUpdateFunc> update_queue_; ///< Used to send scene updates to the main window
};

} // namespace gvs::display
