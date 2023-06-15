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

} // namespace cui::renderer
