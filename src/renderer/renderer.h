#pragma once

#include "../core/macros.h"
#include "../vulkan/vk_instance.h"

namespace cui {

class Renderer {
public:
  Renderer(Renderer &) = delete;
  void initialize();

  static Renderer &get() { return s_instance; }

private:
  Renderer() = default;
  vulkan::vk_instance *m_vk_instance;
  
  
  
  static Renderer s_instance;
};

} // namespace cui
