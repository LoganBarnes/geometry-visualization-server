// ///////////////////////////////////////////////////////////////////////////////////////
//                                                                           |________|
//  Copyright (c) 2018 CloudNC Ltd - All Rights Reserved                        |  |
//                                                                              |__|
//        ____                                                                .  ||
//       / __ \                                                               .`~||$$$$
//      | /  \ \         /$$$$$$  /$$                           /$$ /$$   /$$  /$$$$$$$
//      \ \ \ \ \       /$$__  $$| $$                          | $$| $$$ | $$ /$$__  $$
//    / / /  \ \ \     | $$  \__/| $$  /$$$$$$  /$$   /$$  /$$$$$$$| $$$$| $$| $$  \__/
//   / / /    \ \__    | $$      | $$ /$$__  $$| $$  | $$ /$$__  $$| $$ $$ $$| $$
//  / / /      \__ \   | $$      | $$| $$  \ $$| $$  | $$| $$  | $$| $$  $$$$| $$
// | | / ________ \ \  | $$    $$| $$| $$  | $$| $$  | $$| $$  | $$| $$\  $$$| $$    $$
//  \ \_/ ________/ /  |  $$$$$$/| $$|  $$$$$$/|  $$$$$$/|  $$$$$$$| $$ \  $$|  $$$$$$/
//   \___/ ________/    \______/ |__/ \______/  \______/  \_______/|__/  \__/ \______/
//
// ///////////////////////////////////////////////////////////////////////////////////////
#include "gvs/vis-client/scene.hpp"
#include "scene.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/CompressIndices.h>
#include <Magnum/MeshTools/Interleave.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/Trade/MeshData3D.h>

namespace gvs {
namespace vis {

using namespace Magnum;
using namespace Math::Literals;

Scene::Scene() {
    camera_object_.setParent(&scene_).translate(Vector3::zAxis(5.0f));

    (*(camera_ = new SceneGraph::Camera3D{camera_object_})) // Memory control is handled elsewhere
        .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
        .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
        .setViewport(GL::defaultFramebuffer.viewport().size());

    root_object_.setParent(&scene_);

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
    colored_shader_.setAmbientColor(0x111111_rgbf).setSpecularColor(0xffffff_rgbf).setShininess(80.0f);

    // Temp cube mesh stuff
    const Trade::MeshData3D cube = Primitives::cubeSolid();

    vertex_buffer_.setData(MeshTools::interleave(cube.positions(0), cube.normals(0)));

    Containers::Array<char> index_data;
    MeshIndexType index_type;
    UnsignedInt index_start, index_end;
    std::tie(index_data, index_type, index_start, index_end) = MeshTools::compressIndices(cube.indices());
    index_buffer_.setData(index_data);

    mesh_.setPrimitive(cube.primitive())
        .setCount(static_cast<int>(cube.indices().size()))
        .addVertexBuffer(vertex_buffer_, 0, Shaders::Phong::Position{}, Shaders::Phong::Normal{})
        .setIndexBuffer(index_buffer_, 0, index_type, index_start, index_end);
}

void Scene::render(const Magnum::Vector2i& viewport) {

    auto transformation = Matrix4::rotationX(30.0_degf) * Matrix4::rotationY(40.0_degf);
    auto projection = Matrix4::perspectiveProjection(35.0_degf, Vector2{viewport}.aspectRatio(), 0.01f, 100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));
    auto color = Color3::fromHsv(35.0_degf, 1.0f, 1.0f);

    shader_.setLightPosition({7.0f, 5.0f, 2.5f})
        .setLightColor(Color3{1.0f})
        .setDiffuseColor(color)
        .setAmbientColor(Color3::fromHsv(color.hue(), 1.0f, 0.3f))
        .setTransformationMatrix(transformation)
        .setNormalMatrix(transformation.rotationScaling())
        .setProjectionMatrix(projection);

    mesh_.draw(shader_);
}

void Scene::configure_gui(const Magnum::Vector2i& /*viewport*/) {}

} // namespace vis
} // namespace gvs
