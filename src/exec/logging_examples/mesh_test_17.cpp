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
#include "gvs/log/geometry_logger.hpp"

template <typename T, std::size_t N>
struct Vec;

template <typename T, std::size_t N>
struct Vec {
    Vec() : data_{0.f, 0.f, 0.f} {}
    Vec(float x, float y, float z) : data_{x, y, z} {}
    const float& operator[](unsigned i) const { return data_[i]; }
    float& operator[](unsigned i) { return data_[i]; }
    std::array<float, 3> data_;
};
using Vec3 = Vec<float, 3>;

template <typename T, std::size_t N, std::size_t M>
struct Mat;

template <typename T>
struct Mat<T, 4, 4> {
    Mat() : data_{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1} {}
    float* operator[](std::size_t i) { return data_.data() + 4 * i; }
    const float* operator[](std::size_t i) const { return data_.data() + 4 * i; }
    std::array<float, 16> data_;
};

using Mat4 = Mat<float, 4, 4>;

struct Mesh {
    std::vector<Vec3> verts;
    std::vector<Vec3> norms;
    std::vector<unsigned> tris;
};

void make_cube_mesh(Mesh* cube);
void make_cube_mesh(Mesh* cube) {
    cube->verts.resize(6 * 4); // 6 faces, 4 points each
    cube->norms.resize(6 * 4);
    cube->tris.resize(6 * 2 * 3); // 6 faces, 2 triangles each, 3 verts per triangle

    auto add_face = [&](unsigned face, unsigned x_o, unsigned y_o, unsigned z_o, float x_s, float y_s, float z_s) {
        cube->verts[face * 4 + 0][x_o] = -1 * x_s;
        cube->verts[face * 4 + 0][y_o] = -1 * y_s;
        cube->verts[face * 4 + 0][z_o] = 1 * z_s;

        cube->verts[face * 4 + 1][x_o] = 1 * x_s;
        cube->verts[face * 4 + 1][y_o] = -1 * y_s;
        cube->verts[face * 4 + 1][z_o] = 1 * z_s;

        cube->verts[face * 4 + 2][x_o] = -1 * x_s;
        cube->verts[face * 4 + 2][y_o] = 1 * y_s;
        cube->verts[face * 4 + 2][z_o] = 1 * z_s;

        cube->verts[face * 4 + 3][x_o] = 1 * x_s;
        cube->verts[face * 4 + 3][y_o] = 1 * y_s;
        cube->verts[face * 4 + 3][z_o] = 1 * z_s;

        for (unsigned i = 0; i < 4; ++i) {
            cube->norms[face * 4 + i][x_o] = 0 * x_s;
            cube->norms[face * 4 + i][y_o] = 0 * y_s;
            cube->norms[face * 4 + i][z_o] = 1 * z_s;
        }

        cube->tris[face * 6 + 0] = face * 4 + 0;
        cube->tris[face * 6 + 1] = face * 4 + 1;
        cube->tris[face * 6 + 2] = face * 4 + 2;
        cube->tris[face * 6 + 3] = face * 4 + 2;
        cube->tris[face * 6 + 4] = face * 4 + 1;
        cube->tris[face * 6 + 5] = face * 4 + 3;
    };

    add_face(0u, 0u, 1u, 2u, 1.f, 1.f, 1.f); // Z
    add_face(1u, 0u, 1u, 2u, -1.f, 1.f, -1.f); // -Z

    add_face(2u, 2u, 1u, 0u, -1.f, 1.f, 1.f); // X
    add_face(3u, 2u, 1u, 0u, 1.f, 1.f, -1.f); // -X

    add_face(4u, 0u, 2u, 1u, 1.f, -1.f, 1.f); // Y
    add_face(5u, 0u, 2u, 1u, 1.f, 1.f, -1.f); // Y
}

void add_translated_mesh_to_scene(const gvs::log::GeometryLogger& scene, const Mesh& mesh, float x, float y, float z);
void add_translated_mesh_to_scene(const gvs::log::GeometryLogger& scene, const Mesh& mesh, float x, float y, float z) {
    auto cube_stream = scene.item_stream();

    Mat4 trans;
    trans[3][0] = x;
    trans[3][1] = y;
    trans[3][2] = z;

    cube_stream << gvs::positions_3d(mesh.verts) << gvs::normals_3d(mesh.norms) << gvs::triangles(mesh.tris)
                << gvs::transformation(trans) << gvs::shading(gvs::LambertianShading{{-1.f, -2.f, -3.f}}) << gvs::send;
    CHECK_WITH_PRINT(cube_stream);
}

int main(int argc, char* argv[]) {
    std::string server_address = "0.0.0.0:50055";

    if (argc > 1) {
        server_address = argv[1];
    }

    using namespace std::chrono_literals;

    gvs::log::GeometryLogger scene(server_address, 3s);
    scene.clear_all_items();

    {
        auto stream = scene.item_stream("Axes")
            << gvs::positions_3d({0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f})
            << gvs::vertex_colors_3d({1.f, 1.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 1.f})
            << gvs::coloring(gvs::proto::Coloring::VERTEX_COLORS) << gvs::lines({0, 1, 0, 2, 0, 3}) << gvs::replace;
        CHECK_WITH_PRINT(stream);
    }

    Mesh cube_mesh;
    make_cube_mesh(&cube_mesh);

    add_translated_mesh_to_scene(scene, cube_mesh, -2.1f, 0.f, 0.f);
    add_translated_mesh_to_scene(scene, cube_mesh, 2.1f, 0.f, 0.f);
    add_translated_mesh_to_scene(scene, cube_mesh, 0.f, -2.1f, 0.f);
    add_translated_mesh_to_scene(scene, cube_mesh, 0.f, 2.1f, 0.f);
    add_translated_mesh_to_scene(scene, cube_mesh, 0.f, 0.f, -2.1f);
    add_translated_mesh_to_scene(scene, cube_mesh, 0.f, 0.f, 2.1f);
}
