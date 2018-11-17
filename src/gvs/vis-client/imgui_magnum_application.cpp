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
#include "gvs/vis-client/imgui_magnum_application.hpp"
#include "gvs/vis-client/imgui_theme.hpp"
#include "imgui_magnum_application.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Magnum/GL/Context.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Corrade/Utility/Unicode.h>

namespace gvs {
namespace vis {

ImGuiMagnumApplication::ImGuiMagnumApplication(const Magnum::Platform::GlfwApplication::Arguments& arguments,
                                               const Magnum::Platform::GlfwApplication::Configuration& configuration)
    : GlfwApplication(arguments, configuration) {

    this->startTextInput(); // allow for text input callbacks

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    imgui_ = std::shared_ptr<ImGuiContext>(ImGui::CreateContext(), ImGui::DestroyContext);

    ImGuiIO& io = ImGui::GetIO();

    // Use a shared pointer to handle ImGui initialization and destruction via RAII
    auto init_imgui_gl = [this]() {
        bool success = true;
        success &= ImGui_ImplGlfw_InitForOpenGL(this->window(), false);
        success &= ImGui_ImplOpenGL3_Init();
        return new bool(success);
    };

    imgui_gl_ = std::shared_ptr<bool>(init_imgui_gl(), [](auto* p) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        delete p;
    });

    // Some fonts are loaded in the theme class but we might also want the default so it's loaded here
    io.Fonts->AddFontDefault();
    theme_ = std::make_unique<detail::Theme>();

    theme_->set_style();
    Magnum::GL::Renderer::setClearColor({theme_->background.Value.x,
                                         theme_->background.Value.y,
                                         theme_->background.Value.z,
                                         theme_->background.Value.w});

    reset_draw_counter();
}

ImGuiMagnumApplication::ImGuiMagnumApplication(const Arguments& arguments)
    : ImGuiMagnumApplication(arguments,
                             Configuration{}
                                 .setTitle("ImGui Magnum Application")
                                 .setWindowFlags(Configuration::WindowFlag::Resizable)) {}

ImGuiMagnumApplication::~ImGuiMagnumApplication() = default;

void ImGuiMagnumApplication::drawEvent() {
    update();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::PushFont(theme_->font);
    configure_gui();
    ImGui::PopFont();

    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    swapBuffers();

    if (draw_counter_-- > 0) {
        redraw();
    }
}

void ImGuiMagnumApplication::keyPressEvent(Magnum::Platform::GlfwApplication::KeyEvent& event) {
    if (not event.isRepeated()) {
        ImGui_ImplGlfw_KeyCallback(this->window(),
                                   static_cast<int>(event.key()),
                                   -1,
                                   GLFW_PRESS,
                                   static_cast<int>(event.modifiers()));
    }
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::keyReleaseEvent(Magnum::Platform::GlfwApplication::KeyEvent& event) {
    ImGui_ImplGlfw_KeyCallback(this->window(),
                               static_cast<int>(event.key()),
                               -1,
                               GLFW_RELEASE,
                               static_cast<int>(event.modifiers()));

    using KE = Magnum::Platform::GlfwApplication::KeyEvent;

    // Ctrl + Shift + Q == Exit
    if (event.modifiers() & KE::Modifier::Ctrl and event.modifiers() & KE::Modifier::Shift
        and event.key() == KE::Key::Q) {

        this->exit();
    }

    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::textInputEvent(Magnum::Platform::GlfwApplication::TextInputEvent& event) {
    char32_t codepoint;
    std::tie(codepoint, std::ignore) = Magnum::Utility::Unicode::nextChar(event.text(), 0);

    ImGui_ImplGlfw_CharCallback(this->window(), codepoint);

    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::mousePressEvent(MouseEvent& event) {
    ImGui_ImplGlfw_MouseButtonCallback(this->window(),
                                       static_cast<int>(event.button()),
                                       GLFW_PRESS,
                                       static_cast<int>(event.modifiers()));
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::mouseReleaseEvent(MouseEvent& event) {
    ImGui_ImplGlfw_MouseButtonCallback(this->window(),
                                       static_cast<int>(event.button()),
                                       GLFW_RELEASE,
                                       static_cast<int>(event.modifiers()));
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::mouseMoveEvent(MouseMoveEvent& event) {
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::mouseScrollEvent(Magnum::Platform::GlfwApplication::MouseScrollEvent& event) {
    ImGui_ImplGlfw_ScrollCallback(this->window(), event.offset().x(), event.offset().y());
    event.setAccepted(true);
    reset_draw_counter();
}

void ImGuiMagnumApplication::reset_draw_counter() {
    draw_counter_ = 5;
    redraw();
}

} // namespace vis
} // namespace gvs
