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
#include "gvs/util/result_11.hpp"
#include "types.hpp"

// standard
#include <random>

namespace gvs {
namespace scene {

class Scene {
public:
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
    auto add_item(Functors&&... functors) -> SceneId;

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
    auto update_item(SceneId const& item_id, Functors&&... functors) -> void;

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
    auto append_to_item(SceneId const& item_id, Functors&&... functors) -> void;

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
    auto get_item_info(SceneId const& item_id, Functors&&... functors) -> void;

    template <typename... Functors>
    auto safe_add_item(Functors&&... functors) -> util11::Result<SceneId>;

    template <typename... Functors>
    auto safe_update_item(SceneId const& item_id, Functors&&... functors) -> util11::Error;

    template <typename... Functors>
    auto safe_append_to_item(SceneId const& item_id, Functors&&... functors) -> util11::Error;

    template <typename... Functors>
    auto safe_get_item_info(SceneId const& item_id, Functors&&... functors) -> util11::Error;

    auto size() const -> std::size_t;
    auto empty() const -> bool;

    auto begin() const -> SceneItems::const_iterator;
    auto end() const -> SceneItems::const_iterator;

    virtual auto clear() -> void                                          = 0;
    virtual auto set_seed(std::random_device::result_type seed) -> Scene& = 0;

private:
    /// \brief Adds the new item to the scene
    virtual auto actually_add_item(SparseSceneItemInfo&& info) -> util11::Result<SceneId> = 0;

    /// \brief Updates the specified item by replacing existing fields with the new ones
    virtual auto actually_update_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error = 0;

    /// \brief Updates the specified item by appending all new geometry
    virtual auto actually_append_to_item(SceneId const& item_id, SparseSceneItemInfo&& info) -> util11::Error = 0;

    /// \brief The map of all items in the scene
    virtual auto items() const -> SceneItems const& = 0;
};

namespace detail {

template <typename Functor>
auto apply_functor(SparseSceneItemInfo* info, std::string* error_strings, Functor&& functor) -> void {
    auto error_string = functor(info);
    if (!error_string.empty()) {
        *error_strings += error_string + '\n';
    }
}

template <typename... Functors>
auto get_scene_info(Functors&&... functors) -> SparseSceneItemInfo {
    SparseSceneItemInfo info;
    std::string         error_strings;
    // iterate over all functors and apply them to a new SceneItemInfo
    int dummy[] = {(apply_functor(&info, &error_strings, std::forward<Functors>(functors)), 0)...};
    (void)dummy;

    if (!error_strings.empty()) {
        throw std::runtime_error(error_strings);
    }

    return info;
}

} // namespace detail

template <typename... Functors>
auto Scene::add_item(Functors&&... functors) -> SceneId {
    return safe_add_item(std::forward<Functors>(functors)...).value_or_throw();
}

template <typename... Functors>
auto Scene::update_item(SceneId const& item_id, Functors&&... functors) -> void {
    safe_update_item(item_id, std::forward<Functors>(functors)...).throw_if_error();
}

template <typename... Functors>
auto Scene::append_to_item(SceneId const& item_id, Functors&&... functors) -> void {
    safe_append_to_item(item_id, std::forward<Functors>(functors)...).throw_if_error();
}

template <typename... Functors>
auto Scene::get_item_info(SceneId const& item_id, Functors&&... functors) -> void {
    safe_get_item_info(item_id, std::forward<Functors>(functors)...).throw_if_error();
}

template <typename... Functors>
auto Scene::safe_add_item(Functors&&... functors) -> util11::Result<SceneId> {
    auto info = detail::get_scene_info(std::forward<Functors>(functors)...);
    return actually_add_item(std::move(info));
}

template <typename... Functors>
auto Scene::safe_update_item(SceneId const& item_id, Functors&&... functors) -> util11::Error {
    auto info = detail::get_scene_info(std::forward<Functors>(functors)...);
    return actually_update_item(item_id, std::move(info));
}

template <typename... Functors>
auto Scene::safe_append_to_item(SceneId const& item_id, Functors&&... functors) -> util11::Error {
    auto info = detail::get_scene_info(std::forward<Functors>(functors)...);
    return actually_append_to_item(item_id, std::move(info));
}

template <typename... Functors>
auto Scene::safe_get_item_info(SceneId const& item_id, Functors&&... functors) -> util11::Error {
    auto const& item    = items().at(item_id);
    int         dummy[] = {(functors(item), 0)...};
    (void)dummy;

    return util11::success();
}

} // namespace scene
} // namespace gvs
