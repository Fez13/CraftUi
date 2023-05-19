#pragma once

#include "vk_descriptor_set.h"
#include "vk_utils.h"

namespace cui::vulkan {

// This is a virtual class for a compute pipeline.
class vk_compute_pipeline {
public:
  virtual void initialize(vk_device *device);
  virtual void free();
  virtual void compute(VkCommandBuffer &cmd);

private:
  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device;
};


//This is a virtual class for a rasterization pipeline.
class vk_rasterization_pipeline {
public:
  virtual void initialize(vk_device *device);
  virtual void free();
  virtual void render(VkCommandBuffer &cmd,VkRenderPass& render_pass);

private:
  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device;
};

} // namespace cui::vulkan
