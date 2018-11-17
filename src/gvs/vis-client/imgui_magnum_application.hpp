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
#pragma once

#include "gvs/forward_declarations.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/Platform/GlfwApplication.h>

#include <memory>

struct ImGuiContext;

namespace gvs {
namespace vis {

class ImGuiMagnumApplication : public Magnum::Platform::Application {
public:
    explicit ImGuiMagnumApplication(const Arguments& arguments, const Configuration& configuration);
    explicit ImGuiMagnumApplication(const Arguments& arguments);
    virtual ~ImGuiMagnumApplication();

protected:
    std::unique_ptr<detail::Theme> theme_;

private:
    virtual void update() = 0;
    virtual void render() const = 0;
    virtual void configure_gui() = 0;

    void drawEvent() override;

    // keyboard
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void textInputEvent(TextInputEvent& event) override;

    // mouse
    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;
    void mouseScrollEvent(MouseScrollEvent& event) override;

    // Ensures the application renders at least 5 more times after all events are
    // finished to give ImGui a chance to update and render correctly
    void reset_draw_counter();

    std::shared_ptr<ImGuiContext> imgui_;
    std::shared_ptr<bool> imgui_gl_;
    int draw_counter_; // continue drawing until this counter is zero
};

} // namespace vis
} // namespace gvs
