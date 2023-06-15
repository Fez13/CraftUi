#include "app.h"

#include "../entities/entity.h"
#include "../entities/extensions/extension.h"
#include "../entities/extensions/extension_labels.h"
#include "../entities/extensions/generic_extensions.h"
#include "../renderer/mesh.h"
#include "../renderer/pipelines/rasterization_pipeline.h"
#include "../shader_compiler/compile_Spir_V.h"

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
  scene *current_scene = get_current_scene();

  // Compile shaders
  std::string fragment_shader_path =
      "/home/federico/CLionProjects/CraftUi/resources/shaders/compiled/frag/"
      "rasterization_default_2d.spirv";
  std::string vertex_shader_path =
      "/home/federico/CLionProjects/CraftUi/resources/shaders/compiled/vert/"
      "rasterization_default_2d.spirv";

  renderer::spirv::compile("/home/federico/CLionProjects/CraftUi/resources/"
                           "shaders/frag/rasterization_default_2d.frag",
                           renderer::spirv::EMPTY, fragment_shader_path, true);
  renderer::spirv::compile("/home/federico/CLionProjects/CraftUi/resources/"
                           "shaders/vert/rasterization_default_2d.vert",
                           renderer::spirv::EMPTY, vertex_shader_path, true);

  renderer::renderer::get()
      .create_draw_pipeline<renderer::default_rasterization_2d>(
          {fragment_shader_path}, {vertex_shader_path},
          renderer::CUI_PIPELINE_RASTERIZATION_2D_DEFAULT);


  entity *new_entt = entity::create();

  geometry_2d m_new_geometry(
      {vertex_2d({-1, -1}, {1, 1}), vertex_2d({1, -1}, {1, 1}),
       vertex_2d({1, 1}, {1, 1}), vertex_2d({-1, 1}, {1, 1})},
      {0, 1, 2, 2, 3, 0});
  mesh new_mesh;

  current_scene->scene_mesh_manager.create_mesh(&new_mesh, &m_new_geometry);

  current_scene->scene_mesh_manager.initialize(
      vulkan::vk_device_manager::get().get_device("DEVICE_KHR"));

  transform *new_trf = attach_extension<transform>(new_entt);
  new_trf->m_position = glm::vec3{0,0,0};
  new_trf->m_scale = glm::vec3{1,1,1};
  new_trf->m_rotation = glm::vec3{0,0,0};
  new_trf->update_main_matrix();
  drawable *new_drawable =
      attach_extension<drawable>(new_entt, &new_mesh, nullptr, new_trf);
      
  new_drawable->set_pipeline(CUI_PIPELINE_RASTERIZATION_2D_DEFAULT);

  LOG("Close the window to finish the window test.", TEXT_COLOR_NOTIFICATION);
  while (!glfwWindowShouldClose(window.get_glfw_window())) {
    new_drawable->draw();
    std::chrono::duration<double, std::milli> delta_ms(80);
    auto delta_ms_duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(delta_ms_duration.count()));

    renderer::renderer::get().draw();
    glfwPollEvents();
  }
  window.free();

  LOG("App initialization finished at : " + std::to_string(GET_ELAPSED()),
      TEXT_COLOR_NOTIFICATION);
}
} // namespace cui
