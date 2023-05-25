#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

#define TESTING
#define DESKTOP
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

inline VkPhysicalDeviceFeatures2 physical_device_features() {
  VkPhysicalDeviceFeatures2 features;
  features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  features.features.multiDrawIndirect = VK_TRUE;
  features.features.sampleRateShading = VK_TRUE;
  features.features.samplerAnisotropy = VK_TRUE;
  features.features.geometryShader = VK_TRUE;
  features.features.fillModeNonSolid = VK_TRUE;
  features.features.robustBufferAccess = VK_FALSE;
  features.features.fullDrawIndexUint32 = VK_FALSE;
  features.features.imageCubeArray = VK_FALSE;
  features.features.independentBlend = VK_FALSE;
  features.features.tessellationShader = VK_FALSE;
  features.features.dualSrcBlend = VK_FALSE;
  features.features.logicOp = VK_FALSE;
  features.features.drawIndirectFirstInstance = VK_FALSE;
  features.features.depthClamp = VK_FALSE;
  features.features.depthBiasClamp = VK_FALSE;
  features.features.depthBounds = VK_FALSE;
  features.features.wideLines = VK_FALSE;
  features.features.largePoints = VK_FALSE;
  features.features.alphaToOne = VK_FALSE;
  features.features.multiViewport = VK_FALSE;
  features.features.textureCompressionETC2 = VK_FALSE;
  features.features.textureCompressionASTC_LDR = VK_FALSE;
  features.features.textureCompressionBC = VK_FALSE;
  features.features.occlusionQueryPrecise = VK_FALSE;
  features.features.pipelineStatisticsQuery = VK_FALSE;
  features.features.vertexPipelineStoresAndAtomics = VK_FALSE;
  features.features.fragmentStoresAndAtomics = VK_FALSE;
  features.features.shaderTessellationAndGeometryPointSize = VK_FALSE;
  features.features.shaderImageGatherExtended = VK_FALSE;
  features.features.shaderStorageImageExtendedFormats = VK_FALSE;
  features.features.shaderStorageImageMultisample = VK_FALSE;
  features.features.shaderStorageImageReadWithoutFormat = VK_FALSE;
  features.features.shaderStorageImageWriteWithoutFormat = VK_FALSE;
  features.features.shaderUniformBufferArrayDynamicIndexing = VK_FALSE;
  features.features.shaderSampledImageArrayDynamicIndexing = VK_FALSE;
  features.features.shaderStorageBufferArrayDynamicIndexing = VK_FALSE;
  features.features.shaderStorageImageArrayDynamicIndexing = VK_FALSE;
  features.features.shaderClipDistance = VK_FALSE;
  features.features.shaderCullDistance = VK_FALSE;
  features.features.shaderFloat64 = VK_FALSE;
  features.features.shaderInt64 = VK_FALSE;
  features.features.shaderInt16 = VK_FALSE;
  features.features.shaderResourceResidency = VK_FALSE;
  features.features.shaderResourceMinLod = VK_FALSE;
  features.features.sparseBinding = VK_FALSE;
  features.features.sparseResidencyBuffer = VK_FALSE;
  features.features.sparseResidencyImage2D = VK_FALSE;
  features.features.sparseResidencyImage3D = VK_FALSE;
  features.features.sparseResidency2Samples = VK_FALSE;
  features.features.sparseResidency4Samples = VK_FALSE;
  features.features.sparseResidency8Samples = VK_FALSE;
  features.features.sparseResidency16Samples = VK_FALSE;
  features.features.sparseResidencyAliased = VK_FALSE;
  features.features.variableMultisampleRate = VK_FALSE;
  features.features.inheritedQueries = VK_FALSE;
  features.pNext = nullptr;
  return features;
}

inline VkPhysicalDeviceBufferAddressFeaturesEXT buffer_address_feature() {
  VkPhysicalDeviceBufferAddressFeaturesEXT bufferAddress;
  bufferAddress.bufferDeviceAddress = VK_TRUE;
  bufferAddress.bufferDeviceAddressCaptureReplay = VK_FALSE;
  bufferAddress.bufferDeviceAddressMultiDevice = VK_FALSE;
  bufferAddress.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_ADDRESS_FEATURES_EXT;
  bufferAddress.pNext = nullptr;
  return bufferAddress;
}

#ifdef DESKTOP

#define SHADOW_PASS_DEPTH_FORMAT VK_FORMAT_D16_UNORM

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

inline std::vector<const char *> device_khr_extensions() {
  std::vector<const char *> extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_RAY_QUERY_EXTENSION_NAME,
      VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
      VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
      VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
      VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,
      "VK_KHR_deferred_host_operations",
      "VK_EXT_descriptor_indexing"};
  return extensions;
}

#else

#define SHADOW_PASS_DEPTH_FORMAT VK_FORMAT_D16_UNORM


inline bool vulkan_physical_device_necessities(VkPhysicalDeviceProperties &pt,
                                               VkPhysicalDeviceFeatures &ft) {
  if (ft.multiDrawIndirect != VK_TRUE)
    return false;

  if (ft.sampleRateShading != VK_TRUE)
    return false;

  if (ft.geometryShader != VK_TRUE)
    return false;

  if (ft.fillModeNonSolid != VK_TRUE)
    return false;

  return true;
}

inline std::vector<const char *> device_khr_extensions() {
  std::vector<const char *> extensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME,
      VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME,
      VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
      "VK_KHR_deferred_host_operations",
      "VK_EXT_descriptor_indexing"};
  return extensions;
}

#endif

} // namespace cui::vulkan
