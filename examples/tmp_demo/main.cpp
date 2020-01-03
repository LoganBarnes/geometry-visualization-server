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
// project
#include "gvs/display/display_scene.hpp"
#include "gvs/display/local_scene.hpp"
#include "gvs/net/client_scene.hpp"
#include "gvs/scene/log_params.hpp"
#include "gvs/scene/nil_scene.hpp"

// examples
#include "../common/test_scene.hpp"

int main(int argc, char* argv[]) {
    std::string server_address = "0.0.0.0:50055";

    if (argc > 1) {
        server_address = argv[1];
    }

    std::unique_ptr<gvs::scene::Scene> scene;

#define SCENE 1

#if SCENE == 0
    scene = std::make_unique<gvs::display::LocalScene>(gvs::display::BackendType::Empty);
#elif SCENE == 1
    scene = std::make_unique<gvs::display::DisplayScene>();
#elif SCENE == 2
    scene = std::make_unique<gvs::net::ClientScene>(server_address);
#else
    auto client_scene = std::make_unique<gvs::net::ClientScene>(server_address);
    if (client_scene->connected()) {
        scene = std::move(client_scene);
    } else {
        std::cerr << "Falling back to NilScene" << std::endl;
        scene = std::make_unique<gvs::scene::NilScene>();
    }
#endif

    scene->clear();

    auto trans = gvs::identity_mat4;
    trans[12]  = +0.0f; // X
    trans[13]  = +0.0f; // Y
    trans[14]  = +0.0f; // Z

    auto test_root
        = scene->add_item(gvs::SetReadableId("Test Scene"), gvs::SetPositions3d(), gvs::SetTransformation(trans));

    trans[12] = -0.0f; // X
    trans[13] = -0.0f; // Y
    trans[14] = -0.0f; // Z

    auto primitives_root
        = scene->add_item(gvs::SetReadableId("Primitives"), gvs::SetPositions3d(), gvs::SetTransformation(trans));

    example::build_test_scene(scene.get(), test_root);
    example::build_primitive_scene(scene.get(), primitives_root);

    return 0;
}
