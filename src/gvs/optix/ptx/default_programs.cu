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
// This define shouldn't be here. TODO: Figure out how to include the header properly
#define __CUDA_INCLUDE_COMPILER_INTERNAL_HEADERS__
#include <optix.h>
#include <optixu/optixu_math_namespace.h>

struct PerRayData {
    float3 result;
    float depth;
};

rtDeclareVariable(float3, eye, , );
rtDeclareVariable(float3, U, , );
rtDeclareVariable(float3, V, , );
rtDeclareVariable(float3, W, , );
rtDeclareVariable(float, scene_epsilon, , );
rtDeclareVariable(rtObject, top_object, , );
rtDeclareVariable(unsigned int, radiance_ray_type, , );

rtDeclareVariable(uint2, launch_index, rtLaunchIndex, );
rtDeclareVariable(uint2, launch_dim, rtLaunchDim, );

rtDeclareVariable(PerRayData, prd_current, rtPayload, );
rtBuffer<float4, 2> output_buffer;

rtDeclareVariable(float3, background_color, , );
rtDeclareVariable(float3, error_color, , );
rtDeclareVariable(float, miss_depth, , );

RT_PROGRAM void pinhole_camera() {
    auto d = make_float2(launch_index) / make_float2(launch_dim) * 2.f - 1.f;
    float3 ray_origin = eye;
    float3 ray_direction = optix::normalize(d.x * U + d.y * V + W);

    optix::Ray ray = optix::make_Ray(ray_origin, ray_direction, radiance_ray_type, scene_epsilon, RT_DEFAULT_MAX);

    PerRayData prd;
    prd.depth = miss_depth;

    rtTrace(top_object, ray, prd);

    output_buffer[launch_index] = make_float4(prd.result, prd.depth);
}

RT_PROGRAM void miss() {
    prd_current.result = background_color;
}

RT_PROGRAM void exception() {
    const unsigned int code = rtGetExceptionCode();
    rtPrintf("Caught exception 0x%X at launch index (%d,%d)\n", code, launch_index.x, launch_index.y);
    output_buffer[launch_index] = make_float4(error_color, 1.0);
}
