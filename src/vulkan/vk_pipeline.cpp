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
  vkDestroyPipeline(m_device->get_device(), m_pipeline, nullptr);
  vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, nullptr);
}

void vk_compute_pipeline::compute(VkCommandBuffer &cmd, const uint32_t size_x,
                                  const uint32_t size_y,
                                  const uint32_t size_z) {

  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_pipeline);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE,
                          m_pipeline_layout, 0, m_descriptor_sets.count(),
                          m_descriptor_sets.get_descriptor_array().data(), 0,
                          0);

  vkCmdDispatch(cmd, size_x, size_y, size_z);
}

void vk_rasterization_pipeline::pipeline_create_dynamic_state_info() {
  m_dynamics_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  m_dynamics_state_info.dynamicStateCount =
      static_cast<uint32_t>(m_dynamic_states.size());
  m_dynamics_state_info.pDynamicStates = m_dynamic_states.data();
  m_pipeline_create_info.pDynamicState = &m_dynamics_state_info;
}

void vk_rasterization_pipeline::pipeline_create_input_assembly_state_info(
    const VkPrimitiveTopology primitiveTopology, VkBool32 primitiveRestart) {
  m_assembly_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  m_assembly_state_create_info.topology = primitiveTopology;
  m_assembly_state_create_info.primitiveRestartEnable = primitiveRestart;
  m_pipeline_create_info.pInputAssemblyState = &m_assembly_state_create_info;
}
void vk_rasterization_pipeline::pipeline_create_multisample_state_info(
    const VkBool32 sampleShading,
    const VkSampleCountFlagBits rasterizationSamples,
    const float minSampleShading) {
  m_multisample_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  m_multisample_state_create_info.sampleShadingEnable = sampleShading;
  m_multisample_state_create_info.rasterizationSamples = rasterizationSamples;
  m_multisample_state_create_info.minSampleShading = minSampleShading;
  m_multisample_state_create_info.pSampleMask = nullptr;
  m_multisample_state_create_info.alphaToCoverageEnable = VK_TRUE;
  m_multisample_state_create_info.alphaToOneEnable = VK_FALSE;
  m_pipeline_create_info.pMultisampleState = &m_multisample_state_create_info;
}

void vk_rasterization_pipeline::pipeline_create_rasterization_state_info(
    const bool culling, const VkBool32 depthClamp,
    const VkBool32 rasterizerDiscard, const float lineWidth) {
  m_rasterization_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  m_rasterization_state_create_info.depthClampEnable = depthClamp;
  m_rasterization_state_create_info.rasterizerDiscardEnable = rasterizerDiscard;
  m_rasterization_state_create_info.polygonMode = m_polygon_mode;
  m_rasterization_state_create_info.lineWidth = 1.0f;
  if (culling)
    m_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
  else
    m_rasterization_state_create_info.cullMode = VK_CULL_MODE_NONE;
  m_rasterization_state_create_info.frontFace = VK_FRONT_FACE_CLOCKWISE;

  // May change
  m_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
  m_rasterization_state_create_info.depthBiasConstantFactor = 0.0f; // Optional
  m_rasterization_state_create_info.depthBiasClamp = 0.0f;          // Optional
  m_rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;    // Optional
  m_pipeline_create_info.pRasterizationState =
      &m_rasterization_state_create_info;
}

void vk_rasterization_pipeline::set_viewport_state(
    const VkPipelineViewportStateCreateInfo *viewPort) {
  m_pipeline_create_info.pViewportState = viewPort;
}

void vk_rasterization_pipeline::update_size() {
  vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, nullptr);
  vkDestroyPipeline(m_device->get_device(), m_pipeline, nullptr);
  create_pipeline_layout(m_size_of_push_data);
  create_pipeline();
}

void vk_rasterization_pipeline::pipeline_create_color_blend_state_info(
    const VkBool32 blend, const VkBlendOp colorBlendOp,
    const VkBlendOp alphaBlendOp, const VkBool32 logicOp) {
  m_color_blend_attachment_state.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  m_color_blend_attachment_state.blendEnable = VK_TRUE;
  m_color_blend_attachment_state.srcColorBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  m_color_blend_attachment_state.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  m_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
  m_color_blend_attachment_state.srcAlphaBlendFactor =
      VK_BLEND_FACTOR_SRC_ALPHA;
  m_color_blend_attachment_state.dstAlphaBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  m_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

  m_color_blend_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  m_color_blend_state_create_info.logicOpEnable = VK_FALSE;
  m_color_blend_state_create_info.attachmentCount = 1;
  m_color_blend_state_create_info.pAttachments =
      &m_color_blend_attachment_state;
  m_pipeline_create_info.pColorBlendState = &m_color_blend_state_create_info;
}

void vk_rasterization_pipeline::pipeline_create_depth_stencil_state_create_info(
    const VkBool32 depthTest, const VkBool32 depthWrite,
    const VkBool32 depthBounds) {
  m_depth_stencil_state_create_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  m_depth_stencil_state_create_info.depthTestEnable = depthTest;
  m_depth_stencil_state_create_info.depthWriteEnable = depthWrite;
  m_depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
  m_depth_stencil_state_create_info.depthBoundsTestEnable = depthBounds;
  m_pipeline_create_info.pDepthStencilState =
      &m_depth_stencil_state_create_info;
}

void vk_rasterization_pipeline::initialize_default_draw_buffers() {

  m_matrices_array_buffer =
      vk_buffer(m_device, sizeof(matrices), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_SHARING_MODE_EXCLUSIVE);
  m_matrices_array_buffer.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  m_matrices_array_mapped =
      m_matrices_array_buffer.get_memory_location<matrices>();

  m_material_index_array_buffer =
      vk_buffer(m_device, sizeof(material_index),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
  m_material_index_array_buffer.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  m_material_index_mapped =
      m_material_index_array_buffer.get_memory_location<material_index>();

  m_draw_calls_buffer = vk_buffer(
      m_device, sizeof(VkDrawIndexedIndirectCommand) * MAX_DRAW_COUNT,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
      VK_SHARING_MODE_EXCLUSIVE);
  m_draw_calls_buffer.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  m_draw_calls_mapped =
      m_draw_calls_buffer.get_memory_location<VkDrawIndexedIndirectCommand>();

  vk_descriptor_set *default_descriptor = m_descriptor_sets.get(0);
  default_descriptor->update_binding_uniform_buffer(1,
                                                    &m_matrices_array_buffer);
  
  vk_descriptor_set *material_descriptor = m_descriptor_sets.get(1);
  material_descriptor->update_binding_uniform_buffer(
      3, &m_material_index_array_buffer);
}

void vk_rasterization_pipeline::create_pipeline() {
  m_pipeline_create_info.sType =
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  m_pipeline_create_info.stageCount = 2;

  m_shader_stage_create_info[0] =
      m_vertex_shader.get_shader_stage_create_info();
  m_shader_stage_create_info[1] =
      m_fragment_shader.get_shader_stage_create_info();
  m_pipeline_create_info.pStages = m_shader_stage_create_info;

  m_pipeline_create_info.layout = m_pipeline_layout;
  m_pipeline_create_info.renderPass = m_render_pass;
  m_pipeline_create_info.subpass = 0;
  m_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
  m_pipeline_create_info.basePipelineIndex = -1;

  VK_CHECK(vkCreateGraphicsPipelines(m_device->get_device(), VK_NULL_HANDLE, 1,
                                     &m_pipeline_create_info, nullptr,
                                     &m_pipeline),
           "Error creating a pipeline");
}

void vk_rasterization_pipeline::create_pipeline_layout(
    const size_t size_of_push) {
  m_size_of_push_data = size_of_push;
  VkPushConstantRange pushConstantRange{};
  pushConstantRange.stageFlags =
      VK_SHADER_STAGE_ALL; // At the moment push constants afect ALL stages
  pushConstantRange.offset = 0;
  pushConstantRange.size = size_of_push;

  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
  pipelineLayoutCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  std::vector<VkDescriptorSetLayout> layouts = m_descriptor_sets.get_layouts();
  pipelineLayoutCreateInfo.setLayoutCount = layouts.size();

  pipelineLayoutCreateInfo.pSetLayouts = layouts.data();
  pipelineLayoutCreateInfo.pushConstantRangeCount = 1; // Only one push constant
  pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

  VK_CHECK(vkCreatePipelineLayout(m_device->get_device(),
                                  &pipelineLayoutCreateInfo, nullptr,
                                  &m_pipeline_layout),
           "Fail creating a pipelineLayout");
}

} // namespace cui::vulkan
