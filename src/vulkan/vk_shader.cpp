#include "vk_shader.h"

namespace cui::vulkan {

static std::vector<char> read_binary_file(const char *path) {
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  ASSERT(!file.is_open(),
         "Fail opening binary file...\n\tPath: " + std::string(path),
         TEXT_COLOR_ERROR);
  auto file_s = (uint32_t)file.tellg();
  std::vector<char> in(file_s);

  file.seekg(0);
  file.read(in.data(), file_s);
  file.close();

  return in;
}

void vk_shader::create_shader(std::vector<char> &data) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = data.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(data.data());
  createInfo.pNext = nullptr;
  VK_CHECK(
      vkCreateShaderModule(m_device, &createInfo, nullptr, &m_shader_module),
      "Fail creating shader module");
}

vk_shader::vk_shader(const std::string &path, vk_device *device) {
  m_device = device->get_device();
  m_path = path;
  std::vector<char> data = read_binary_file(path.c_str()); 
  create_shader(data);
}
  
  
  
} // namespace cui::vulkan
