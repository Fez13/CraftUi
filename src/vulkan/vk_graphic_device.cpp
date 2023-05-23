#include "vk_graphic_device.h"

namespace cui::vulkan {

vk_graphic_device vk_graphic_device::s_instance;

void vk_graphic_device::initialize(
    std::function<bool(VkPhysicalDeviceProperties &,
                       VkPhysicalDeviceFeatures &)>
        required_properties_and_features) {
  LOG("Begging of graphic device creation.", TEXT_COLOR_NOTIFICATION);
  uint32_t device_count = 0;
  vkEnumeratePhysicalDevices(vk_instance::get().get_instance(), &device_count,
                             nullptr);
  ASSERT(device_count == 0,
         "Error, no vulkan compatible graphics card was found.",
         TEXT_COLOR_ERROR);

  std::vector<VkPhysicalDevice> devices;
  devices.resize(device_count);

  bool pick = false;
  vkEnumeratePhysicalDevices(vk_instance::get().get_instance(), &device_count,
                             devices.data());

  for (VkPhysicalDevice &device : devices) {
    if (device_suitable(device, required_properties_and_features)) {
      m_physical_device = device;
      pick = true;
      break;
    }
  }

  ASSERT(!pick,
         "There is a vulkan capable device, but it doesn't contain the "
         "necessary proprieties.",
         TEXT_COLOR_ERROR);

  m_device_families.set(
      getSuitableQueueFamily(m_physical_device,
                             [](const VkQueueFamilyProperties &fp) {
                               if ((fp.queueFlags & VK_QUEUE_GRAPHICS_BIT))
                                 return true;
                               return false;
                             }),
      "GRAPHIC");

  m_device_families.set(
      getSuitableQueueFamily(m_physical_device,
                             [](const VkQueueFamilyProperties &fp) {
                               if ((fp.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                                   (!(fp.queueFlags & VK_QUEUE_GRAPHICS_BIT)))
                                 return true;
                               return false;
                             }),
      "COMPUTE");

  m_device_families.set(
      getSuitableQueueFamily(m_physical_device,
                             [](const VkQueueFamilyProperties &fp) {
                               if ((fp.queueFlags & VK_QUEUE_TRANSFER_BIT))
                                 return true;
                               return false;
                             }),
      "TRANSFER");
  LOG("End of graphic device creation.", TEXT_COLOR_NOTIFICATION);
}

} // namespace cui::vulkan
