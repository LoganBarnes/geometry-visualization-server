#pragma once

// ImGui uses gl3w.h to access OpenGL in its OpenGL3/GLFW implementation.
// This project uses Magnum to handle OpenGL. To avoid copying and maintaining
// an entire ImGui class in order to replace a single header, we simply wrap
// the desired OpenGL header in a file with the same 'gl3w.h' name.

#include <Magnum/GL/OpenGL.h>
