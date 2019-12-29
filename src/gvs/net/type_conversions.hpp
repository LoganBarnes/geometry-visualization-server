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

// generated
#include <types.pb.h>

namespace gvs {

/*
 * To proto
 */
auto to_proto(std::string const& value) -> google::protobuf::StringValue;
auto to_proto(bool const& value) -> google::protobuf::BoolValue;
auto to_proto(float const& value) -> google::protobuf::FloatValue;

auto to_proto(SceneID const& value) -> net::SceneId;

auto to_proto(vec3 const& value) -> net::Vec3;
auto to_proto(mat4 const& value) -> net::Mat4;

auto to_proto(GeometryFormat const& value) -> net::GeometryFormat;
auto to_proto(Coloring const& value) -> net::Coloring;
auto to_proto(Shading const& value) -> net::Shading;

template <std::size_t N>
auto to_proto(AttributeVector<N> const& value) -> net::FloatList;
auto to_proto(std::vector<unsigned> const& value) -> net::UIntList;
auto to_proto(std::vector<SceneID> const& value) -> net::SceneIdList;

auto to_proto(Primitive const& value) -> net::Primitive;
auto to_proto(GeometryInfoSetter const& value) -> net::GeometryInfo3d;
auto to_proto(DisplayInfoSetter const& value) -> net::DisplayInfo;
auto to_proto(Geometry const& value) -> net::Geometry;

auto to_proto(SceneItemInfoSetter const& value) -> net::SceneItemInfo;

/*
 * From proto
 */
auto from_proto(google::protobuf::StringValue const& proto) -> std::string;
auto from_proto(google::protobuf::BoolValue const& proto) -> bool;
auto from_proto(google::protobuf::FloatValue const& proto) -> float;

auto from_proto(net::SceneId const& proto) -> SceneID;

auto from_proto(net::Vec3 const& proto) -> vec3;
auto from_proto(net::Mat4 const& proto) -> mat4;

auto from_proto(net::GeometryFormat const& proto) -> GeometryFormat;
auto from_proto(net::Coloring const& proto) -> Coloring;
auto from_proto(net::Shading const& proto) -> Shading;

template <std::size_t N>
auto from_proto(net::FloatList const& proto) -> AttributeVector<N>;
auto from_proto(net::UIntList const& proto) -> std::vector<unsigned>;
auto from_proto(net::SceneIdList const& proto) -> std::vector<SceneID>;

auto from_proto(net::Primitive const& proto) -> Primitive;
auto from_proto(net::GeometryInfo3d const& proto) -> GeometryInfoSetter;
auto from_proto(net::DisplayInfo const& proto) -> DisplayInfoSetter;
auto from_proto(net::Geometry const& proto) -> Geometry;

auto from_proto(net::SceneItemInfo const& proto) -> SceneItemInfoSetter;

} // namespace gvs
