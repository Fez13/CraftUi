#include "app.h"

#include "../entities/entity.h"
#include "../entities/extensions/extension.h"
#include "../entities/extensions/generic_extensions.h"
#include "../entities/extensions/extension_labels.h"
#include "../examples/glfw/glfw_examples.hpp"
#include "../examples/vulkan/vulkan_examples.hpp"
namespace cui {


void App::clean() {}

using namespace entities::extensions;
using namespace entities;

bool test() {std::cout<<"a"<<'\n'; return true;}

exit_state App::execute() { return CUI_EXIT_CLOSE; }
void App::initialize() {
  LOG("App initialization started.", TEXT_COLOR_NOTIFICATION);
  GET_ELAPSED();

  vulkan::initialize_vulkan();
  // vulkan::example::test_vulkan();

  glfw::initialize_glfw();
  // glfw::example::test_glfw();

  entity* new_entt = entity::create();
  
  transform* new_trf = attach_extension<transform>(new_entt);
  transform* get_new_trf= new_entt->get_extension<transform>();
  

  glfw::glfw_window window;
  window.initialize(800, 600, 60);

  renderer::renderer::get().initialize(
      static_cast<renderer::window *>(&window));

  LOG("Close the window to finish the window test.", TEXT_COLOR_NOTIFICATION);
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
