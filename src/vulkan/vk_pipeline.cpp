#include "vk_pipeline.h"

namespace cui::vulkan {

void vk_compute_pipeline::create_descriptor(
    std::function<void(vk_descriptor_set_array *)> function) {
  ASSERT(m_descriptor_sets.is_lock(),
         "Error, tried to modify locked descriptor", TEXT_COLOR_ERROR);
  function(&m_descriptor_sets);
  m_descriptor_sets.lock_array();
}

void vk_compute_pipeline::initialize(vk_shader &shader) {
  ASSERT(
      !m_descriptor_sets.is_lock(),
      "Error, tried to create pipeline whiteout initializing it's descriptor.",
      TEXT_COLOR_ERROR);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = m_descriptor_sets.count();

  std::vector<VkDescriptorSetLayout> layouts = m_descriptor_sets.get_layouts();
  pipelineLayoutInfo.pSetLayouts = layouts.data();

  VK_CHECK(vkCreatePipelineLayout(m_device->get_device(), &pipelineLayoutInfo,
                                  nullptr, &m_pipeline_layout),
           "Error creating pipeline layout for a compute pipeline.");

  VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
  computeShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
  computeShaderStageInfo.module = shader.get_shader_module();
  computeShaderStageInfo.pName = "main";

  VkComputePipelineCreateInfo pipeline_create_info =
      vkcComputePipelineCreateInfo(m_pipeline_layout, computeShaderStageInfo);

  vkCreateComputePipelines(m_device->get_device(), VK_NULL_HANDLE, 1,
                           &pipeline_create_info, nullptr, &m_pipeline);
}
void vk_compute_pipeline::free() {
  m_descriptor_sets.free();
  vkDestroyPipeline(m_device->get_device(), m_pipeline,nullptr);
  vkDestroyPipelineLayout(m_device->get_device(),m_pipeline_layout,nullptr);
}

void vk_compute_pipeline::compute(VkCommandBuffer &cmd, uint32_t size_x,
                                  uint32_t size_y, uint32_t size_z) {

  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE,
                    m_pipeline);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE,
                          m_pipeline_layout, 0, m_descriptor_sets.count(),
                          m_descriptor_sets.get_descriptor_array().data(), 0, 0);

  vkCmdDispatch(cmd, size_x, size_y, size_z);
  
  
}

} // namespace cui::vulkan
