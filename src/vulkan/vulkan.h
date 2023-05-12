#pragma once
#include "../vulkan_config/vulkan_config.hpp"
#include "vk_device.h"
#include "vk_graphic_device.h"
#include "vk_instance.h"
#include "vk_utils.h"

namespace cui::vulkan {

// This will handel: instance creation, device creation, function
// initialization.
void initialize_vulkan();
} // namespace cui::vulkan
