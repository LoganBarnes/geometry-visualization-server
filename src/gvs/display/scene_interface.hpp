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
#include "gvs/display/types.hpp"
#include "gvs/vis-client/scene/camera_package.hpp"

// external
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Object.h>
#include <imgui.h>

namespace gvs::display {

class SceneInterface {
public:
    virtual ~SceneInterface() = 0;

    virtual auto update(Magnum::Vector2i const& viewport) -> void = 0;
    virtual auto render(vis::CameraPackage const& camera_package) -> void = 0;

    virtual auto add_item(SceneItemInfo const& info) -> void = 0;
    virtual auto update_item(SceneItemInfo const& info) -> void = 0;
    virtual auto reset(SceneItems const& items) -> void = 0;

    virtual auto resize(Magnum::Vector2i const& viewport) -> void = 0;

    virtual auto root() const -> SceneItemInfo const& = 0;

    virtual auto size() const -> std::size_t = 0;
    virtual auto empty() const -> bool = 0;
};

inline SceneInterface::~SceneInterface() = default;

} // namespace gvs::display
