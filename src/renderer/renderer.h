#pragma once
#include "../core/macros.h"
#include "../utils/utils.hpp"
#include "../vulkan/vk_utils.h"
#include "../vulkan/vulkan.h"
#include "pipelines/pipeline_label.h"
#include "scene.h"
#include "window.h"

namespace cui::renderer {

using namespace pipeline_info;

struct default_buffer_data {
  int window_height;
  int window_width;
  float time;
  int frame_rate;
  float random_number;
};

class renderer {
public:
  template <class pipeline>
  pipeline *
  create_draw_pipeline(util::optional_parameter<const std::string> fragment,
                       util::optional_parameter<const std::string> vertex,
                       const pipeline_label label) {
    m_ubiquitous_pipelines[label] = std::make_unique<pipeline>();
    vulkan::vk_rasterization_pipeline *new_pipeline =
        m_ubiquitous_pipelines[label].get();

    new_pipeline->m_descriptor_sets = vulkan::vk_descriptor_set_array(m_device);
    new_pipeline->set_vulkan_device(m_device);
    new_pipeline->set_fragment_shader(fragment.get_copy_else("TODO"));
    new_pipeline->set_vertex_shader(vertex.get_copy_else("TODO"));

    new_pipeline->set_dynamic_states(
        {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
    new_pipeline->set_render_pass(m_draw_render_pass);

    //Pipeline descriptors
    vulkan::vk_descriptor_set *dst = new_pipeline->m_descriptor_sets.create();
    dst->create_binding_uniform_buffer(0,
                                       VK_SHADER_STAGE_ALL); // Default buffer
    dst->create_binding_uniform_buffer(1, VK_SHADER_STAGE_ALL); // Transforms
    
    new_pipeline->set_viewport_state(&m_universal_view_port_state);
    new_pipeline->initialize();
    new_pipeline->initialize_buffers();
    
    return static_cast<pipeline *>(new_pipeline);
  }

  template <class pipeline>
  pipeline *
  create_shadow_pipeline(util::optional_parameter<const std::string> fragment,
                         util::optional_parameter<const std::string> vertex,
                         const pipeline_label label) {}

  renderer(renderer &) = delete;

  void initialize(window *window);

  /*
  @brief Will call draw on default pipelines.
  */
  // TODO
  void draw();

  /*
  @brief Will call draw on occasional pipelines as shadow mapping or one draw
  pipelines.
  */
  // TODO
  void update_occasional_pipelines() {}

  /*
  @brief Adapts pipelines and images to swap_chain sizes.
  */
  void update_extents();

  void create_render_passes();

  static renderer &get() { return s_instance; }

private:
  /*
    Default buffers are:


  */

  // TODO
  void create_semaphore() {
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    vkCreateSemaphore(m_device->get_device(), &semaphoreCreateInfo, nullptr,
                      &m_image_wait);
    vkCreateSemaphore(m_device->get_device(), &semaphoreCreateInfo, nullptr,
                      &m_render_wait);
  }

  glm::vec4 m_clear_color = {1, 1, 1, 1};

  void record_command_buffer(const uint32_t index);

  void begin_render_pass(const uint32_t index);

  renderer() = default;

  void update_view_port();

  std::unordered_map<pipeline_label,
                     std::unique_ptr<vulkan::vk_rasterization_pipeline>>
      m_ubiquitous_pipelines;

  std::unordered_map<pipeline_label,
                     std::unique_ptr<vulkan::vk_rasterization_pipeline>>
      m_occasional_pipelines;

  // Avoid requesting device for speed.
  vulkan::vk_device *m_device;

  // Avoid getting queue via string for speed.
  vulkan::queue m_graphic_queue;

  vulkan::vk_image m_depth_image;

  window *m_window;
  vulkan::vk_swap_chain *m_swap_chain;

  VkRenderPass m_draw_render_pass;
  VkRenderPass m_shadow_render_pass;

  VkViewport m_view_port;
  VkRect2D m_scissor;

  VkPipelineViewportStateCreateInfo m_universal_view_port_state;

  // TODO: initialize
  VkSemaphore m_image_wait;
  VkSemaphore m_render_wait;

  VkCommandBuffer m_command_buffer;

  static renderer s_instance;
};

} // namespace cui::renderer
