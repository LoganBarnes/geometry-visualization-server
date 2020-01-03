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

// external
#include <mapbox/variant.hpp> // C++11 variant

namespace gvs {

/// \brief A set of unit length axes at the origin.
///        <R,G,B> corresponds to <X,Y,Z>
struct Axes {};

/// \brief A cone of with a base radius of 1.
///        The center is at the origin and the rotational axis is along Y.
struct Cone {
    float    half_length = 1.f;
    unsigned rings       = 35u;
    unsigned segments    = 35u;
};

/// \brief A 2x2x2 cube centered at the origin.
struct Cube {};

/// \brief A cylinder with a radius of 1.
///        The center is at the origin and the rotational axis is along Y.
struct Cylinder {
    float    half_length = 1.f;
    unsigned rings       = 35u;
    unsigned segments    = 35u;
};

/// \brief A 2x2 plane in the XY plane centered at the origin.
struct Plane {};

/// \brief A unit sphere (radius 1) centered at the origin.
struct Sphere {
    unsigned rings    = 35u;
    unsigned segments = 35u;
};

namespace detail {
using PrimitiveBase = mapbox::util::variant<Axes, Cone, Cube, Cylinder, Plane, Sphere>;
}

struct Primitive : detail::PrimitiveBase {
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Primitive(Axes shape) : detail::PrimitiveBase(shape) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Primitive(Cone shape) : detail::PrimitiveBase(shape) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Primitive(Cube shape) : detail::PrimitiveBase(shape) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Primitive(Cylinder shape) : detail::PrimitiveBase(shape) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Primitive(Plane shape) : detail::PrimitiveBase(shape) {}
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    Primitive(Sphere shape) : detail::PrimitiveBase(shape) {}
};

} // namespace gvs
