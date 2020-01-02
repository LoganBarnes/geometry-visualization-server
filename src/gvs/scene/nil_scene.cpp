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
#include "nil_scene.hpp"

namespace gvs {
namespace scene {

NilScene::NilScene()  = default;
NilScene::~NilScene() = default;

auto NilScene::clear() -> NilScene& {
    return *this;
}

auto NilScene::set_seed(unsigned /*seed*/) -> NilScene& {
    return *this;
}

auto NilScene::actually_add_item(gvs::SparseSceneItemInfo && /*info*/) -> util11::Result<gvs::SceneId> {
    return gvs::nil_id();
}

auto NilScene::actually_update_item(gvs::SceneId const& /*item_id*/, gvs::SparseSceneItemInfo && /*info*/)
    -> util11::Error {
    return util11::success();
}

auto NilScene::actually_append_to_item(gvs::SceneId const& /*item_id*/, gvs::SparseSceneItemInfo && /*info*/)
    -> util11::Error {
    return util11::success();
}

auto NilScene::items() const -> gvs::SceneItems const& {
    return empty_item_list_;
}

} // namespace scene
} // namespace gvs
