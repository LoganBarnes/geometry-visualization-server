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
#include "imgui_utils.hpp"

// external
#include <imgui_internal.h>

// standard
#include <algorithm>

namespace gvs::gui {

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

bool configure_gui(const std::string& label, std::string* data, ImGuiInputTextFlags_ const& flags) {
    char buf[1024];

    std::size_t max_size = std::min(sizeof(buf) - 1u, data->size());
    data->copy(buf, max_size);
    buf[max_size] = '\0';

    bool value_changed = ImGui::InputText(label.c_str(), buf, sizeof(buf), flags);

    *data = buf;

    return value_changed;
}

ScopedID::ScopedID(const char* str_id) {
    ImGui::PushID(str_id);
}

ScopedID::ScopedID(const char* str_id_begin, const char* str_id_end) {
    ImGui::PushID(str_id_begin, str_id_end);
}

ScopedID::ScopedID(const void* ptr_id) {
    ImGui::PushID(ptr_id);
}

ScopedID::ScopedID(int int_id) {
    ImGui::PushID(int_id);
}

ScopedID::~ScopedID() {
    ImGui::PopID();
}

} // namespace gvs::gui
