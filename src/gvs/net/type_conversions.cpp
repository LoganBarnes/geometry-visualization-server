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
#include "type_conversions.hpp"

namespace gvs {

/*
 * To proto
 */
auto to_proto(std::string const& value) -> google::protobuf::StringValue {
    google::protobuf::StringValue proto = {};
    proto.set_value(value);
    return proto;
}

auto to_proto(bool const& value) -> google::protobuf::BoolValue {
    google::protobuf::BoolValue proto = {};
    proto.set_value(value);
    return proto;
}

auto to_proto(float const& value) -> google::protobuf::FloatValue {
    google::protobuf::FloatValue proto = {};
    proto.set_value(value);
    return proto;
}

auto to_proto(SceneID const& value) -> net::SceneId {
    net::SceneId proto = {};
    proto.set_value(gvs::to_string(value));
    return proto;
}

auto to_proto(vec3 const& value) -> net::Vec3 {
    net::Vec3 proto = {};
    proto.set_x(value[0]);
    proto.set_y(value[1]);
    proto.set_z(value[2]);
    return proto;
}

auto to_proto(mat4 const& value) -> net::Mat4 {
    net::Mat4 proto       = {};
    *proto.mutable_data() = {value.begin(), value.end()};
    return proto;
}

auto to_proto(GeometryFormat const& value) -> net::GeometryFormat {
    net::GeometryFormat proto = {};

    switch (value) {
    case GeometryFormat::Points:
        proto.set_type(net::GeometryFormat::POINTS);
        break;
    case GeometryFormat::Lines:
        proto.set_type(net::GeometryFormat::LINES);
        break;
    case GeometryFormat::LineStrip:
        proto.set_type(net::GeometryFormat::LINE_STRIP);
        break;
    case GeometryFormat::Triangles:
        proto.set_type(net::GeometryFormat::TRIANGLES);
        break;
    case GeometryFormat::TriangleStrip:
        proto.set_type(net::GeometryFormat::TRIANGLE_STRIP);
        break;
    case GeometryFormat::TriangleFan:
        proto.set_type(net::GeometryFormat::TRIANGLE_FAN);
        break;
    }
    return proto;
}

auto to_proto(Coloring const& value) -> net::Coloring {
    net::Coloring proto = {};

    switch (value) {
    case Coloring::Positions:
        proto.set_type(net::Coloring::POSITIONS);
        break;
    case Coloring::Normals:
        proto.set_type(net::Coloring::NORMALS);
        break;
    case Coloring::TextureCoordinates:
        proto.set_type(net::Coloring::TEXTURE_COORDINATES);
        break;
    case Coloring::VertexColors:
        proto.set_type(net::Coloring::VERTEX_COLORS);
        break;
    case Coloring::UniformColor:
        proto.set_type(net::Coloring::UNIFORM_COLOR);
        break;
    case Coloring::Texture:
        proto.set_type(net::Coloring::TEXTURE);
        break;
    case Coloring::White:
        proto.set_type(net::Coloring::WHITE);
        break;
    }
    return proto;
}

auto to_proto(Shading const& value) -> net::Shading {
    net::Shading proto = {};

    switch (value) {
    case Shading::UniformColor:
        proto.set_type(net::Shading::UNIFORM_COLOR);
        break;
    case Shading::Lambertian:
        proto.set_type(net::Shading::LAMBERTIAN);
        break;
    case Shading::CookTorrance:
        proto.set_type(net::Shading::COOK_TORRANCE);
        break;
    }
    return proto;
}

template <std::size_t N>
auto to_proto(AttributeVector<N> const& value) -> net::FloatList {
    net::FloatList proto   = {};
    *proto.mutable_value() = {value.begin(), value.end()};
    return proto;
}

auto to_proto(std::vector<unsigned> const& value) -> net::UIntList {
    net::UIntList proto    = {};
    *proto.mutable_value() = {value.begin(), value.end()};
    return proto;
}

auto to_proto(std::vector<SceneID> const& value) -> net::SceneIdList {
    net::SceneIdList proto = {};
    for (auto const& scene_id : value) {
        *proto.add_value() = to_proto(scene_id);
    }
    return proto;
}

//auto to_proto(GeometryInfoSetter const& value) -> net::GeometryInfo3d {
//    net::GeometryInfo3d proto = {};
//    if (value.positions) {
//        *proto.mutable_positions() = to_proto(*value.positions);
//    }
//    if (value.normals) {
//        *proto.mutable_normals() = to_proto(*value.normals);
//    }
//    if (value.texture_coordinates) {
//        *proto.mutable_texture_coordinates() = to_proto(*value.texture_coordinates);
//    }
//    if (value.vertex_colors) {
//        *proto.mutable_vertex_colors() = to_proto(*value.vertex_colors);
//    }
//    return proto;
//}

auto to_proto(DisplayInfoSetter const& value) -> net::DisplayInfo {
    net::DisplayInfo proto = {};
    if (value.readable_id) {
        *proto.mutable_readable_id() = to_proto(*value.readable_id);
    }
    if (value.geometry_format) {
        *proto.mutable_geometry_format() = to_proto(*value.geometry_format);
    }
    if (value.transformation) {
        *proto.mutable_transformation() = to_proto(*value.transformation);
    }
    if (value.uniform_color) {
        *proto.mutable_uniform_color() = to_proto(*value.uniform_color);
    }
    if (value.coloring) {
        *proto.mutable_coloring() = to_proto(*value.coloring);
    }
    if (value.shading) {
        *proto.mutable_shading() = to_proto(*value.shading);
    }
    if (value.visible) {
        *proto.mutable_visible() = to_proto(*value.visible);
    }
    if (value.opacity) {
        *proto.mutable_opacity() = to_proto(*value.opacity);
    }
    if (value.wireframe_only) {
        *proto.mutable_wireframe_only() = to_proto(*value.wireframe_only);
    }
    return proto;
}

template auto to_proto(AttributeVector<3> const& value) -> net::FloatList;
template auto to_proto(AttributeVector<2> const& value) -> net::FloatList;

/*
 * From proto
 */
auto from_proto(google::protobuf::StringValue const& proto) -> std::string {
    return proto.value();
}

auto from_proto(google::protobuf::BoolValue const& proto) -> bool {
    return proto.value();
}

auto from_proto(google::protobuf::FloatValue const& proto) -> float {
    return proto.value();
}

auto from_proto(net::SceneId const& proto) -> SceneID {
    return gvs::from_string(proto.value());
}

auto from_proto(net::Vec3 const& proto) -> vec3 {
    return {proto.x(), proto.y(), proto.z()};
}

auto from_proto(net::Mat4 const& proto) -> mat4 {
    if (proto.data_size() != 16) {
        throw std::invalid_argument("Mat4 data must contain 16 elements");
    }
    return {
        proto.data(0),
        proto.data(1),
        proto.data(2),
        proto.data(3),
        proto.data(4),
        proto.data(5),
        proto.data(6),
        proto.data(7),
        proto.data(8),
        proto.data(9),
        proto.data(10),
        proto.data(11),
        proto.data(12),
        proto.data(13),
        proto.data(14),
        proto.data(15),
    };
}

auto from_proto(net::GeometryFormat const& proto) -> GeometryFormat {
    switch (proto.type()) {
    case net::GeometryFormat::POINTS:
        return GeometryFormat::Points;
    case net::GeometryFormat::LINES:
        return GeometryFormat::Lines;
    case net::GeometryFormat::LINE_STRIP:
        return GeometryFormat::LineStrip;
    case net::GeometryFormat::TRIANGLES:
        return GeometryFormat::Triangles;
    case net::GeometryFormat::TRIANGLE_STRIP:
        return GeometryFormat::TriangleStrip;
    case net::GeometryFormat::TRIANGLE_FAN:
        return GeometryFormat::TriangleFan;

    case net::GeometryFormat_Type_GeometryFormat_Type_INT_MIN_SENTINEL_DO_NOT_USE_:
    case net::GeometryFormat_Type_GeometryFormat_Type_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    throw std::runtime_error("net::GeometryFormat enum not set properly");
}

auto from_proto(net::Coloring const& proto) -> Coloring {
    switch (proto.type()) {
    case net::Coloring::POSITIONS:
        return Coloring::Positions;
    case net::Coloring::NORMALS:
        return Coloring::Normals;
    case net::Coloring::TEXTURE_COORDINATES:
        return Coloring::TextureCoordinates;
    case net::Coloring::VERTEX_COLORS:
        return Coloring::VertexColors;
    case net::Coloring::UNIFORM_COLOR:
        return Coloring::UniformColor;
    case net::Coloring::TEXTURE:
        return Coloring::Texture;
    case net::Coloring::WHITE:
        return Coloring::White;

    case net::Coloring_Type_Coloring_Type_INT_MIN_SENTINEL_DO_NOT_USE_:
    case net::Coloring_Type_Coloring_Type_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    throw std::runtime_error("net::Coloring enum not set properly");
}

auto from_proto(net::Shading const& proto) -> Shading {
    switch (proto.type()) {
    case net::Shading::UNIFORM_COLOR:
        return Shading::UniformColor;
    case net::Shading::LAMBERTIAN:
        return Shading::Lambertian;
    case net::Shading::COOK_TORRANCE:
        return Shading::CookTorrance;

    case net::Shading_Type_Shading_Type_INT_MIN_SENTINEL_DO_NOT_USE_:
    case net::Shading_Type_Shading_Type_INT_MAX_SENTINEL_DO_NOT_USE_:
        break;
    }
    throw std::runtime_error("net::Shading enum not set properly");
}

template <std::size_t N>
auto from_proto(net::FloatList const& proto) -> AttributeVector<N> {
    return {proto.value().begin(), proto.value().end()};
}

auto from_proto(net::UIntList const& proto) -> std::vector<unsigned> {
    return {proto.value().begin(), proto.value().end()};
}

auto from_proto(net::SceneIdList const& proto) -> std::vector<SceneID> {
    std::vector<SceneID> scene_ids;

    for (auto const& scene_id : proto.value()) {
        scene_ids.emplace_back(from_proto(scene_id));
    }
    return scene_ids;
}

auto from_proto(net::GeometryInfo3d const& proto) -> GeometryInfoSetter {
    GeometryInfoSetter value = {};

    if (proto.has_positions()) {
        value.positions = std::make_unique<AttributeVector<3>>(from_proto<3>(proto.positions()));
    }
    if (proto.has_normals()) {
        value.normals = std::make_unique<AttributeVector<3>>(from_proto<3>(proto.normals()));
    }
    if (proto.has_texture_coordinates()) {
        value.texture_coordinates = std::make_unique<AttributeVector<2>>(from_proto<2>(proto.texture_coordinates()));
    }
    if (proto.has_vertex_colors()) {
        value.vertex_colors = std::make_unique<AttributeVector<3>>(from_proto<3>(proto.vertex_colors()));
    }
    return value;
}

auto from_proto(net::DisplayInfo const& proto) -> DisplayInfoSetter {
    DisplayInfoSetter value = {};

    if (proto.has_readable_id()) {
        value.readable_id = std::make_unique<std::string>(from_proto(proto.readable_id()));
    }
    if (proto.has_geometry_format()) {
        value.geometry_format = std::make_unique<GeometryFormat>(from_proto(proto.geometry_format()));
    }
    if (proto.has_transformation()) {
        value.transformation = std::make_unique<mat4>(from_proto(proto.transformation()));
    }
    if (proto.has_uniform_color()) {
        value.uniform_color = std::make_unique<vec3>(from_proto(proto.uniform_color()));
    }
    if (proto.has_coloring()) {
        value.coloring = std::make_unique<Coloring>(from_proto(proto.coloring()));
    }
    if (proto.has_shading()) {
        value.shading = std::make_unique<Shading>(from_proto(proto.shading()));
    }
    if (proto.has_visible()) {
        value.visible = std::make_unique<bool>(from_proto(proto.visible()));
    }
    if (proto.has_opacity()) {
        value.opacity = std::make_unique<float>(from_proto(proto.opacity()));
    }
    if (proto.has_wireframe_only()) {
        value.wireframe_only = std::make_unique<bool>(from_proto(proto.wireframe_only()));
    }
    return value;
}

template auto from_proto(net::FloatList const& proto) -> AttributeVector<3>;
template auto from_proto(net::FloatList const& proto) -> AttributeVector<2>;

} // namespace gvs