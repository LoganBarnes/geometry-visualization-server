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

// version will be inserted automagically

/*
 * Constants
 */
const float PI  = 3.141592653589793f;
const float INF = 1.f / 0.f;

const int COLORING_POSITIONS           = 0;
const int COLORING_NORMALS             = 1;
const int COLORING_TEXTURE_COORDINATES = 2;
const int COLORING_VERTEX_COLORS       = 3;
const int COLORING_UNIFORM_COLOR       = 4;
const int COLORING_TEXTURE             = 5;
const int COLORING_WHITE               = 6;

const int SHADING_COLOR         = 0;
const int SHADING_LAMBERTIAN    = 1;
const int SHADING_COOK_TORRANCE = 2;

/*
 * Inputs
 */
layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;
layout(location = 2) in vec2 texture_coordinates;
layout(location = 3) in vec3 vertex_color;

/*
 * Uniforms
 */
uniform int   coloring      = COLORING_UNIFORM_COLOR;
uniform vec3  uniform_color = { 1.f, 0.9f, 0.7f };
uniform float opacity       = 1.f;

uniform int   shading         = SHADING_COLOR;
uniform vec3  light_direction = { -1.f, -2.f, -3.f };
uniform vec3  light_color     = { +1.f, +1.f, +1.f };
uniform float ambient_scale   = 0.15f;

uniform uint id = 0;

layout(location = 0) out vec4 out_color;
layout(location = 1) out uint out_id;

void main()
{
    out_id = id;
    vec3 shape_color = { 1.f, 1.f, 1.f };

    switch (coloring) {
        case COLORING_POSITIONS:
        shape_color = world_position;
        break;

        case COLORING_NORMALS:
        shape_color = world_normal * 0.5f + 0.5f;// between 0 and 1
        break;

        case COLORING_TEXTURE_COORDINATES:
        shape_color = vec3(texture_coordinates, 1.f);
        break;

        case COLORING_VERTEX_COLORS:
        shape_color = vertex_color;
        break;

        case COLORING_UNIFORM_COLOR:
        shape_color = uniform_color;
        break;

        case COLORING_TEXTURE:// TODO
        case COLORING_WHITE:
        break;

    }// end switch(coloring)

    vec3 final_color = { 1.f, 1.f, 1.f };

    vec3 surface_normal = normalize(world_normal);
    vec3 direction_to_light = normalize(-light_direction);

    switch (shading) {
        case SHADING_COLOR:
        final_color = shape_color;
        break;

        case SHADING_COOK_TORRANCE:// TODO
        case SHADING_LAMBERTIAN:
        vec3 diffuse_lighting = max(0.f, dot(surface_normal, direction_to_light)) * light_color;
        final_color = diffuse_lighting * shape_color + ambient_scale * shape_color;
        break;

    }// end switch(shading)

    out_color = vec4(final_color, opacity);
}
