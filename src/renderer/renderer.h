#pragma once
#include "../core/macros.h"
#include "../vulkan/vulkan.h"
#include "../vulkan/vk_utils.h"
#include "window.h"
#include "pipelines/pipeline_label.h"

namespace cui::renderer {

using namespace pipeline_info;
class renderer {
public:
  renderer(renderer &) = delete;
  void initialize(window *window);

  /*
  @brief Will call draw on default pipelines.
  */ 
  //TODO
  void draw(){};

  /*
  @brief Will call draw on occasional pipelines as shadow mapping or one draw pipelines.
  */
  //TODO
  void update_occasional_pipelines();
  
  /*
  @brief Adapts pipelines and images to swap_chain sizes.
  */
  void update_extents();

  void create_render_passes();

  static renderer &get() { return s_instance; }

private:
  renderer() = default;

  void update_view_port();

  std::unordered_map<pipeline_id,
                     std::unique_ptr<vulkan::vk_rasterization_pipeline>>
      m_ubiquitous_pipelines;

  std::unordered_map<pipeline_id,
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
  
  static renderer s_instance;
};

} // namespace cui::renderer
