#pragma once

#include "vk_device.h"
#include "vk_utils.h"

#include <fstream>

namespace cui::vulkan {

class vk_shader {
public:
  vk_shader(const std::string &path, vk_device *device);

  VkShaderModule get_shader_module() { return m_shader_module; }

protected:
  void create_shader(std::vector<char> &data);
  VkShaderModule m_shader_module;
  VkDevice m_device;
  std::string m_path;
};

class vk_graphic_shader : public vk_shader {
public:
  vk_graphic_shader(const std::string &path, vk_device *device,
                    const VkShaderStageFlagBits stage);
  VkPipelineShaderStageCreateInfo get_shader_stage_create_info() {return m_shader_stage;}

protected:
  VkShaderStageFlagBits m_stage;

  VkPipelineShaderStageCreateInfo m_shader_stage{};
};

} // namespace cui::vulkan
