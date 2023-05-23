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
  if (extensions != nullptr) {
    device_create_info.ppEnabledExtensionNames = extensions->data();
    device_create_info.enabledExtensionCount = extensions->size();
  } else {
    device_create_info.ppEnabledExtensionNames = VK_NULL_HANDLE;
    device_create_info.enabledExtensionCount = 0;
  }
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

VkBufferCreateInfo vkcBufferCreateInfo(const VkSharingMode sharing,
                                       const size_t size,
                                       const VkBufferUsageFlags usage) {
  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.sharingMode = sharing;
  bufferCreateInfo.size = size;
  bufferCreateInfo.usage = usage;
  bufferCreateInfo.pNext = nullptr;
  return bufferCreateInfo;
}

VkMemoryAllocateInfo vkcMemoryAllocateInfo(const uint32_t memory_index,
                                           const size_t size) {
  VkMemoryAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocateInfo.allocationSize = size;
  allocateInfo.memoryTypeIndex = memory_index;
  allocateInfo.pNext = nullptr;
  return allocateInfo;
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
  ASSERT(true, "There isn't any queue whit the requested capabilities.",
         TEXT_COLOR_ERROR);
  return UINT32_MAX;
}

//
// Memory functions
//

uint32_t
find_memory(const VkMemoryPropertyFlags memory_properties,
            VkPhysicalDeviceMemoryProperties physical_device_memory_properties,
            const uint32_t memory_type) {
  for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount;
       i++) {
    if (!(memory_type & (1 < i)))
      continue;

    if ((physical_device_memory_properties.memoryTypes[i].propertyFlags &
         memory_properties) == memory_properties)
      return i;
  }
  ASSERT(true, "Couldn't find memory properties", TEXT_COLOR_ERROR);
  return UINT32_MAX;
}

//
// Descriptor sets
//

VkDescriptorSetLayoutBinding vkcDescriptorSetLayoutBinding(
    const uint32_t binding, const uint32_t count, const VkDescriptorType type,
    const VkSampler *sampler, const VkShaderStageFlags stages) {
  VkDescriptorSetLayoutBinding obj;
  obj.binding = binding;
  obj.descriptorCount = count;
  obj.descriptorType = type;
  obj.pImmutableSamplers = sampler;
  obj.stageFlags = stages;
  return obj;
}

VkDescriptorSetLayoutCreateInfo
vkcDescriptorSetLayoutCreateInfo(const uint32_t count,
                                 const VkDescriptorSetLayoutBinding *bindings) {
  VkDescriptorSetLayoutCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  createInfo.bindingCount = count;
  createInfo.pBindings = bindings;
  createInfo.pNext = nullptr;
  return createInfo;
}

VkDescriptorPoolCreateInfo
vkcDescriptorPoolCreateInfo(const uint32_t count,
                            const VkDescriptorPoolSize *pool_sizes) {
  VkDescriptorPoolCreateInfo descriptor_pool_create{};
  descriptor_pool_create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descriptor_pool_create.flags =
      VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  descriptor_pool_create.poolSizeCount = count;
  descriptor_pool_create.pPoolSizes = pool_sizes;
  descriptor_pool_create.maxSets = count;
  return descriptor_pool_create;
}

VkDescriptorSetAllocateInfo
vkcDescriptorSetAllocateInfo(const VkDescriptorSetLayout *layout,
                             const VkDescriptorPool pool) {
  VkDescriptorSetAllocateInfo descriptor_set_allocate{};
  descriptor_set_allocate.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descriptor_set_allocate.descriptorSetCount = 1;
  descriptor_set_allocate.pSetLayouts = layout;
  descriptor_set_allocate.descriptorPool = pool;
  descriptor_set_allocate.pNext = nullptr;
  return descriptor_set_allocate;
}

//
// Images
//

VkImageCreateInfo vkcImageCreateInfo(const glm::ivec2 size,
                                     const VkFormat format,
                                     const VkImageTiling tiling,
                                     const VkImageUsageFlags usage,
                                     const VkImageLayout image_layout) {
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.extent.width = size.x;
  image_info.extent.height = size.y;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.format = format;
  image_info.tiling = tiling;
  image_info.initialLayout = image_layout;
  image_info.usage = usage;
  image_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  image_info.pNext = nullptr;
  return image_info;
}

VkImageMemoryBarrier vkcImageMemoryBarrier(const VkImageLayout old_layout,
                                           const VkImageLayout new_layout,
                                           VkImage image) {

  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = old_layout;
  barrier.newLayout = new_layout;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.pNext = nullptr;
  return barrier;
}

VkImageViewCreateInfo
vkcImageViewCreateInfo(const VkImageViewType view_type, VkImage image,
                       const VkFormat format, const VkImageAspectFlags flags,
                       const uint32_t baseMinp, const uint32_t base_array,
                       const uint32_t level_count, const uint32_t layer_count) {

  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.viewType = view_type;
  info.image = image;
  info.format = format;
  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  info.subresourceRange.aspectMask = flags;
  info.subresourceRange.baseMipLevel = baseMinp;
  info.subresourceRange.levelCount = level_count;
  info.subresourceRange.baseArrayLayer = base_array;
  info.subresourceRange.layerCount = layer_count;
  info.pNext = nullptr;
  return info;
}

//
// Swap chains
//

VkSwapchainCreateInfoKHR vkcSwapchainCreateInfoKHR(
    const VkSurfaceKHR &surface, const uint32_t min_image_count,
    const VkFormat &image_format, const VkColorSpaceKHR &color_space,
    const VkExtent2D &extent,
    const VkSurfaceTransformFlagBitsKHR transform_flags,
    const VkPresentModeKHR present_mode) {

  VkSwapchainCreateInfoKHR swap_chain_info = {};
  swap_chain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swap_chain_info.surface = surface;
  swap_chain_info.minImageCount = min_image_count;
  swap_chain_info.imageFormat = image_format;
  swap_chain_info.imageColorSpace = color_space;
  swap_chain_info.imageExtent = extent;
  swap_chain_info.imageArrayLayers = 1;
  swap_chain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swap_chain_info.preTransform = transform_flags;
  swap_chain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swap_chain_info.presentMode = present_mode;
  swap_chain_info.clipped = VK_TRUE;
  swap_chain_info.oldSwapchain = VK_NULL_HANDLE;
  swap_chain_info.pNext = nullptr;
  return swap_chain_info;
}

//
// Pipelines
//

VkComputePipelineCreateInfo vkcComputePipelineCreateInfo(
    const VkPipelineLayout layout,
    const VkPipelineShaderStageCreateInfo shader_stage) {
  VkComputePipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
  pipeline_info.layout = layout;
  pipeline_info.stage = shader_stage;
  pipeline_info.pNext = nullptr;
  return pipeline_info;
}

} // namespace cui::vulkan
