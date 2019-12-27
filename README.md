Geometry Visualization Server
=============================
[![Travis CI][travis-badge]][travis-link]
[![Codecov][codecov-badge]][codecov-link]
[![MIT License][license-badge]][license-link]
[![Docs][docs-badge]][docs-link]

### TODO BEFORE MERGE WITH MASTER:
* ~~Create setters and getter types `gvs::SetPositions`, `gvs::GetPositions`, `gvs::HasChildren`, etc.~~
* Clean up magnum scene class
* Create implicitly convertible vector and matrix classes for use with setters and getters
* Move display namespace types to their proper location
* Use display interfaces in relevant parts of gvs
* Add tests after new architecture is finalized
* Add global scene traits (like lights)
* Add primitive setters `gvs::Cube()`, `gvs::Sphere`, etc.
* Add Renderable interface
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

Logging
-------

### Item Options

#### Geometry

| Name                           | Data type               |
| ------------------------------ | ----------------------- |
| `gvs::positions_3d`            | `std::vector<float>`    |
| `gvs::normals_3d`              | `std::vector<float>`    |
| `gvs::tex_coords_3d`           | `std::vector<float>`    |
| `gvs::vertex_colors_3d`        | `std::vector<float>`    |
| `gvs::vertex_colors_3d`        | `std::vector<float>`    |
| `gvs::indices<GeometryFormat>` | `std::vector<unsigned>` |

#### Indices Aliases

| Name                  | Index type                                     |
| --------------------- | ---------------------------------------------- |
| `gvs::points`         | `gvs::incices<GeometryFormat::POINTS>`         |
| `gvs::lines`          | `gvs::incices<GeometryFormat::LINES>`          |
| `gvs::line_strip`     | `gvs::incices<GeometryFormat::LINE_STRIP>`     |
| `gvs::triangles`      | `gvs::incices<GeometryFormat::TRIANGLES>`      |
| `gvs::triangle_strip` | `gvs::incices<GeometryFormat::TRIANGLE_STRIP>` |
| `gvs::triangle_fan`   | `gvs::incices<GeometryFormat::TRIANGLE_FAN>`   |

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

[view-geometry-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/view-geometry.gif

[start-the-server-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/start-the-server.gif
[connect-to-server-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/connect-to-server.gif
[send-geometry-gif]: https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/send-geometry.gif