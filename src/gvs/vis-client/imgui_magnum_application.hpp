// ///////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2018 Logan Barnes - All Rights Reserved
//  Geometry Visualisation Server
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
