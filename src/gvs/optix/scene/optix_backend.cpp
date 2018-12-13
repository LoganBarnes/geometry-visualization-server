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
#include "optix_backend.hpp"

#include <gvs/optix/ptx_files.hpp>

#include <iostream>
#include <sstream>

namespace gvs {
namespace vis {

namespace {

std::unordered_map<std::string, std::string> build_ptx_file_map() {
    std::unordered_map<std::string, std::string> ptx_files;

    std::istringstream path_stream(ptx::ptx_files);
    std::string path;
    while (std::getline(path_stream, path, ';')) {
        auto path_end = path.rfind('/');
        ptx_files.emplace(path.substr(path_end + 1), path);
    }

    return ptx_files;
}

} // namespace

OptiXBackend::OptiXBackend()
    : context_(new optix::Context(optix::Context::create()),
               [](auto* p) {
                   static_assert(std::is_same<decltype(p), optix::Context*>::value, "");
                   (*p)->destroy();
                   delete p;
               })
    , ptx_files_(build_ptx_file_map()) {

    for (const auto& file_pair : ptx_files_) {
        std::cout << file_pair.first << ": " << file_pair.second << std::endl;
    }

    optix::Context& ctx = *context_;

    // Set up context
    ctx->setRayTypeCount(2);
    ctx->setEntryPointCount(1);

    ctx["radiance_ray_type"]->setUint(0u);
    ctx["shadow_ray_type"]->setUint(1u);
    ctx["scene_epsilon"]->setFloat(1.e-2f);
}

} // namespace vis

} // namespace gvs
