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
#include "test_scene.hpp"

// project
#include "gvs/scene/log_params.hpp"
#include "gvs/scene/scene.hpp"

// standard
#include <cmath>
#include <random>

namespace example {
namespace {

constexpr gvs::vec3 blueish  = {0.3f, 0.3f, 1.f};
constexpr gvs::vec3 greenish = {0.3f, 1.f, 0.3f};
constexpr gvs::vec3 redish   = {1.f, 0.3f, 0.3f};
constexpr gvs::vec3 orange   = {1.f, 0.5f, 0.1f};

} // namespace

auto build_test_scene(gvs::scene::Scene* scene, gvs::SceneId const& root_id) -> void {
    {
        scene->add_item(gvs::SetReadableId("Axes"),
                        gvs::SetPositions3d({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f}),
                        gvs::SetVertexColors3d({1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f}),
                        gvs::SetColoring(gvs::Coloring::VertexColors),
                        gvs::SetShading(gvs::Shading::UniformColor),
                        gvs::SetLines({0, 1, 0, 2, 0, 3}),
                        gvs::SetParent(root_id));
    }

    std::vector<float> circle;

    constexpr int max_verts = 50;
    for (int i = 0; i <= max_verts; ++i) {
        float angle = pi * static_cast<float>(i) * 2.f / max_verts;
        circle.emplace_back(std::cos(angle));
        circle.emplace_back(std::sin(angle));
        circle.emplace_back(0.f);
    }

    // Snowman
    {
        auto head_id
            = scene->add_item(gvs::SetReadableId("Head"),
                              gvs::SetPositions3d(circle),
                              gvs::SetTransformation({0.75f, 0, 0, 0, 0, 0.75f, 0, 0, 0, 0, 0.75f, 0, 2, 2, 1, 1}),
                              gvs::SetUniformColor({1.f, 0.5f, 1.f}),
                              gvs::SetLineStrip(),
                              gvs::SetShading(gvs::Shading::UniformColor),
                              gvs::SetParent(root_id));

        auto body_id
            = scene->add_item(gvs::SetReadableId("Body"),
                              gvs::SetParent(head_id),
                              gvs::SetPositions3d(circle),
                              gvs::SetTransformation({1.3f, 0, 0, 0, 0, 1.3f, 0, 0, 0, 0, 1.3f, 0, 0, -2.3f, 0, 1}),
                              gvs::SetUniformColor({1.f, 1.f, 0.5f}),
                              gvs::SetLineStrip());

        scene->add_item(gvs::SetReadableId("Feet"),
                        gvs::SetParent(body_id),
                        gvs::SetPositions3d(circle),
                        gvs::SetTransformation({1.3f, 0, 0, 0, 0, 1.3f, 0, 0, 0, 0, 1.3f, 0, 0, -2.3f, 0, 1}),
                        gvs::SetUniformColor({0.5f, 1.f, 1.f}),
                        gvs::SetTriangleFan());
    }

    scene->add_item(gvs::SetPositions3d({-1.f, -1.f, 0.f, 1.f, -1.f, 0.f, 0.f, 1.5f, -1.f}),
                    gvs::SetUniformColor({.5f, 0.25f, 0.05f}),
                    gvs::SetTriangles(),
                    gvs::SetParent(root_id));

    scene->add_item(gvs::SetPositions3d({-1.f, -1.f, -2.f, 2.f, -1.f, -2.f, -1.f, 2.f, -2.f, 2.f, 2.f, -2.f}),
                    gvs::SetTriangleStrip(),
                    gvs::SetParent(root_id));

    std::vector<float> sphere;
    {
        float                                 u, theta, coeff;
        std::mt19937                          gen{std::random_device{}()};
        std::uniform_real_distribution<float> u_dist(-1.f, 1.f);
        std::uniform_real_distribution<float> theta_dist(0.f, 2.f * pi);

        for (int i = 0; i < 5000; ++i) {
            u     = u_dist(gen);
            theta = theta_dist(gen);

            coeff = std::sqrt(1.f - u * u);

            sphere.emplace_back(coeff * std::cos(theta));
            sphere.emplace_back(coeff * std::sin(theta));
            sphere.emplace_back(u);
        }
    }

    scene->add_item(gvs::SetReadableId("Sphere"),
                    gvs::SetPositions3d(sphere),
                    gvs::SetNormals3d(sphere),
                    gvs::SetTransformation({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -2, 2, 2, 1}),
                    gvs::SetShading(gvs::Shading::Lambertian),
                    gvs::SetColoring(gvs::Coloring::Normals),
                    gvs::SetParent(root_id));
}

auto build_primitive_scene(gvs::scene::Scene* scene, gvs::SceneId const& root_id) -> void {
    scene->add_item(gvs::SetReadableId("Axes"), gvs::SetPrimitive(gvs::Axes{}), gvs::SetParent(root_id));

    gvs::SceneId cube;
    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +4.0f; // x
        translation[13]  = +0.0f; // y
        translation[14]  = +0.0f; // z

        cube = scene->add_item(gvs::SetReadableId("Cube"),
                               gvs::SetPrimitive(gvs::Cube{}),
                               gvs::SetTransformation(translation),
                               gvs::SetUniformColor(blueish),
                               gvs::SetParent(root_id));
    }

    gvs::SceneId sphere;
    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +2.0f; // x
        translation[13]  = +2.0f; // y
        translation[14]  = -2.0f; // z

        sphere = scene->add_item(gvs::SetReadableId("Sphere"),
                                 gvs::SetPrimitive(gvs::Sphere{}),
                                 gvs::SetTransformation(translation),
                                 gvs::SetUniformColor(greenish),
                                 gvs::SetParent(cube));
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = -3.0f; // x
        translation[13]  = -5.0f; // y
        translation[14]  = +0.0f; // z

        scene->add_item(gvs::SetReadableId("Cylinder"),
                        gvs::SetPrimitive(gvs::Cylinder{}),
                        gvs::SetTransformation(translation),
                        gvs::SetUniformColor(redish),
                        gvs::SetParent(sphere));
    }

    gvs::SceneId cone;
    {
        auto translation = gvs::identity_mat4;
        translation[12]  = -2.0f; // x
        translation[13]  = +1.0f; // y
        translation[14]  = +0.0f; // z

        cone = scene->add_item(gvs::SetReadableId("Cone"),
                               gvs::SetPrimitive(gvs::Cone{}),
                               gvs::SetTransformation(translation),
                               gvs::SetParent(root_id));
    }

    {
        auto translation = gvs::identity_mat4;
        translation[12]  = +0.0f; // x
        translation[13]  = +2.0f; // y
        translation[14]  = +0.0f; // z

        scene->add_item(gvs::SetReadableId("Plane"),
                        gvs::SetPrimitive(gvs::Plane{}),
                        gvs::SetTransformation(translation),
                        gvs::SetUniformColor(orange),
                        gvs::SetParent(cone));
    }
}

} // namespace example