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
#include "error_alert.hpp"

// project
#include "imgui_utils.hpp"

// standard
#include <iostream>

namespace gvs::gui {

ErrorAlert::ErrorAlert(const std::string& unique_label, LogToConsole log_errors_to_console)
    : imgui_label_("Error popup###" + unique_label),
      log_errors_to_console_(log_errors_to_console == LogToConsole::yes) {}

auto ErrorAlert::display_next_error() -> void {
    if (error_messages_.empty()) {
        return;
    }
    const auto& error = error_messages_.front().error;
    const auto& type  = error_messages_.front().type;

    if (display(type, error.error_message())) {
        error_messages_.pop_front();
    }
}

auto ErrorAlert::should_be_open() const -> bool {
    return !error_messages_.empty();
}

auto ErrorAlert::record_error(util::Error error) -> void {
    if (log_errors_to_console_) {
        std::cerr << "ERROR: " << error.debug_error_message() << std::endl;
    }
    error_messages_.push_back({std::move(error), Type::error});
}

auto ErrorAlert::record_warning(util::Error error) -> void {
    if (log_errors_to_console_) {
        std::cout << "WARNING: " << error.debug_error_message() << std::endl;
    }
    error_messages_.push_back({std::move(error), Type::warning});
}

auto ErrorAlert::display(const gvs::gui::ErrorAlert::Type& type, const std::string& error_message) -> bool {
    const auto red    = ImVec4{1.f, 0.f, 0.f, 1.f};
    const auto yellow = ImVec4{1.f, 1.f, 0.f, 1.f};

    ScopedID id(imgui_label_.c_str());

    bool popup_closed = false;

    /*
     * Error pop-up (Only shown if there are errors obvs)
     */
    if (!error_message.empty()) {
        ImGui::OpenPopup(imgui_label_.c_str());
    }

    if (ImGui::BeginPopupModal(imgui_label_.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        switch (type) {
        case Type::error:
            ImGui::TextColored(red, "ERROR: %s", error_message.c_str());
            break;
        case Type::warning:
            ImGui::TextColored(yellow, "WARNING: %s", error_message.c_str());
            break;
        }

        if (ImGui::Button("Continue")) {
            popup_closed = true;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }

    return popup_closed;
}

} // namespace gvs::gui
