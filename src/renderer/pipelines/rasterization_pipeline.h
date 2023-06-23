#pragma once
#include "../../vulkan/vulkan.h"
#include "../vertex.h"
#include "../scene.h"
#include "../material.h"

namespace cui::renderer {

using namespace vulkan;
class default_rasterization_2d : public vk_rasterization_pipeline {

public:
  void initialize() override;

  void free() override;

  void render(VkCommandBuffer &cmd) override;

  void initialize_buffers() override;

  void initialize_default_mesh() override {}

private:
  void on_size_update() override {}

  vk_descriptor_set *m_descriptor;
};


struct rasterization_text_data{
  glm::vec4 pos;
};

class default_rasterization_text : public vk_rasterization_pipeline {

public:
  void initialize() override;

  void free() override;

  void render(VkCommandBuffer &cmd) override;

  void initialize_buffers() override;

  void initialize_default_mesh() override {}
  
  //Pointer to a buffer
  rasterization_text_data *special_draw_call_data = nullptr;

private:
  
  vulkan::vk_buffer m_special_data_buffer;
  
  void on_size_update() override {}

  vk_descriptor_set *m_descriptor;
};

} // namespace cui::renderer
