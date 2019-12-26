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

#include "gvs/display/types.hpp"

// standard
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace gvs {
namespace detail {

/*
 * The following two classes are intended to only be used as rvalue references inside the
 * 'StaticScene::add_item', 'StaticScene::update_item', and 'StaticScene::get_item_info` functions.
 * All the move and copy constructors are deleted so the only way to use these classes is by
 * creating an instance as part of the function call:
 *
 * scene.add_item(loop::SetPositions(positions), loop::SetReadableID("Sphere"));
 *
 * where loop::SetPositions and loop::SetReadableID are the "named parameter" classes.
 *
 * The SceneGetter and SceneGetter classes are kept generic since they do the same thing for
 * many different types. To create a new named parameter we add a new class that derives from
 * SceneGetter or SceneGetter.
 *
 * The Curiously Recurring Template Pattern (CRTP) is used to avoid any indirection performance
 * costs typically associated with base classes. Because of this, each derived class will have
 * to implement a 'move_data' or 'update_data' function for the SceneGetter and SceneGetter classes
 * respectively.
 */

///
/// \brief A "named parameter" wrapper used to set items in a StaticScene
///
template <typename T, std::optional<T> SceneItemInfo::*member>
struct SceneGetter {
    explicit SceneGetter(std::optional<T>* value) : data_(value) {}

    auto operator()(SceneItemInfo const& info) -> void { *data_ = info.*member; }

    SceneGetter(const SceneGetter&) = delete;
    SceneGetter(SceneGetter&&) noexcept = delete;
    SceneGetter& operator=(const SceneGetter&) = delete;
    SceneGetter& operator=(SceneGetter&&) noexcept = delete;

private:
    std::optional<T>* data_;
};

///
/// \brief A "named parameter" wrapper used to set geometry info for items in a StaticScene
///
template <typename T, std::optional<T> GeometryInfo::*member>
struct SceneGeometryGetter {
    explicit SceneGeometryGetter(std::optional<T>* value) : data_(value) {}

    auto operator()(SceneItemInfo const& info) -> void {
        if (!info.geometry_info) {
            *data_ = std::nullopt;
            return;
        }

        *data_ = info.geometry_info->*member;
    }

    SceneGeometryGetter(const SceneGeometryGetter&) = delete;
    SceneGeometryGetter(SceneGeometryGetter&&) noexcept = delete;
    SceneGeometryGetter& operator=(const SceneGeometryGetter&) = delete;
    SceneGeometryGetter& operator=(SceneGeometryGetter&&) noexcept = delete;

private:
    std::optional<T>* data_;
};

///
/// \brief A "named parameter" wrapper used to set display info for items in a StaticScene
///
template <typename T, std::optional<T> DisplayInfo::*member>
struct SceneDisplayGetter {
    explicit SceneDisplayGetter(std::optional<T>* value) : data_(value) {}

    auto operator()(SceneItemInfo const& info) -> void {
        if (!info.display_info) {
            *data_ = std::nullopt;
            return;
        }

        *data_ = info.display_info->*member;
    }

    SceneDisplayGetter(const SceneDisplayGetter&) = delete;
    SceneDisplayGetter(SceneDisplayGetter&&) noexcept = delete;
    SceneDisplayGetter& operator=(const SceneDisplayGetter&) = delete;
    SceneDisplayGetter& operator=(SceneDisplayGetter&&) noexcept = delete;

protected:
    std::optional<T>* data_;
};

} // namespace detail

/*
 * Getters
 */
using GetPositions3d = detail::SceneGeometryGetter<AttributeVector<3>, &GeometryInfo::positions>;
using GetNormals3d = detail::SceneGeometryGetter<AttributeVector<3>, &GeometryInfo::normals>;
using GetTextureCoordinates3d = detail::SceneGeometryGetter<AttributeVector<2>, &GeometryInfo::texture_coordinates>;
using GetVertexColors3d = detail::SceneGeometryGetter<AttributeVector<3>, &GeometryInfo::vertex_colors>;
using GetIndices = detail::SceneGeometryGetter<std::vector<unsigned>, &GeometryInfo::indices>;

using GetParent = detail::SceneGetter<SceneID, &SceneItemInfo::parent>;
using GetChildren = detail::SceneGetter<std::vector<SceneID>, &SceneItemInfo::children>;

using GetReadableId = detail::SceneDisplayGetter<std::string, &DisplayInfo::readable_id>;
using GetGeometryFormat = detail::SceneDisplayGetter<GeometryFormat, &DisplayInfo::geometry_format>;
using GetTransformation = detail::SceneDisplayGetter<mat4, &DisplayInfo::transformation>;
using GetUniformColor = detail::SceneDisplayGetter<vec3, &DisplayInfo::uniform_color>;
using GetColoring = detail::SceneDisplayGetter<Coloring, &DisplayInfo::coloring>;
using GetShading = detail::SceneDisplayGetter<Shading, &DisplayInfo::shading>;
using IsVisible = detail::SceneDisplayGetter<bool, &DisplayInfo::visible>;
using GetOpacity = detail::SceneDisplayGetter<float, &DisplayInfo::opacity>;

} // namespace gvs
