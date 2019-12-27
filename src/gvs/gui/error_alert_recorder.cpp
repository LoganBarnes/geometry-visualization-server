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
#include "error_alert_recorder.hpp"

// project
#include "error_alert.hpp"

namespace gvs::gui {

ErrorAlertRecorder::ErrorAlertRecorder(const std::shared_ptr<ErrorAlert>& popup) : error_alert_(popup) {}

auto ErrorAlertRecorder::record_error(util::Error error) const -> bool {
    if (auto popup = error_alert_.lock()) {
        popup->record_error(std::move(error));
        return true;
    }
    return false;
}

auto ErrorAlertRecorder::record_warning(util::Error error) const -> bool {
    if (auto popup = error_alert_.lock()) {
        popup->record_warning(std::move(error));
        return true;
    }
    return false;
}

} // namespace gvs::gui
