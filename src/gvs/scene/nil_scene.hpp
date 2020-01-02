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
#include "gvs/scene/scene.hpp"

namespace gvs {
namespace scene {

/// \brief An implementation of Scene that does nothing.
///
///        Calls to add_item will return a nil SceneId and no items will ever be added or updated.
///
///        This is useful as a placeholder class when you cannot connect to a server. Replacing a
///        net::ClientScene instance will a NilScene instance will allow your code to remain the same
///        but it will prevent errors from being thrown when the client cannot connect.
class NilScene : public Scene {
public:
    explicit NilScene();
    ~NilScene() override;

    /*
     * Start `Scene` functions
     */
    auto clear() -> NilScene& override;
    auto set_seed(unsigned seed) -> NilScene& override;

private:
    auto actually_add_item(SparseSceneItemInfo&& info) -> util11::Result<SceneId> override;
    auto actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error override;
    auto actually_append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error override;

    auto items() const -> SceneItems const& override;
    /*
     * End `Scene` functions
     */

    SceneItems empty_item_list_;
};

} // namespace scene
} // namespace gvs
