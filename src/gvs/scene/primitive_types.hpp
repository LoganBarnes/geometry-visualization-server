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

struct Cone {
    float height        = 1.f;
    float base_diameter = 1.f;
};

struct Cube {
    float width = 1.f;
};

struct Cylinder {
    float height   = 1.f;
    float diameter = 1.f;
};

struct Sphere {
    float diameter = 1.f;
};

struct Torus {
    float major_diameter = 1.0f; ///< Diameter of ring through center of doughnut
    float minor_diameter = 0.5f; ///< Thickness of the doughnut
};

struct Quad {
    float width = 1.f;
};

namespace detail {
using PrimitiveBase = mapbox::util::variant<Cone, Cube, Cylinder, Sphere, Torus, Quad>;
}

struct Primitive : detail::PrimitiveBase {
    using detail::PrimitiveBase::variant;

    int u_tessellation = 25;
    int v_tessellation = 25;
};

} // namespace gvs
