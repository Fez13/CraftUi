#pragma once
#include "../vulkan/vk_swap_chain.h"
#include "../vulkan/vk_device.h"

namespace cui::renderer {

class window {
public:
  window() = default;
  virtual void initialize(const uint32_t width, const uint32_t height, const uint32_t refresh_rate){};
  
  vulkan::vk_swap_chain *get_swap_chain() { return &m_swap_chain; }
  virtual uint32_t get_queue_family() {return 0;}
  
protected:
  vulkan::vk_swap_chain m_swap_chain;
  vulkan::vk_device *m_device;
};

} // namespace cui::renderer
