#pragma once
#include "../../glfw/glfw.h"
#include "../../vulkan/vulkan.h"
#include "../../vulkan_config/vulkan_config.hpp"

namespace cui::glfw::example {
#ifdef TESTING

static inline void glfw_window_test() {

  glfw::glfw_window window;
  window.initialize(800,600,60);
  LOG("Close the window to finish the window test.",TEXT_COLOR_NOTIFICATION);
  while (!glfwWindowShouldClose(window.get_glfw_window())) {

    std::chrono::duration<double, std::milli> delta_ms(160);
    auto delta_ms_duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(delta_ms_duration.count()));

    glfwPollEvents();
  }
  window.free();

  LOG("\t glfw_window / vk_swap_chain completed.", TEXT_COLOR_NOTIFICATION);
}

static inline void test_glfw() {
  LOG("GLFW testing has started.", TEXT_COLOR_WARNING);

  try {
    if (!vulkan::is_vulkan_initialized())
      vulkan::initialize_vulkan();
    if (!is_glfw_initialized())
      initialize_glfw();

    glfw_window_test();

  } catch (std::runtime_error error) {
    LOG("GLFW testing ended with error: " + error.what(), TEXT_COLOR_ERROR);
  }
}

#endif

} // namespace cui::glfw::example
