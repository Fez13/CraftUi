#include "vulkan.h"

namespace cui::vulkan {

static bool vulkan_initialized = false;

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

void testing() {
  LOG("Testing has started.", TEXT_COLOR_WARNING);

  try {
    if (!vulkan_initialized)
      initialize_vulkan();

    {
      vk_buffer test_buffer_src(
          vk_device_manager::get().get_device("main"), sizeof(int) * 200,
          VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
      test_buffer_src.initialize_buffer_memory(
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      vk_buffer test_buffer_dst(
          vk_device_manager::get().get_device("main"), sizeof(int) * 200,
          VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_SHARING_MODE_EXCLUSIVE);
      test_buffer_dst.initialize_buffer_memory(
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      int *buffer = test_buffer_src.get_memory_location<int>();
      buffer[0] = 1;
      test_buffer_src.unmap_memory();

      test_buffer_dst.copy_from(&test_buffer_src);

      ASSERT(test_buffer_dst.get_memory_location<int>()[0] == 1,
             "Error coping data form buffer.", TEXT_COLOR_ERROR);
      test_buffer_dst.free();
      test_buffer_src.free();
      LOG("\t Buffer_test completed.", TEXT_COLOR_NOTIFICATION);
    }
    {
      vk_descriptor_set *descriptor_set_test = vk_descriptor_set::create(
          vk_device_manager::get().get_device("main"));
          

      vk_buffer descriptor_set_test_buffer(vk_device_manager::get().get_device("main"), sizeof(int) * 200,
                VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
      descriptor_set_test_buffer.initialize_buffer_memory(
          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

      descriptor_set_test->create_binding_uniform_buffer(0);
      
      descriptor_set_test->initialize_layout();
      descriptor_set_test->allocate_descriptor_set();
      
      descriptor_set_test->update_binding_uniform_buffer(0,&descriptor_set_test_buffer);
      
      vk_descriptor_set::free_one(descriptor_set_test);
      descriptor_set_test_buffer.free();
      LOG("\t Descriptor_test completed.", TEXT_COLOR_NOTIFICATION);

    }

  } catch (std::runtime_error error) {
    LOG("Testing ended with error: " + error.what(),TEXT_COLOR_ERROR);
  }
  LOG("Testing is done!.", TEXT_COLOR_WARNING);
}

} // namespace cui::vulkan
