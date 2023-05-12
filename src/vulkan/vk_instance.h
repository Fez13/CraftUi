#pragma once
#include "vk_utils.h"

namespace cui::vulkan {

class vk_instance {

public:
  vk_instance(const vk_instance &) = delete;
  void initialize(const char *appName, uint32_t appVersion, uint32_t apiVersion,
                  const std::vector<std::string> &layers,
                  const std::vector<std::string> &extensions);

  VkInstance &get_instance() {return m_instance;};

  static std::vector<VkLayerProperties> get_availableLayers();

  static std::vector<VkExtensionProperties> get_availableExtensions();

  static vk_instance &get() { return s_instance; }

private:
  vk_instance() = default;

  VkInstance m_instance;
  bool check_layer(const std::vector<VkLayerProperties> &availableLayers,
                  const std::string &layer);

  bool
  check_extension(const std::vector<VkExtensionProperties> &availableExtensions,
                 const std::string &extension);

  std::vector<const char *>
  set_available_layers(const std::vector<std::string> &layers);

  std::vector<const char *>
  set_available_extensions(const std::vector<std::string> &extensions);

  static vk_instance s_instance;
};
} // namespace cui::vulkan
