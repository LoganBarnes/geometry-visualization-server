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

#include "settable_types.hpp"

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
 * The SceneChecker and SceneChecker classes are kept generic since they do the same thing for
 * many different types. To create a new named parameter we add a new class that derives from
 * SceneChecker or SceneChecker.
 *
 * The Curiously Recurring Template Pattern (CRTP) is used to avoid any indirection performance
 * costs typically associated with base classes. Because of this, each derived class will have
 * to implement a 'move_data' or 'update_data' function for the SceneChecker and SceneChecker classes
 * respectively.
 */

///
/// \brief A "named parameter" wrapper used to set items in a StaticScene
///
template <typename T, std::optional<T> SceneItemInfo::*member>
struct SceneChecker {
    explicit SceneChecker(bool* value) : data_(value) {}

    auto operator()(SceneItemInfo const& info) -> void { *data_ = (!!(info.*member) || (info.*member)->empty()); }

    SceneChecker(const SceneChecker&) = delete;
    SceneChecker(SceneChecker&&) noexcept = delete;
    SceneChecker& operator=(const SceneChecker&) = delete;
    SceneChecker& operator=(SceneChecker&&) noexcept = delete;

private:
    bool* data_;
};

///
/// \brief A "named parameter" wrapper used to set geometry info for items in a StaticScene
///
template <typename T, std::optional<T> GeometryInfo::*member>
struct SceneGeometryChecker {
    explicit SceneGeometryChecker(bool* value) : data_(value) {}

    auto operator()(SceneItemInfo const& info) -> void {
        if (!info.geometry_info) {
            *data_ = false;
            return;
        }
        auto const& geometry_info = info.geometry_info.value();
        *data_ = (!!(geometry_info.*member) || (geometry_info.*member)->empty());
    }

    SceneGeometryChecker(const SceneGeometryChecker&) = delete;
    SceneGeometryChecker(SceneGeometryChecker&&) noexcept = delete;
    SceneGeometryChecker& operator=(const SceneGeometryChecker&) = delete;
    SceneGeometryChecker& operator=(SceneGeometryChecker&&) noexcept = delete;

private:
    bool* data_;
};

///
/// \brief A "named parameter" wrapper used to set display info for items in a StaticScene
///
template <typename T, std::optional<T> DisplayInfo::*member>
struct SceneDisplayChecker {
    explicit SceneDisplayChecker(bool* value) : data_(value) {}

    auto operator()(SceneItemInfo const& info) -> void {
        if (!info.display_info) {
            *data_ = false;
            return;
        }

        *data_ = (!!(info.*member));
    }

    SceneDisplayChecker(const SceneDisplayChecker&) = delete;
    SceneDisplayChecker(SceneDisplayChecker&&) noexcept = delete;
    SceneDisplayChecker& operator=(const SceneDisplayChecker&) = delete;
    SceneDisplayChecker& operator=(SceneDisplayChecker&&) noexcept = delete;

protected:
    bool* data_;
};

} // namespace detail

/*
 * Checkers
 */
using HasGeometry = detail::SceneGeometryChecker<AttributeVector<3>, &GeometryInfo::positions>;
using HasPositions3d = detail::SceneGeometryChecker<AttributeVector<3>, &GeometryInfo::positions>;
using HasNormals3d = detail::SceneGeometryChecker<AttributeVector<3>, &GeometryInfo::normals>;
using HasTextureCoordinates3d = detail::SceneGeometryChecker<AttributeVector<2>, &GeometryInfo::texture_coordinates>;
using HasVertexColors3d = detail::SceneGeometryChecker<AttributeVector<3>, &GeometryInfo::vertex_colors>;
using HasIndices = detail::SceneGeometryChecker<std::vector<unsigned>, &GeometryInfo::indices>;

using HasChildren = detail::SceneChecker<std::vector<SceneID>, &SceneItemInfo::children>;

using HasReadableId = detail::SceneDisplayChecker<std::string, &DisplayInfo::readable_id>;
using HasGeometryFormat = detail::SceneDisplayChecker<GeometryFormat, &DisplayInfo::geometry_format>;
using HasTransformation = detail::SceneDisplayChecker<mat4, &DisplayInfo::transformation>;
using HasUniformColor = detail::SceneDisplayChecker<vec3, &DisplayInfo::uniform_color>;
using HasColoring = detail::SceneDisplayChecker<Coloring, &DisplayInfo::coloring>;
using HasShading = detail::SceneDisplayChecker<Shading, &DisplayInfo::shading>;
using HasOpacity = detail::SceneDisplayChecker<float, &DisplayInfo::opacity>;

} // namespace gvs
