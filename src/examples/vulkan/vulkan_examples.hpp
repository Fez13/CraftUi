#pragma once
#include "../../vulkan/vulkan.h"
#include "../../vulkan_config/vulkan_config.hpp"

namespace cui::vulkan::example {
#ifdef TESTING

static void buffer_test() {
  vk_buffer test_buffer_src(vk_device_manager::get().get_device("main"),
                            sizeof(int) * 200, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                            VK_SHARING_MODE_EXCLUSIVE);
  test_buffer_src.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  vk_buffer test_buffer_dst(vk_device_manager::get().get_device("main"),
                            sizeof(int) * 200, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                            VK_SHARING_MODE_EXCLUSIVE);
  test_buffer_dst.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  int *buffer = test_buffer_src.get_memory_location<int>();
  buffer[0] = 1;
  test_buffer_src.unmap_memory();

  test_buffer_dst.copy_from(&test_buffer_src);

  ASSERT(test_buffer_dst.get_memory_location<int>()[0] != 1,
         "Error copying data form buffer.", TEXT_COLOR_ERROR);
  test_buffer_dst.free();
  test_buffer_src.free();
  LOG("\t buffer_test completed.", TEXT_COLOR_NOTIFICATION);
}

static void descriptor_test() {
  vk_descriptor_set *descriptor_set_test =
      vk_descriptor_set::create(vk_device_manager::get().get_device("main"));

  vk_buffer descriptor_set_test_buffer(
      vk_device_manager::get().get_device("main"), sizeof(int) * 200,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE);
  descriptor_set_test_buffer.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  descriptor_set_test->create_binding_uniform_buffer(0);

  descriptor_set_test->initialize_layout();
  descriptor_set_test->allocate_descriptor_set();

  descriptor_set_test->update_binding_uniform_buffer(
      0, &descriptor_set_test_buffer);

  vk_descriptor_set::free_one(descriptor_set_test);
  descriptor_set_test_buffer.free();
  LOG("\t Descriptor_test completed.", TEXT_COLOR_NOTIFICATION);
}

static void image_test() {
  vk_image image("/home/federico/CLionProjects/CraftUi/resources/images/"
                 "Screenshot from 2022-11-05 20-40-04.png",
                 "main");
  image.free();
  LOG("\t Image_test completed.", TEXT_COLOR_NOTIFICATION);
}

static void compute_pipeline_test() {
  vk_compute_pipeline pipeline =
      vk_compute_pipeline(vk_device_manager::get().get_device("main"));
  pipeline.create_descriptor([](vk_descriptor_set_array *array) {
    vk_descriptor_set *descriptor = array->create();
    descriptor->create_binding_storage_buffer(0, VK_SHADER_STAGE_COMPUTE_BIT);
    descriptor->initialize_layout();
    descriptor->allocate_descriptor_set();
  });

  std::string new_path =
      "/home/federico/CLionProjects/CraftUi/resources/shaders/compute/"
      "compiled/compute_shader_test.comp";

  spirv::compile("/home/federico/CLionProjects/CraftUi/resources/shaders/"
                 "compute/compute_shader_test.comp",
                 spirv::COMPUTE, new_path, true);
  vk_shader shader =
      vk_shader(new_path, vk_device_manager::get().get_device("main"));
  pipeline.initialize(shader);

  vk_buffer buffer(
      vk_device_manager::get().get_device("main"), sizeof(int) * 10,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE);

  buffer.initialize_buffer_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

  int *buffer_mapped = buffer.get_memory_location<int>();
  for (uint32_t i = 0; i < 10; i++) {
    buffer_mapped[i] = i;
  }
  buffer.unmap_memory();
  pipeline.get_descriptor(0)->update_binding_storage_buffer(0, &buffer, 1);

  VkCommandBuffer cmd = vk_device_manager::get()
                            .get_device("main")
                            ->create_one_time_use_command_buffer(
                                VK_COMMAND_BUFFER_LEVEL_PRIMARY, "COMPUTE");

  pipeline.compute(cmd, 10, 1, 1);
  vk_device_manager::get().get_device("main")->submit_command_buffer(&cmd, 1,
                                                                     "COMPUTE");

  vk_device_manager::get().get_device("main")->wait_to_finish("COMPUTE");

  buffer_mapped = buffer.get_memory_location<int>();
  ASSERT(buffer_mapped[9] != 81, "Error executing Compute_pipeline_test.",
         TEXT_COLOR_ERROR);
  buffer.unmap_memory();

  buffer.free();
  pipeline.free();
  LOG("\t Compute_pipeline_test completed.", TEXT_COLOR_NOTIFICATION);
}

#else
static void buffer_test() {
  LOG("Please define TESTING in vulkan_config/vulkan_config.hpp (#define "
      "TESTING) for testing functions.",
      TEXT_COLOR_WARNING);
}
static void descriptor_test() {
  LOG("Please define TESTING in vulkan_config/vulkan_config.hpp (#define "
      "TESTING) for testing functions.",
      TEXT_COLOR_WARNING);
}
static void image_test() {
  LOG("Please define TESTING in vulkan_config/vulkan_config.hpp (#define "
      "TESTING) for testing functions.",
      TEXT_COLOR_WARNING);
}
static void compute_pipeline_test() {
  LOG("Please define TESTING in vulkan_config/vulkan_config.hpp (#define "
      "TESTING) for testing functions.",
      TEXT_COLOR_WARNING);
}
#endif


/*
  @brief Uses the example code to test the engine.
  @warning It's needs to have TESTING defined in the vulkan_config.hpp file.
*/
void test_vulkan() {
  LOG("Vulkan testing has started.", TEXT_COLOR_WARNING);

  try {
    
    if(!vulkan::is_vulkan_initialized())
      vulkan::initialize_vulkan();
    
    
    buffer_test();
    
    descriptor_test();
    
    image_test();
    
    compute_pipeline_test();

  } catch (std::runtime_error error) {
    LOG("Vulkan testing ended with error: " + error.what(), TEXT_COLOR_ERROR);
  }
  LOG("Vulkan testing is done!.", TEXT_COLOR_WARNING);
}

} // namespace cui::vulkan::example
