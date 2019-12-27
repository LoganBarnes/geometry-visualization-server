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
#include "scene_logger.hpp"

// project
#include "gvs/display/geometry_item_stream.hpp"
#include "gvs/display/scene.hpp"

// external
#include <uuid.h>

namespace gvs::display {

SceneLogger::SceneLogger() : scene_(std::make_unique<Scene>()) {}

SceneLogger::~SceneLogger() = default;

auto SceneLogger::render(const gvs::vis::CameraPackage& camera_package) const -> void {
    scene_->render(camera_package);
}

auto SceneLogger::resize(const Magnum::Vector2i& viewport) -> void {
    scene_->resize(viewport);
}

auto SceneLogger::item_stream() -> log::GeometryItemStream {
    return item_stream(uuids::to_string(uuids::uuid_system_generator{}()));
}

auto SceneLogger::item_stream(const std::string& id) -> log::GeometryItemStream {
    return log::GeometryItemStream(id, *this);
}

auto SceneLogger::clear_all_items() -> void {}

auto SceneLogger::update_scene(SceneID const& id, SceneItemInfo&& info, log::SendType type) -> util::Result<void> {

    auto result = util::success();

    switch (type) {
    case log::SendType::Safe:
    case log::SendType::Replace:
        result = scene_->add_item(id, std::move(info));
        break;

    case log::SendType::Append:
        break;
    }

    return result;
}

} // namespace gvs::display
