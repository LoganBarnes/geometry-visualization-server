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

// gvs
#include "backends/backend_interface.hpp"
#include "log_params.hpp"

// standard
#include <memory>

namespace gvs::display {

class Scene {
public:
    explicit Scene(std::unique_ptr<backends::BackendInterface> backend = nullptr);
    ~Scene();

    auto update() -> void;
    auto render(vis::CameraPackage const& camera_package) -> void;

    auto add_item(SceneID const& item_id, SceneItemInfo&& item) -> void;
    auto update_item(SceneID const& item_id, SceneItemInfo&& item) -> void;
    auto remove_item(SceneID const& item_id) -> void;

    auto clear() -> void;

    auto resize(Magnum::Vector2i const& viewport) -> void;

    [[nodiscard]] auto items() const -> SceneItems const&;

    [[nodiscard]] auto size() const -> std::size_t;
    [[nodiscard]] auto empty() const -> bool;

private:
    std::unique_ptr<backends::BackendInterface> backend_;

    SceneItems items_;
};

} // namespace gvs::display
