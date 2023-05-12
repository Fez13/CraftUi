#include "vulkan.h"

namespace cui::vulkan {

void initialize_vulkan() {
  vk_instance::get().initialize(vulkan_initialization_data.application_name,
                          vulkan_initialization_data.application_version,
                          vulkan_initialization_data.api_version,
                          vulkan_initialization_data.layers,
                          vulkan_initialization_data.extensions);
  vk_graphic_device::get().initialize(vulkan_physical_device_necessities);
}



} // namespace cui::vulkan
