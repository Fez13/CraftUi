#pragma once
#include "../vulkan_config/vulkan_config.hpp"
#include "vk_device.h"
#include "vk_graphic_device.h"
#include "vk_instance.h"
#include "vk_utils.h"
#include "vk_buffer.h"
#include "vk_descriptor_set.h"
#include "vk_pipeline.h"
#include "../shader_compiler/compile_Spir_V.h"

namespace cui::vulkan {

/*
  @brief Will initialize vulkan, device and will create a basic device named main.
*/
void initialize_vulkan();

bool is_vulkan_initialized();

} // namespace cui::vulkan
