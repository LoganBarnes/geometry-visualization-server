// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <imgui.h>
#include <string>

namespace gvs {
namespace vis {
namespace detail {

class Theme {
public:
    Theme();

    // utility functions
    static ImColor parse_color(int rgb_hex);

    // set ImGui::Style colors
    void set_style();

    // background color, use with glClear for example
    ImColor background;

    // font, use with ImGui::PushFont/PopFont
    ImFont* font;
};

} // namespace detail
} // namespace vis
} // namespace gvs
