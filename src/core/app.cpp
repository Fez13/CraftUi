#include "app.h"

#include "../entities/entity.h"
#include "../entities/extensions/extension.h"
#include "../entities/extensions/extension_labels.h"
#include "../entities/extensions/generic_extensions.h"
#include "../renderer/mesh.h"
#include "../renderer/pipelines/rasterization_pipeline.h"

#include "../examples/glfw/glfw_examples.hpp"
#include "../examples/vulkan/vulkan_examples.hpp"

namespace cui {

void App::clean() {}

using namespace entities::extensions;
using namespace entities;
using namespace renderer;

bool test() {
  std::cout << "a" << '\n';
  return true;
}

exit_state App::execute() { return CUI_EXIT_CLOSE; }
void App::initialize() {
  LOG("App initialization started.", TEXT_COLOR_NOTIFICATION);
  GET_ELAPSED();

  vulkan::initialize_vulkan();
  // vulkan::example::test_vulkan();

  glfw::initialize_glfw();
  // glfw::example::test_glfw();

  glfw::glfw_window window;
  window.initialize(800, 600, 60);

  renderer::renderer::get().initialize(
      static_cast<renderer::window *>(&window));

  create_default_scene();

  entity *new_entt = entity::create();

  mesh_manager new_mesh_manager;

  geometry_2d m_new_geometry(
      {vertex_2d({0, 0}, {1, 1}), vertex_2d({0, 1}, {1, 1}),
       vertex_2d({1, 0}, {1, 1}), vertex_2d({1, 1}, {1, 1})},
      {0, 1, 3, 0, 2, 3});
  mesh new_mesh;

  new_mesh_manager.create_mesh(&new_mesh, &m_new_geometry);

  new_mesh_manager.initialize(
      vulkan::vk_device_manager::get().get_device("DEVICE_KHR"));

  transform *new_trf = attach_extension<transform>(new_entt);
  drawable *new_drawable = attach_extension<drawable>(new_entt,&new_mesh,nullptr,new_trf);
  
  renderer::renderer::get().create_draw_pipeline<vulkan::vk_rasterization_pipeline>({""},{""},renderer::CUI_PIPELINE_RASTERIZATION_DEFAULT);

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
