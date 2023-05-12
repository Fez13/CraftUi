#include "vulkan.h"

namespace cui::vulkan {

//
// Initializers
//
VkApplicationInfo vkcApplicationInfo(const char *app_name,
                                     const uint32_t app_version,
                                     const uint32_t api_version,
                                     const glm::ivec3 engine_version) {
  VkApplicationInfo AppInfo{};
  AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  AppInfo.pApplicationName = app_name;
  AppInfo.applicationVersion = app_version;
  AppInfo.apiVersion = api_version;
  AppInfo.engineVersion =
      VK_MAKE_VERSION(engine_version[0], engine_version[1], engine_version[2]);
  AppInfo.pEngineName = "CraftUI";
  AppInfo.pNext = nullptr;
  return AppInfo;
}

VkInstanceCreateInfo
vkcInstanceCreateInfo(VkApplicationInfo &app_info,
                      const std::vector<const char *> &extensions,
                      const std::vector<const char *> &layers) {
  VkInstanceCreateInfo InstanceCreateInfo{};
  InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  InstanceCreateInfo.flags = VkInstanceCreateFlags();
  InstanceCreateInfo.pApplicationInfo = &app_info;

  InstanceCreateInfo.enabledExtensionCount =
      static_cast<uint32_t>(extensions.size());
  InstanceCreateInfo.ppEnabledExtensionNames = extensions.data();

  InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
  InstanceCreateInfo.ppEnabledLayerNames = layers.data();
  InstanceCreateInfo.pNext = nullptr;
  return InstanceCreateInfo;
}

VkDeviceQueueCreateInfo vkcDeviceQueueCreateInfo(VkDeviceQueueCreateFlags flags,
                                                 const uint32_t queue_count,
                                                 float *queue_priority,
                                                 const uint32_t queue_index) {
  VkDeviceQueueCreateInfo queue_create_info;
  queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_create_info.queueFamilyIndex = queue_index;
  queue_create_info.flags = flags;
  queue_create_info.queueCount = queue_count;
  queue_create_info.pQueuePriorities = queue_priority;
  queue_create_info.pNext = VK_NULL_HANDLE;
  return queue_create_info;
}

VkDeviceCreateInfo
vkcDeviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo> &queue_info,
                    const VkPhysicalDeviceFeatures *features,
                    const std::vector<const char *> *extensions,
                    const void *pnext) {

  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_create_info.pQueueCreateInfos = queue_info.data();
  device_create_info.queueCreateInfoCount = queue_info.size();
  device_create_info.pEnabledFeatures = features;
  device_create_info.ppEnabledExtensionNames = extensions->data();
  device_create_info.enabledExtensionCount = extensions->size();
  device_create_info.pNext = pnext;
  return device_create_info;
}

VkCommandPoolCreateInfo vkcCommandPoolCreateInfo(const uint32_t queue_index) {
  VkCommandPoolCreateInfo poolCreateInfo{};
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolCreateInfo.queueFamilyIndex = queue_index;
  poolCreateInfo.pNext = VK_NULL_HANDLE;
  return poolCreateInfo;
}

//
// Queues and devices
//
bool device_suitable(VkPhysicalDevice const &physical_device,
                     std::function<bool(VkPhysicalDeviceProperties &,
                                        VkPhysicalDeviceFeatures &)>
                         checks) {
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;

  vkGetPhysicalDeviceProperties(physical_device, &deviceProperties);
  vkGetPhysicalDeviceFeatures(physical_device, &deviceFeatures);

  if (!checks(deviceProperties, deviceFeatures))
    return false;

  return true;
}
uint32_t getSuitableQueueFamily(
    VkPhysicalDevice physical_device,
    std::function<bool(const VkQueueFamilyProperties &fp)> checks) {
  uint32_t families = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &families, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(families);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &families,
                                           queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilies.size(); i++)
    if (checks(queueFamilies[i]))
      return i;
  ASSERT(false, "There isn't any queue whit the requested capabilities.",
         TEXT_COLOR_ERROR);
  return -1;
}
} // namespace cui::vulkan
