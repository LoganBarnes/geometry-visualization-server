Geometry Visualization Server
=============================
[![Travis CI][travis-badge]][travis-link]
[![Codecov][codecov-badge]][codecov-link]
[![MIT License][license-badge]][license-link]
[![Docs][docs-badge]][docs-link]

**[Source][source-code-link]** | **[Documentation][documentation-link]**

### TODO BEFORE MERGE WITH MASTER:
* ~~Create setters and getter types `gvs::SetPositions`, `gvs::GetPositions`, `gvs::HasChildren`, etc.~~
* ~~Use display interfaces in relevant parts of gvs~~
* Add primitive setters `gvs::Cube()`, `gvs::Sphere`, etc.
* Re-implement client/server scene interaction
* Add global scene traits (like lights)
* Clean up magnum scene class
* Create implicitly convertible vector and matrix classes for use with setters and getters
* Move display namespace types to their proper location
* Add Renderable interface
* Implement DisplayScene (maybe)
* Add tests after new architecture is finalized
* Add documentation

A tool for visually debugging geometric applications. Built on a 
[Protobuf][protobuf-link]/[gRPC][grpc-link] service and a 
[Magnum][magnum-link] rendering backend.

The General Idea
----------------

Start the visualization server:

![start-the-server][start-the-server-gif]

In your code, create a scene that is connected to the server:

![connect-to-server][connect-to-server-gif]

Create a geometry stream and send some data to the server:

![send-geometry][send-geometry-gif]

Run your code and view the geometry!

![view-geometry][view-geometry-gif]

Building
--------

GVS probably does not compile on windows or mac. This has not 
been confirmed but the probability seems really low.

### Linux

Required packages for logging only:

```bash
sudo apt install uuid-dev
```

Required packages for visualization:

```bash
sudo apt install xorg-dev libgl1-mesa-dev uuid-dev
```

Scene
-----

### Types

| Name                       | Description                                                          |
| -------------------------- | -------------------------------------------------------------------- |
| `gvs::scene::ClientScene`  | Sends items to an external server for viewing                        |
| `gvs::scene::DisplayScene` | Displays scene items in a window tied to the current process         |
| `gvs::scene::LocalScene`   | Stores items that must be explicitly rendered by the current process |

| Name                       | Use cases                                                                              |
| -------------------------- | -------------------------------------------------------------------------------------- |
| `gvs::scene::ClientScene`  | Repeatedly writing geometry to the same scene during multiple runs of the same process |
| `gvs::scene::DisplayScene` | Quickly displaying geometry that is only relevant within the scope a single process    |
| `gvs::scene::LocalScene`   | For integrating scenes into existing GUI applications                                  |

### Named Parameters

### Required Parameters

All scene items require some form of geometry to be specified.

This can be either a list of positions 

```cpp
    scene.add_item(gvs::SetPositions({{-1, -1, 0}, {1, -1, 0}, {0, 1, 0}}));
```

a predefined primitive

```cpp
    scene.add_item(gvs::SetPrimitive(gvs::Primitive::Cube{}));
```

or a custom renderable (Only available when using `gvs::scene::LocalScene`).

```cpp
    scene.add_item(gvs::SetRenderable(std::make_shared<MySpecialRenderable>()));
```

Providing more or less than one of these items will cause an error. "Geometry-less" items 
can be added by leaving the positions parameter empty:

```cpp
    auto root_item = scene.add_item(gvs::SetPositions(), gvs::SetReadableId("Root"));
```


### Item Options

#### Primitives

| Name                           | Data type               |
| ------------------------------ | ----------------------- |
| `gvs::SetPositions3d`          | `std::vector<Vec3>`     |
| `gvs::SetNormals3d`            | `std::vector<Vec3>`     |
| `gvs::SetTextureCoordinates3d` | `std::vector<Vec2>`     |
| `gvs::SetVertexColors`         | `std::vector<Vec3>`     |
| `gvs::SetIndices`              | `std::vector<unsigned>` |

#### Geometry

| Name                           | Data type               |
| ------------------------------ | ----------------------- |
| `gvs::SetPositions3d`          | `std::vector<Vec3>`     |
| `gvs::SetNormals3d`            | `std::vector<Vec3>`     |
| `gvs::SetTextureCoordinates3d` | `std::vector<Vec2>`     |
| `gvs::SetVertexColors`         | `std::vector<Vec3>`     |
| `gvs::SetIndices`              | `std::vector<unsigned>` |

OR

| Name                           | Data type               |
| ------------------------------ | ----------------------- |
| `gvs::UsePrimitive`            | `gvs::PrimitiveType`    |

where `gvs::PrimitiveType` is one of

| Name                           | Dimensions      |
| ------------------------------ | --------------- |
| `gvs::PrimitiveType::Cone`     |   `1 x 1 x 1`   |
| `gvs::PrimitiveType::Cube`     |   `1 x 1 x 1`   |
| `gvs::PrimitiveType::Cylinder` |   `1 x 1 x 1`   |
| `gvs::PrimitiveType::Sphere`   |   `1 x 1 x 1`   |
| `gvs::PrimitiveType::Torus`    |   `1 x 1 x 1`   |
| `gvs::PrimitiveType::Quad`     |   `1 x 1 x 1`   |

#### Indices Aliases

| Name                    | Geometry Format                      |
| ----------------------- | ------------------------------------ |
| `gvs::SetPoints`        | `gvs::GeometryFormat::Points`        |
| `gvs::SetLines`         | `gvs::GeometryFormat::Lines`         |
| `gvs::SetLineStrip`     | `gvs::GeometryFormat::LineStrip`     |
| `gvs::SetTriangles`     | `gvs::GeometryFormat::Triangles`     |
| `gvs::SetTriangleStrip` | `gvs::GeometryFormat::TriangleStrip` |
| `gvs::SetTriangleFan`   | `gvs::GeometryFormat::TriangleFan`   |

#### Display

| Name                   | Type                    | Default                                        |
| ---------------------- | ----------------------- | ---------------------------------------------- |
| `gvs::geometry_format` | `proto::GeometryFormat` | `proto::GeometryFormat::POINTS`                |
| `gvs::coloring`        | `proto::Coloring`       | `proto::Coloring::UNIFORM_COLOR`               |
| `gvs::transformation`  | `std::array<float, 16>` | 1,0,0,0,<br> 0,1,0,0,<br> 0,0,1,0,<br> 0,0,0,1 |
| `gvs::uniform_color`   | `std::array<float, 3>`  | `{0.9f, 0.8f, 0.7f}`                           |
| `gvs::parent`          | `std::string`           | `""`                                           |
| `gvs::shading`         | See below               | See below                                      |

#### Shading

**Current Options:**

* Uniform color
* Lambertian

**Uniform Color**

Uses the color style defined by `gvs::coloring()`.

**Lambertian**

Uses the color style defined by `gvs::coloring()` but also applies shading using the folowing parameters:

| Parameter       | Default                 |
| --------------- | ----------------------- |
| light direction | `{-1.f, -2.f, -3.f}`    |
| light color     | `{0.85f, 0.85f, 0.85f}` |
| ambient color   | `{0.1f, 0.1f, 0.1f}`    |

### Logging Rules

There are 3 request types that can be used to send data: `gvs::send`, `gvs::replace` (not implemented), 
or `gvs::update` (not implemented). The server behaves differently depending on what data you send and 
which request you use:

| Request Type   | With Positions | Item Exists                     | Item Does Not Exist |
| -------------- |:--------------:| ------------------------------- | ------------------- |
| `gvs::send`    |     **Yes**    | **Error**                       | Creates new item    |
| `gvs::send`    |      *No*      | Updates item                    | **Error**           |
| `gvs::replace` |     **Yes**    | Replaces existing geometry*     | Creates new item    |
| `gvs::replace` |      *No*      | Updates item                    | **Error**           |
| `gvs::append`  |     **Yes**    | Appends positions to geometry** | Creates new item    |
| `gvs::append`  |      *No*      | Updates item                    | **Error**           |

\* Non-geometry info is also updated

** If the geometry format does not that of the existing item, the server 
will return an error. If no error is thrown the non-geometry info will also be updated.


[travis-badge]: https://travis-ci.org/LoganBarnes/geometry-visualization-server.svg?branch=master
[travis-link]: https://travis-ci.org/LoganBarnes/geometry-visualization-server
[codecov-badge]: https://codecov.io/gh/LoganBarnes/geometry-visualization-server/branch/master/graph/badge.svg
[codecov-link]: https://codecov.io/gh/LoganBarnes/geometry-visualization-server
[license-badge]: https://img.shields.io/badge/License-MIT-blue.svg
[license-link]: https://github.com/LoganBarnes/geometry-visualization-server/blob/master/LICENSE
[docs-badge]: https://codedocs.xyz/LoganBarnes/geometry-visualization-server.svg
[docs-link]: https://codedocs.xyz/LoganBarnes/geometry-visualization-server

[protobuf-link]: https://developers.google.com/protocol-buffers/
[grpc-link]: https://grpc.io/
[magnum-link]: https://magnum.graphics/

[source-code-link]: https://github.com/LoganBarnes/geometry-visualization-server
[documentation-link]: https://codedocs.xyz/LoganBarnes/geometry-visualization-server/index.html

[view-geometry-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/view-geometry.gif

[start-the-server-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/start-the-server.gif
[connect-to-server-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/connect-to-server.gif
[send-geometry-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/send-geometry.gif