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

int main(int argc, char* argv[]) {
    std::string server_address;

    if (argc > 1) {
        server_address = argv[1];
    }

    using namespace std::chrono_literals;

    gvs::log::GeometryLogger scene(server_address, 3s);

    gvs::log::GeometryItemStream stream = scene.item_stream() << gvs::positions_3d({}) << gvs::send;
    CHECK(stream);

    gvs::log::GeometryItemStream blah_stream = scene.item_stream("blah") << gvs::positions_3d({});

    stream << gvs::positions_3d({}) << gvs::normals_3d({}) << gvs::tex_coords_3d({}) << gvs::vertex_colors_3d({});
    stream << gvs::indices<gvs::proto::GeometryFormat::TRIANGLE_FAN>({}) << gvs::replace;
    CHECK(stream);

    stream << gvs::points({}) << gvs::append;
    CHECK(stream);

    stream << gvs::line_strip({}) << gvs::append;
    CHECK(stream);

    blah_stream << gvs::replace;
    CHECK(blah_stream);
}
