#include "vk_instance.h"

namespace cui::vulkan {

vk_instance vk_instance::s_instance;

void vk_instance::initialize(const char *app_name, const uint32_t app_version,
                             const uint32_t api_version,
                             const std::vector<std::string> &layers,
                             const std::vector<std::string> &extensions) {
  VkApplicationInfo app_info =
      vkcApplicationInfo(app_name, app_version, api_version);

  std::vector<const char*> available_extensions = set_available_extensions(extensions);
  std::vector<const char*> available_layers = set_available_layers(layers);
  
  VkInstanceCreateInfo instance_create_info = vkcInstanceCreateInfo(app_info,available_extensions,available_layers);
  
  VK_CHECK(vkCreateInstance(&instance_create_info,nullptr,&m_instance));
}

std::vector<VkExtensionProperties> vk_instance::get_availableExtensions() {

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());

  return extensions;
}

std::vector<VkLayerProperties> vk_instance::get_availableLayers() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  return layers;
}

std::vector<const char *> vk_instance::set_available_extensions(
    const std::vector<std::string> &extensions) {
  auto availableExtension = get_availableExtensions();
  std::vector<const char *> pp_extensions;

  for (const auto &extension : extensions) {
    if (!check_extension(availableExtension, extension)) {
      LOG("Couldn't find extension... \n\tExtension name: " + extension + "\n",
          TEXT_COLOR_WARNING);
      continue;
    }
    pp_extensions.push_back(extension.c_str());
  }
  return pp_extensions;
}

std::vector<const char *>
vk_instance::set_available_layers(const std::vector<std::string> &layers) {

  auto availableLayers = get_availableLayers();
  std::vector<const char *> pp_layers;

  for (const auto &layer : layers) {
    if (!check_layer(availableLayers, layer)) {
      LOG("Couldn't find layer... \n\tLayer name: " + layer + '\n',
          TEXT_COLOR_WARNING);
      continue;
    }
    pp_layers.push_back(layer.c_str());
  }

  return pp_layers;
}

bool vk_instance::check_layer(
    const std::vector<VkLayerProperties> &availableLayers,
    const std::string &layer) {

  for (const auto &availableLayer : availableLayers)
    if ((std::string)availableLayer.layerName == layer)
      return true;
  return false;
}

bool vk_instance::check_extension(
    const std::vector<VkExtensionProperties> &availableExtensions,
    const std::string &extension) {
  for (const auto &availableExtension : availableExtensions)
    if ((std::string)availableExtension.extensionName == extension)
      return true;
  return false;
}

} // namespace cui::vulkan
