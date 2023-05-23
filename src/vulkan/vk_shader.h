#pragma once

#include "vk_device.h"
#include "vk_utils.h"

#include <fstream>

namespace cui::vulkan {
// TODO: shader module

class vk_shader {
public:
  vk_shader(const std::string &path, vk_device *device);

  VkShaderModule get_shader_module(){return m_shader_module;}

protected:
  void create_shader(std::vector<char>& data);
  VkShaderModule m_shader_module;
  VkDevice m_device;
  std::string m_path;
};

} // namespace cui::vulkan
