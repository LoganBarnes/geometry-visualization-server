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
#pragma once

// project
#include "gvs/util/error.hpp"

// standard
#include <list>
#include <string>

namespace gvs::gui {

enum class LogToConsole { yes, no };

class ErrorAlert {
public:
    explicit ErrorAlert(const std::string& unique_label, LogToConsole log_debug_errors_to_console = LogToConsole::yes);

    /**
     * @brief Display an ImGui window popup with the oldest error message (if one exists)
     */
    auto display_next_error() -> void;

    [[nodiscard]] auto should_be_open() const -> bool;

    auto record_error(util::Error error) -> void;
    auto record_warning(util::Error error) -> void;

private:
    std::string imgui_label_;
    bool        log_errors_to_console_;

    enum class Type {
        error,
        warning,
    };
    struct Message {
        util::Error error;
        Type        type = Type::error;
    };
    std::list<Message> error_messages_;

    auto display(const Type& type, const std::string& error_message) -> bool;
};

} // namespace gvs::gui
