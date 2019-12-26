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
#include "gvs/display/geometry_display.hpp"

#include <cmath>
#include <random>

int main() {

    std::unique_ptr<gvs::log::GeometryLogger> scene = std::make_unique<gvs::display::GeometryDisplay>();

    scene->clear_all_items();

    {
        auto stream = scene->item_stream("Axes")
            << gvs::SetPositions3d({{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}})
            << gvs::SetVertexColors3d({{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f}})
#if 1
            << gvs::SetColoring(gvs::Coloring::VertexColors) << gvs::SetLines({0, 1, 0, 2, 0, 3})
#else
            << gvs::SetColoring(gvs::Coloring::UniformColor) << gvs::SetLines({0, 1, 0, 2, 0, 3})
#endif
            << gvs::send;
        CHECK_WITH_PRINT(stream);
    }

    std::vector<gvs::vec3> circle;
    constexpr int max_verts = 50;
    for (int i = 0; i <= max_verts; ++i) {
        float angle = float(M_PI) * static_cast<float>(i) * 2.f / max_verts;
        circle.push_back({std::cos(angle), std::sin(angle), 0.f});
    }

    // Snowman
    {
        // clang-format off
        auto transform = gvs::mat4{
            0.75f, 0.f,   0.f,   0.f,
            0.f,   0.75f, 0.f,   0.f,
            0.f,   0.f,   0.75f, 0.f,
            2.f,   2.f,   1.f,   1.f,
        };
        // clang-format on

        auto stream = scene->item_stream("Head") << gvs::SetPositions3d(circle) << gvs::SetTransformation(transform)
                                                 << gvs::SetUniformColor({1.f, 0.5f, 1.f}) << gvs::SetLineStrip()
                                                 << gvs::SetShading(gvs::UniformColor{}) << gvs::replace;
        CHECK_WITH_THROW(stream);
    }

    {
        // clang-format off
        auto scale_trans = gvs::mat4{
            1.3f, 0.f,  0.f,  0.f,
            0.f,  1.3f, 0.f,  0.f,
            0.f,  0.f,  1.3f, 0.f,
            0.f, -2.3f, 0.f,  1.f,
        };
        // clang-format on
        auto body_stream = scene->item_stream("Body")
            << gvs::SetParent("Head") << gvs::SetPositions3d(circle) << gvs::SetTransformation(scale_trans)
            << gvs::SetUniformColor({1.f, 1.f, 0.5f}) << gvs::SetLineStrip() << gvs::replace;
        CHECK_WITH_THROW(body_stream);

        auto feet_stream = scene->item_stream("Feet")
            << gvs::SetParent(body_stream.id()) << gvs::SetPositions3d(circle) << gvs::SetTransformation(scale_trans)
            << gvs::SetUniformColor({0.5f, 1.f, 1.f}) << gvs::SetTriangleFan() << gvs::replace;
        CHECK_WITH_THROW(feet_stream);
    }

    gvs::log::GeometryItemStream triangle = scene->item_stream()
        << gvs::SetPositions3d({{-1.f, -1.f, 0.f}, {1.f, -1.f, 0.f}, {0.f, 1.5f, -1.f}})
        << gvs::SetUniformColor({.5f, 0.25f, 0.05f}) << gvs::SetTriangles() << gvs::send;
    CHECK_WITH_PRINT(triangle);

    gvs::log::GeometryItemStream stream2 = scene->item_stream()
        << gvs::SetPositions3d({{-1.f, -1.f, -2.f}, {2.f, -1.f, -2.f}, {-1.f, 2.f, -2.f}, {2.f, 2.f, -2.f}})
        << gvs::SetTriangleStrip() << gvs::send;
    CHECK_WITH_PRINT(stream2);

#if 0
    gvs::log::GeometryItemStream blah_stream = scene->item_stream("blah");

    blah_stream << gvs::SetPositions3d() << gvs::SetNormals3d() << gvs::SetTextureCoordinates3d()
                << gvs::SetVertexColors3d();
    blah_stream << gvs::SetTriangleFan() << gvs::replace;
    CHECK_WITH_PRINT(blah_stream);

    blah_stream << gvs::SetPoints() << gvs::append;
    CHECK_WITH_PRINT(blah_stream);

    blah_stream << gvs::SetLineStrip() << gvs::append;
    CHECK_WITH_PRINT(blah_stream);

    blah_stream << gvs::replace;
    CHECK_WITH_PRINT(blah_stream);
#endif

    std::vector<gvs::vec3> sphere;

    {
        float u, theta, coeff;
        std::mt19937 gen{std::random_device{}()};
        std::uniform_real_distribution<float> u_dist(-1.f, 1.f);
        std::uniform_real_distribution<float> theta_dist(0.f, 2.f * M_PIf32);

        for (int i = 0; i < 5000; ++i) {
            u = u_dist(gen);
            theta = theta_dist(gen);
            coeff = std::sqrt(1.f - u * u);
            sphere.push_back({coeff * std::cos(theta), coeff * std::sin(theta), u});
        }
    }

    CHECK_WITH_PRINT(scene->item_stream("sphere").send(gvs::SetPositions3d(sphere),
                                                       gvs::SetNormals3d(sphere),
                                                       // clang-format off
                                                       gvs::SetTransformation ({
                                                            1, 0, 0, 0,
                                                            0, 1, 0, 0,
                                                            0, 0, 1, 0,
                                                           -2, 2, 2, 1,
                                                       }),
                                                       // clang-format on
                                                       gvs::SetShading(gvs::LambertianShading()),
                                                       gvs::SetColoring(gvs::Coloring::Normals)));
}
