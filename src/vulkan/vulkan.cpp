#include "vulkan.h"
namespace cui::vulkan {

static bool vulkan_initialized = false;

bool is_vulkan_initialized(){return vulkan_initialized;}

void initialize_vulkan() {
  if (vulkan_initialized) {
    LOG("Vulkan is already initialized", TEXT_COLOR_WARNING);
    return;
  }
  // Initialize GPU
  vk_instance::get().initialize(vulkan_initialization_data.application_name,
                                vulkan_initialization_data.application_version,
                                vulkan_initialization_data.api_version,
                                vulkan_initialization_data.layers,
                                vulkan_initialization_data.extensions);
  vk_graphic_device::get().initialize(vulkan_physical_device_necessities);

  // Creates default device
  vkc_device_create_data create_data;
  create_data.queues = {
      {"COMPUTE", vk_graphic_device::get().get_device_family("COMPUTE")},
      {"GRAPHIC", vk_graphic_device::get().get_device_family("GRAPHIC")},
      {"TRANSFER", vk_graphic_device::get().get_device_family("TRANSFER")}};
  create_data.device_name = "main";
  create_data.features_chain = VK_NULL_HANDLE;
  create_data.gpu_extensions = nullptr;
  vk_device_manager::get().create(create_data);

  vulkan_initialized = true;
}
} // namespace cui::vulkan
