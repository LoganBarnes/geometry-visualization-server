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

layout(location = 0) in vec4 local_position;
layout(location = 1) in vec3 local_normal;
layout(location = 2) in vec2 texture_coordinates;
layout(location = 3) in vec3 vertex_color;

uniform mat4 world_from_local = mat4(1.f);
uniform mat3 world_from_local_normals = mat3(1.f);
uniform mat4 projection_from_local = mat4(1.f);

layout(location = 0) out vec3 world_position_out;
layout(location = 1) out vec3 world_normal_out;
layout(location = 2) out vec2 texture_coordinates_out;
layout(location = 3) out vec3 vertex_color_out;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    world_position_out      = vec3(world_from_local * local_position);
    world_normal_out        = world_from_local_normals * local_normal;
    texture_coordinates_out = texture_coordinates;
    vertex_color_out        = vertex_color;

    gl_Position = projection_from_local * local_position;
}
