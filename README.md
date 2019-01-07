Geometry Visualization Server
=============================
[![Travis CI][travis-badge]][travis-link]
[![Codecov][codecov-badge]][codecov-link]
[![MIT License][license-badge]][license-link]
[![Docs][docs-badge]][docs-link]

A tool for visually debugging geometric applications. Built on a 
[Protobuf][protobuf-link]/[gRPC][grpc-link] service and a 
[Magnum][magnum-link] rendering backend.

The General Idea
----------------

Start the visualization server:

![start-the-server](https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/start-the-server.gif)

In your code, create a scene that is connected to the server:

![connect-to-server](https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/connect-to-server.gif)

Create a geometry stream and send some data to the server:

![send-geometry](https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/send-geometry.gif)

Run your code and view the geometry!

![view-geometry](https://raw.githubusercontent.com/LoganBarnes/geometry-visualization-server-docs/master/img/view-geometry.gif)

Building
--------

### Linux

Required packages for logging only:

```bash
sudo apt install uuid-dev
```

Required packages for visualization:

```bash
sudo apt install xorg-dev libgl1-mesa-dev uuid-dev
```

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
