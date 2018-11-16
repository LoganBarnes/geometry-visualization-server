// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
// ///////////////////////////////////////////////////////////////////////////////////////
#include "imgui_utils.hpp"

#include <imgui.h>
#include <imgui_internal.h>

namespace imgui {

Disable::Guard::Guard(bool disable) : disable_(disable) {
    if (disable_) {
        disable_push();
    }
}

Disable::Guard::~Guard() {
    if (disable_) {
        disable_pop();
    }
}

void Disable::disable_push() {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
}

void Disable::disable_pop() {
    ImGui::PopStyleVar();
    ImGui::PopItemFlag();
}

} // namespace imgui
