#pragma once

#include "vk_descriptor_set.h"
#include "vk_utils.h"
#include "vk_shader.h"

#include<functional>
namespace cui::vulkan {

class vk_compute_pipeline {
public:
  explicit vk_compute_pipeline(vk_device *device) : m_device(device), m_descriptor_sets(device){}
  void create_descriptor(std::function<void(vk_descriptor_set_array*)> function);
  vk_descriptor_set* get_descriptor(const uint32_t set){return m_descriptor_sets.get(set);}
  void initialize(vk_shader &shader);
  void free();
  void compute(VkCommandBuffer &cmd, uint32_t size_x,uint32_t size_y,uint32_t size_z);

private:
  VkPipeline m_pipeline;
  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device = nullptr;
  VkSemaphore m_compute_semaphore = nullptr;
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
  VkSemaphore m_compute_semaphore = nullptr;
};

} // namespace cui::vulkan
