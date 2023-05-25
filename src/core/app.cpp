#include "app.h"


#include "../examples/vulkan/vulkan_examples.hpp"
#include "../examples/glfw/glfw_examples.hpp"

namespace cui {

void App::clean() {}

exit_state App::execute() { return CUI_EXIT_CLOSE; }
void App::initialize() {
  LOG("App initialization started.", TEXT_COLOR_NOTIFICATION);
  GET_ELAPSED();
  
  vulkan::initialize_vulkan();
  //vulkan::example::test_vulkan();  
  
  glfw::initialize_glfw();
  //glfw::example::test_glfw();

  glfw::glfw_window window;
  window.initialize(800,600,60);
  
  renderer::renderer::get().initialize(static_cast<renderer::window*>(&window));
  
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

  LOG("App initialization finished at : " + std::to_string(GET_ELAPSED()),
      TEXT_COLOR_NOTIFICATION);
}
} // namespace cui
