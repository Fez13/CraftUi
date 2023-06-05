#pragma once
#include "../../vulkan/vulkan.h"
#include "../vertex.h"

namespace cui::renderer {

using namespace vulkan;

class default_rasterization_2d : public vk_rasterization_pipeline {

public:
  void initialize(vk_device *device) override;
  
  void free() override;
  
  void render(VkCommandBuffer &cmd, VkRenderPass &render_pass) override;
  
  void set_view_port_state(const VkPipelineViewportStateCreateInfo
                                       &universal_view_port_state_create_info) override;
  void update_size() override;

private:
  
  vk_descriptor_set *m_descriptor;
  
  void initialize_buffers() override;
  
  void initialize_default_mesh() override;
  
};

} // namespace cui::renderer
