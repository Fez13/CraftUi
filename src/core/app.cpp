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
  vulkan::example::test_vulkan();  
  
  glfw::initialize_glfw();
  glfw::example::test_glfw();


  LOG("App initialization finished at : " + std::to_string(GET_ELAPSED()),
      TEXT_COLOR_NOTIFICATION);
}
} // namespace cui
