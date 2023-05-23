#include "glfw_window.h"

namespace cui::glfw {

void glfw_window::set_window_size(const uint32_t width, const uint32_t height,
                                  const bool update_glfw_window) {
  m_width = width;
  m_height = height;
  if (update_glfw_window)
    glfwSetWindowSize(m_window, width, height);
  m_swap_chain.set_image_size(width, height);
  m_swap_chain.create_swap_chain();
}

void glfw_window::create_depth_image(vulkan::vk_image *image,
                                     VkRenderPass &render_pass) {
  m_swap_chain.create_depth_frame_buffer(image, render_pass);
}

void glfw_window::set_window_refresh_rate(const float refresh) {
  glfwWindowHint(GLFW_REFRESH_RATE, refresh);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
  glfw_window::get_main()->set_window_size(width, height, false);
}

void glfw_window::initialize(const uint32_t width, const uint32_t height, const uint32_t refresh_rate) {
  create_surface();
  if (!vulkan::vk_device_manager::get().contains("DEVICE_KHR")) {
    vulkan::vkc_device_create_data device_khr_create_data;

    device_khr_create_data.device_name = "DEVICE_KHR";

    VkPhysicalDeviceFeatures2 features = vulkan::physical_device_features();
    VkPhysicalDeviceBufferAddressFeaturesEXT buffer_address =
        vulkan::buffer_address_feature();
    features.pNext = &buffer_address;
    device_khr_create_data.features_chain = &features;

    std::vector<const char *> gpu_extensions = vulkan::device_khr_extensions();
    device_khr_create_data.gpu_extensions = &gpu_extensions;

    device_khr_create_data.queues = {
        {"COMPUTE",
         vulkan::vk_graphic_device::get().get_device_family("COMPUTE")},
        {"GRAPHIC", get_queue_family()},
        {"TRANSFER",
         vulkan::vk_graphic_device::get().get_device_family("TRANSFER")}};
    vulkan::vk_device_manager::get().create(device_khr_create_data);
  }
  
  m_width = width;
  m_height = height;
  glfwSetWindowSize(m_window, width, height);
  glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);
  
  m_device = vulkan::vk_device_manager::get().get_device("DEVICE_KHR");
  m_swap_chain.initialize(m_device, 3);
  glfwSetWindowSizeCallback(m_window, window_size_callback);
  if(glfw_window::s_main_window != nullptr)
    LOG("Double window initialization detected, may create a memory leak.",TEXT_COLOR_WARNING);
  glfw_window::s_main_window = this;
}

void glfw_window::create_surface() {

  m_window = glfwCreateWindow(m_width, m_height, "CraftUI", nullptr, nullptr);

  ASSERT(m_window == nullptr, "Error initializing window, fatal error.",
         TEXT_COLOR_ERROR);

  VK_CHECK(glfwCreateWindowSurface(vulkan::vk_instance::get().get_instance(),
                                   m_window, nullptr, &m_surface),
           "Error creating windowSurface");
  m_swap_chain.set_surface(m_surface);
}

void glfw_window::free(){
  m_swap_chain.free();
  glfwDestroyWindow(m_window);
}

} // namespace cui::glfw
