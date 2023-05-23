#include "glfw.h"

namespace cui::glfw {

static bool glfw_initialized = false;

bool is_glfw_initialized() { return glfw_initialized; }

void initialize_glfw() {
  if (glfw_initialized) {
    LOG("Glfw has already been initialized", TEXT_COLOR_WARNING);
    return;
  }
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfw_initialized = true;
}

} // namespace cui::glfw
