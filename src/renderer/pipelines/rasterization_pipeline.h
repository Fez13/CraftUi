#pragma once
#include "../../vulkan/vulkan.h"
#include "../vertex.h"

namespace cui::renderer {

using namespace vulkan;

class default_rasterization_2d : public vk_rasterization_pipeline {

public:
  void initialize() override;
  
  void free() override;
  
  void render(VkCommandBuffer &cmd) override;
  
  void update_size() override;

private:
  
  vk_descriptor_set *m_descriptor;
  
  void initialize_buffers() override;
  
  void initialize_default_mesh() override{};
  
};

} // namespace cui::renderer
