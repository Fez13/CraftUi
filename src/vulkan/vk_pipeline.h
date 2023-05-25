#pragma once

#include "vk_descriptor_set.h"
#include "vk_shader.h"
#include "vk_utils.h"

#include <functional>
namespace cui::vulkan {

class vk_compute_pipeline {
public:
  explicit vk_compute_pipeline(vk_device *device)
      : m_device(device), m_descriptor_sets(device) {}
  void
  create_descriptor(std::function<void(vk_descriptor_set_array *)> function);
  vk_descriptor_set *get_descriptor(const uint32_t set) {
    return m_descriptor_sets.get(set);
  }
  void initialize(vk_shader &shader);
  void free();
  void compute(VkCommandBuffer &cmd, uint32_t size_x, uint32_t size_y,
               uint32_t size_z);

private:
  VkPipeline m_pipeline;
  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device = nullptr;
  VkSemaphore m_compute_semaphore = nullptr;
};

// This is a virtual class for a rasterization pipeline.
class vk_rasterization_pipeline {
public:
  virtual void initialize(vk_device *device) {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  virtual void free() {
    LOG("Warning this is a call from a virtual function, "
        "this should be overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  virtual void render(VkCommandBuffer &cmd, VkRenderPass &render_pass) {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  virtual void set_view_port_state(const VkPipelineViewportStateCreateInfo &universal_view_port_state_create_info) {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  } 
  
  virtual void update_size() {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  } 

private:
  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device;
  VkSemaphore m_compute_semaphore = nullptr;
};

} // namespace cui::vulkan
