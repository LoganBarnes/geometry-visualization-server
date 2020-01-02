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
#include "client_scene.hpp"

namespace gvs {
namespace net {

ClientScene::ClientScene(std::string const& server_address) : ClientScene(server_address, std::chrono::seconds(5)) {}

ClientScene::~ClientScene() = default;

auto ClientScene::set_seed(std::random_device::result_type seed) -> ClientScene& {
    generator_ = std::mt19937(seed);
    return *this;
}

auto ClientScene::clear() -> void {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto ClientScene::actually_add_item(SceneItemInfoSetter && /*info*/) -> util11::Result<gvs::SceneId> {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto ClientScene::actually_update_item(gvs::SceneId const& /*item_id*/, SceneItemInfoSetter && /*info*/)
    -> util11::Error {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto ClientScene::actually_append_to_item(gvs::SceneId const& /*item_id*/, SceneItemInfoSetter && /*info*/)
    -> util11::Error {
    throw std::runtime_error(__FUNCTION__ + std::string(" not yet implemented"));
}

auto ClientScene::items() const -> gvs::SceneItems const& {
    return items_;
}

} // namespace net
} // namespace gvs
