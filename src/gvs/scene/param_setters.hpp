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
 * The SceneSetter and SceneGetter classes are kept generic since they do the same thing for
 * many different types. To create a new named parameter we add a new class that derives from
 * SceneSetter or SceneGetter.
 *
 * The Curiously Recurring Template Pattern (CRTP) is used to avoid any indirection performance
 * costs typically associated with base classes. Because of this, each derived class will have
 * to implement a 'move_data' or 'update_data' function for the SceneSetter and SceneGetter classes
 * respectively.
 */

///
/// \brief A "named parameter" wrapper used to set items in a StaticScene
///
template <typename T, std::optional<T> SceneItemInfo::*member>
struct SceneSetter {
    explicit SceneSetter(T value = {}) : data_(std::move(value)) {}

    auto operator()(SceneItemInfo* info) -> std::string {
        if (info->*member) {
            return "Scene parameter already set.";
        }
        info->*member = std::move(data_);
        return "";
    }

    SceneSetter(const SceneSetter&)     = delete;
    SceneSetter(SceneSetter&&) noexcept = delete;
    SceneSetter& operator=(const SceneSetter&) = delete;
    SceneSetter& operator=(SceneSetter&&) noexcept = delete;

private:
    T data_;
};

///
/// \brief A "named parameter" wrapper used to set geometry info for items in a StaticScene
///
template <typename T, std::optional<T> GeometryInfo::*member>
struct SceneGeometrySetter {
    explicit SceneGeometrySetter(T value = {}) : data_(std::move(value)) {}

    auto operator()(SceneItemInfo* info) -> std::string {
        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }

        auto& geometry_info = info->geometry_info.value();
        if (geometry_info.*member) {
            return "Scene parameter already set.";
        }

        geometry_info.*member = std::move(data_);
        return "";
    }

    SceneGeometrySetter(const SceneGeometrySetter&)     = delete;
    SceneGeometrySetter(SceneGeometrySetter&&) noexcept = delete;
    SceneGeometrySetter& operator=(const SceneGeometrySetter&) = delete;
    SceneGeometrySetter& operator=(SceneGeometrySetter&&) noexcept = delete;

private:
    T data_;
};

///
/// \brief A "named parameter" wrapper used to set geometry indices for items in a StaticScene
///
template <gvs::GeometryFormat Format>
struct SceneIndicesSetter {
    explicit SceneIndicesSetter(std::vector<unsigned> value = {}) : data_(std::move(value)) {}

    auto operator()(SceneItemInfo* info) -> std::string {
        if (!info->geometry_info) {
            info->geometry_info = GeometryInfo{};
        }
        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }

        auto& geometry_info = info->geometry_info.value();
        auto& display_info  = info->display_info.value();

        if (geometry_info.indices) {
            return "Scene parameter already set.";
        }

        if (display_info.geometry_format) {
            return "Scene parameter already set.";
        }

        geometry_info.indices        = std::move(data_);
        display_info.geometry_format = Format;

        return "";
    }

    SceneIndicesSetter(const SceneIndicesSetter&)     = delete;
    SceneIndicesSetter(SceneIndicesSetter&&) noexcept = delete;
    SceneIndicesSetter& operator=(const SceneIndicesSetter&) = delete;
    SceneIndicesSetter& operator=(SceneIndicesSetter&&) noexcept = delete;

private:
    std::vector<unsigned> data_;
};

///
/// \brief A "named parameter" wrapper used to set display info for items in a StaticScene
///
template <typename T, std::optional<T> DisplayInfo::*member>
struct SceneDisplaySetter {
    explicit SceneDisplaySetter(T value = {}) : data(std::move(value)) {}

    auto operator()(SceneItemInfo* info) -> std::string {
        if (!info->display_info) {
            info->display_info = DisplayInfo{};
        }

        auto& display_info = info->display_info.value();
        if (display_info.*member) {
            return "Scene parameter already set.";
        }

        display_info.*member = std::move(data);
        return "";
    }

    SceneDisplaySetter(const SceneDisplaySetter&)     = delete;
    SceneDisplaySetter(SceneDisplaySetter&&) noexcept = delete;
    SceneDisplaySetter& operator=(const SceneDisplaySetter&) = delete;
    SceneDisplaySetter& operator=(SceneDisplaySetter&&) noexcept = delete;

protected:
    T data;
};

} // namespace detail

/*
 * Setters
 */
using SetPositions3d          = detail::SceneGeometrySetter<AttributeVector<3>, &GeometryInfo::positions>;
using SetNormals3d            = detail::SceneGeometrySetter<AttributeVector<3>, &GeometryInfo::normals>;
using SetTextureCoordinates3d = detail::SceneGeometrySetter<AttributeVector<2>, &GeometryInfo::texture_coordinates>;
using SetVertexColors3d       = detail::SceneGeometrySetter<AttributeVector<3>, &GeometryInfo::vertex_colors>;
using SetIndices              = detail::SceneGeometrySetter<std::vector<unsigned>, &GeometryInfo::indices>;

using SetPoints        = detail::SceneIndicesSetter<GeometryFormat::Points>;
using SetLines         = detail::SceneIndicesSetter<GeometryFormat::Lines>;
using SetLineStrip     = detail::SceneIndicesSetter<GeometryFormat::LineStrip>;
using SetTriangles     = detail::SceneIndicesSetter<GeometryFormat::Triangles>;
using SetTriangleStrip = detail::SceneIndicesSetter<GeometryFormat::TriangleStrip>;
using SetTriangleFan   = detail::SceneIndicesSetter<GeometryFormat::TriangleFan>;

using SetParent = detail::SceneSetter<SceneID, &SceneItemInfo::parent>;

using SetReadableId     = detail::SceneDisplaySetter<std::string, &DisplayInfo::readable_id>;
using SetGeometryFormat = detail::SceneDisplaySetter<GeometryFormat, &DisplayInfo::geometry_format>;
using SetTransformation = detail::SceneDisplaySetter<mat4, &DisplayInfo::transformation>;
using SetUniformColor   = detail::SceneDisplaySetter<vec3, &DisplayInfo::uniform_color>;
using SetColoring       = detail::SceneDisplaySetter<Coloring, &DisplayInfo::coloring>;
using SetShading        = detail::SceneDisplaySetter<Shading, &DisplayInfo::shading>;
using SetVisible        = detail::SceneDisplaySetter<bool, &DisplayInfo::visible>;
using SetOpacity        = detail::SceneDisplaySetter<float, &DisplayInfo::opacity>;
using SetWireframeOnly  = detail::SceneDisplaySetter<bool, &DisplayInfo::wireframe_only>;

} // namespace gvs
