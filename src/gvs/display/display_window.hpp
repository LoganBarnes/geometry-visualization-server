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

#include "gvs/util/blocking_queue.hpp"
#include "gvs/vis-client/app/imgui_magnum_application.hpp"

// standard
#include <gvs/util/atomic_data.hpp>
#include <thread>

namespace gvs::display {

class DisplayWindow : public vis::ImGuiMagnumApplication {
public:
    explicit DisplayWindow(util::BlockingQueue<SceneUpdateFunc>& update_queue);
    ~DisplayWindow() override;

private:
    void update() override;
    void render(const vis::CameraPackage& camera_package) const override;
    void configure_gui() override;

    void resize(const Magnum::Vector2i& viewport) override;

    // General Info
    std::string gl_version_str_;
    std::string gl_renderer_str_;
    std::string error_message_;

    // Debugging
    bool run_as_fast_as_possible_ = false;

    // Scene
    std::unique_ptr<vis::SceneInterface> scene_; // forward declaration

    util::BlockingQueue<SceneUpdateFunc>& external_update_queue_;
    std::thread update_thread_;

    util::BlockingQueue<SceneUpdateFunc> internal_update_queue_;
};

} // namespace gvs::display
