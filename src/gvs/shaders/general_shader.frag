// ///////////////////////////////////////////////////////////////////////////////////////
// Geometry Visualization Server
// Copyright (c) 2018 Logan Barnes - All Rights Reserved
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

// version will be inserted automagically

/*
 * Constants
 */
const float PI = 3.141592653589793f;
const float INF = 1.f / 0.f;

const int COLOR_TYPE_POSITIONS = 0;
const int COLOR_TYPE_NORMALS = 1;
const int COLOR_TYPE_TEXTURE_COORDINATES = 2;
const int COLOR_TYPE_VERTEX_COLORS = 3;
const int COLOR_TYPE_UNIFORM_COLOR = 4;
const int COLOR_TYPE_TEXTURE = 5;
const int COLOR_TYPE_WHITE = 6;

/*
 * Inputs
 */
layout(location = 0) in vec3 view_position;
layout(location = 1) in vec3 view_normal;
layout(location = 2) in vec2 texture_coordinates;
layout(location = 3) in vec3 vertex_color;

/*
 * Uniforms
 */
uniform int coloring = COLOR_TYPE_UNIFORM_COLOR;
uniform vec3 uniform_color = {1.f, 0.9f, 0.7f};
uniform float opacity = 1.f;

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 shape_color = {1.f, 1.f, 1.f};

    switch(coloring) {
        case COLOR_TYPE_POSITIONS:
            shape_color = view_position;
            break;

        case COLOR_TYPE_NORMALS:
            shape_color = view_normal * 0.5f + 0.5f; // between 0 and 1
            break;

        case COLOR_TYPE_TEXTURE_COORDINATES:
            shape_color = vec3(texture_coordinates, 1.f);
            break;

        case COLOR_TYPE_VERTEX_COLORS:
            shape_color = vertex_color;
            break;

        case COLOR_TYPE_UNIFORM_COLOR:
            shape_color = uniform_color;
            break;

        case COLOR_TYPE_TEXTURE: // TODO
        case COLOR_TYPE_WHITE:
            break;

    }

    out_color = vec4(shape_color, opacity);
}
