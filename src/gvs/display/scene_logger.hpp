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
#pragma once

// project
#include "gvs/display/geometry_logger.hpp"
#include "gvs/display/scene_info_sender.hpp"

// standard
#include <memory>

namespace gvs::display {

class SceneLogger : public log::GeometryLogger, public log::SceneInfoSender {
public:
    explicit SceneLogger();
    ~SceneLogger() override;

    auto render(gvs::vis::CameraPackage const& camera_package) const -> void;
    auto resize(Magnum::Vector2i const& viewport) -> void;

    // log::GeometryLogger
    auto item_stream() -> log::GeometryItemStream override;
    auto item_stream(const std::string& id) -> log::GeometryItemStream override;
    auto clear_all_items() -> void override;

    // log::SceneInfoSender
    auto update_scene(SceneID const& id, SceneItemInfo&& info, log::SendType type) -> util::Result<void> override;

private:
    std::unique_ptr<Scene> scene_;
};

} // namespace gvs::display
