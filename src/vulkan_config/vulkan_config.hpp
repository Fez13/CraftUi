#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace cui::vulkan {

const struct structure_vulkan_initialization_data {
  const char *application_name = "Cui Test";
  uint32_t application_version = VK_MAKE_VERSION(0, 0, 1);
  uint32_t api_version = VK_API_VERSION_1_3;
  std::vector<std::string> extensions = {
      "VK_KHR_xcb_surface",
      "VK_KHR_surface",
      "VK_KHR_display",
  };
  std::vector<std::string> layers = {"VK_LAYER_KHRONOS_validation"};
} vulkan_initialization_data;

inline bool vulkan_physical_device_necessities(VkPhysicalDeviceProperties &pt,
                                               VkPhysicalDeviceFeatures &ft) {
  if (ft.multiDrawIndirect != VK_TRUE)
    return false;

  if (ft.sampleRateShading != VK_TRUE)
    return false;

  if (pt.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    return false;

  if (ft.samplerAnisotropy != VK_TRUE)
    return false;

  if (ft.geometryShader != VK_TRUE)
    return false;

  if (ft.fillModeNonSolid != VK_TRUE)
    return false;

  return true;
}

} // namespace cui::vulkan
