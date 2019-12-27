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
#include "gvs/display/geometry_item_stream.hpp"

// external
#include <uuid.h>

// standard
#include <random>

namespace gvs::scene {

class Scene {
public:
    explicit Scene(std::random_device::result_type seed = std::random_device{}());
    virtual ~Scene();

    /// \brief Modifies and sends the contents of a stream, updating the server if the stream id does not exist
    ///
    ///     ```cpp
    ///     // Create the scene
    ///     std::unique_ptr<gvs::scene::Scene> scene = /*Some scene type*/;
    ///
    ///     // modify and send stream contents to the server
    ///     scene->add_item(gvs::SetPositions3d(my_points), gvs::SetLineStrip(), gvs::ReadableID("My Points"));
    ///     ```
    template <typename... Functors>
    auto add_item(Functors&&... functors) -> util::Result<uuid::uuid>;

    auto item_stream(const std::string& id) -> GeometryItemStream = 0;
    auto clear_all_items() -> void = 0;

private:
};

inline GeometryLogger::~GeometryLogger() = default;

} // namespace gvs::scene
