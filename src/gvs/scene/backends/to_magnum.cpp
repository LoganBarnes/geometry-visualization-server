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
#include "to_magnum.hpp"

// external
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/Math/Vector3.h>

namespace gvs::scene::backends {

auto to_magnum(GeometryFormat const& format) -> Magnum::MeshPrimitive {
    switch (format) {
    case GeometryFormat::Points:
        return Magnum::MeshPrimitive::Points;
    case GeometryFormat::Lines:
        return Magnum::MeshPrimitive::Lines;
    case GeometryFormat::LineStrip:
        return Magnum::MeshPrimitive::LineStrip;
    case GeometryFormat::Triangles:
        return Magnum::MeshPrimitive::Triangles;
    case GeometryFormat::TriangleStrip:
        return Magnum::MeshPrimitive::TriangleStrip;
    case GeometryFormat::TriangleFan:
        return Magnum::MeshPrimitive::TriangleFan;
    }
    throw std::runtime_error("Unreachable code");
}
auto to_magnum(vec2 const& vector) -> Magnum::Vector2 {
    return {vector[0], vector[1]};
}

auto to_magnum(vec3 const& vector) -> Magnum::Vector3 {
    return {vector[0], vector[1], vector[2]};
}

auto to_magnum(mat4 const& matrix) -> Magnum::Matrix4 {
    return {Magnum::Math::RectangularMatrix<4, 4, float>::from(&matrix[0])};
}

} // namespace gvs::scene::backends
