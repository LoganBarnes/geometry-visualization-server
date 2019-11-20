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
#include "gvs/vis-client/scene/camera_package.hpp"

// generated
#include <types.pb.h>

// external
#include <Magnum/Magnum.h>
#include <Magnum/Math/Vector3.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Object.h>
#include <imgui.h>

namespace gvs::vis {

struct SceneInitializationInfo {
    Magnum::Vector3 background_color;
    Magnum::Vector2i viewport_size;
};

class SceneInterface {
public:
    virtual ~SceneInterface() = 0;

    virtual void update(const Magnum::Vector2i& viewport) = 0;
    virtual void render(const CameraPackage& camera_package) = 0;
    virtual void configure_gui(const Magnum::Vector2i& viewport) = 0;

    virtual void add_item(const proto::SceneItemInfo& info) = 0;
    virtual void update_item(const proto::SceneItemInfo& info) = 0;
    virtual void reset(const proto::SceneItems& items) = 0;

    virtual void resize(const Magnum::Vector2i& viewport) = 0;

    virtual std::size_t items() const = 0;
    virtual bool empty() const = 0;
};

inline SceneInterface::~SceneInterface() = default;

SceneInitializationInfo make_scene_init_info(const ImColor& background_color, const Magnum::Vector2i& viewport);

} // namespace gvs::vis
