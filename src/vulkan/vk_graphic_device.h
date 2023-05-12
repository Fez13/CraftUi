#pragma once
#include <unordered_map>

#include "../core/macros.h"
#include "vk_instance.h"
#include "vk_utils.h"

namespace cui::vulkan {

class vk_graphic_device {
public:
  vk_graphic_device(vk_graphic_device &) = delete;

  void initialize(std::function<bool(VkPhysicalDeviceProperties &,
                                     VkPhysicalDeviceFeatures &)>
                      required_properties_and_features);

  static vk_graphic_device &get() { return s_instance; }
  
  VkPhysicalDevice get_device(){return m_physical_device;}

private:
  struct device_families {
  public:
    uint32_t get(const std::string &name){return 1;};
    void set(const uint32_t &id, const std::string &name) {
      ASSERT(!data.contains(name),
             "Error, family: " + name + " is already in the device",
             TEXT_COLOR_ERROR);
    }

  private:
    std::unordered_map<std::string, uint32_t> data;
  } m_device_families;

  VkPhysicalDevice m_physical_device;
  vk_graphic_device() = default;
  static vk_graphic_device s_instance;
};

} // namespace cui::vulkan
