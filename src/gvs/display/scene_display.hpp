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
#include "backends/backend_interface.hpp"

// standard
#include <memory>

namespace gvs::display {

class SceneDisplay {
public:
    virtual ~SceneDisplay() = 0;

    /// \brief Renders all the visible items in the scene.
    virtual auto render(vis::CameraPackage const& camera_package) const -> void = 0;

    /// \brief Called when a scene's viewport has changed.
    /// \param viewport - The new viewport dimensions.
    virtual auto resize(Magnum::Vector2i const& viewport) -> void = 0;

    /// \brief Sets the backend used to display the rendering of the items
    virtual auto set_backend(std::unique_ptr<display::backends::BackendInterface> backend) -> SceneDisplay& = 0;
};

inline SceneDisplay::~SceneDisplay() = default;

} // namespace gvs::display
