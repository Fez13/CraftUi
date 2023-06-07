#pragma once

#include "vk_descriptor_set.h"
#include "vk_shader.h"
#include "vk_utils.h"

#include <functional>
namespace cui::vulkan {

class vk_compute_pipeline {
public:
  explicit vk_compute_pipeline(vk_device *device)
      : m_device(device), m_descriptor_sets(device) {}
  void
  create_descriptor(std::function<void(vk_descriptor_set_array *)> function);
  vk_descriptor_set *get_descriptor(const uint32_t set) {
    return m_descriptor_sets.get(set);
  }
  void initialize(vk_shader &shader);
  void free();
  void compute(VkCommandBuffer &cmd, const uint32_t size_x,
               const uint32_t size_y, const uint32_t size_z);

private:
  VkPipeline m_pipeline;
  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device = nullptr;
  VkSemaphore m_compute_semaphore = nullptr;
};

// This is a virtual class for a rasterization pipeline.
class vk_rasterization_pipeline {
public:

  virtual void initialize() {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  virtual void free() {
    LOG("Warning this is a call from a virtual function, "
        "this should be overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  virtual void render(VkCommandBuffer &cmd) {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  void set_view_port_state(const VkPipelineViewportStateCreateInfo
                                       &universal_view_port_state_create_info) {
    m_pipeline_create_info.pViewportState = &universal_view_port_state_create_info;
  }

  virtual void update_size() {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }

  void set_fragment_shader(const std::string path){
    m_fragment_shader = vk_graphic_shader(path,m_device,VK_SHADER_STAGE_FRAGMENT_BIT);
  }
  
  void set_vertex_shader(const std::string path){
    m_vertex_shader = vk_graphic_shader(path,m_device,VK_SHADER_STAGE_VERTEX_BIT);
  }

  void set_dynamic_states(const std::vector<VkDynamicState> dynamic_states) {
    m_dynamic_states = dynamic_states;
  }
  
  void set_vulkan_device(vk_device *device){
    m_device = device;
  }

  void set_render_pass(const VkRenderPass& render_pass){
    m_render_pass = render_pass;
  }
  
  
protected:
  VkPipeline m_pipeline;

  virtual void initialize_default_mesh() {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }

  virtual void initialize_buffers() {
    LOG("Warning this is a call from a virtual function, this should be "
        "overwritten by some derived class.",
        TEXT_COLOR_WARNING);
  }
  
  void initialize_default_draw_buffers();

  std::vector<VkDynamicState> m_dynamic_states{};

  vk_buffer m_draw_calls_buffer;
  VkDrawIndexedIndirectCommand *m_draw_calls_mapped;

  vk_buffer m_matrices_array_buffer;
  struct matrices {
    glm::mat4 matrices[MAX_DRAW_COUNT];
  } * m_matrices_array_mapped;

  vk_buffer m_material_index_array_buffer;
  struct material_index {
    int index[MAX_MATERIAL_COUNT];
  } * m_material_index_mapped;

  struct camera_data {
    glm::mat4 *view_matrix = nullptr;
    glm::mat4 *projection_matrix = nullptr;
  } m_camera_data;

  vk_descriptor_set_array m_descriptor_sets;
  VkPipelineLayout m_pipeline_layout = nullptr;
  vk_device *m_device;
  VkSemaphore m_compute_semaphore = nullptr;
  VkSemaphore m_window_semaphore = nullptr;
  size_t m_size_of_push_data = 0;
  
  vk_graphic_shader m_fragment_shader;
  vk_graphic_shader m_vertex_shader;
  VkPipelineShaderStageCreateInfo m_shader_stage_create_info[2];
  VkRenderPass m_render_pass = nullptr;

  // Pipeline creation

  VkPolygonMode m_polygon_mode = VK_POLYGON_MODE_FILL; // VK_POLYGON_MODE_FILL

  VkGraphicsPipelineCreateInfo m_pipeline_create_info{};

  VkPipelineDynamicStateCreateInfo m_dynamics_state_info{};
  void pipeline_create_dynamic_state_info();

  VkPipelineInputAssemblyStateCreateInfo m_assembly_state_create_info{};
  void pipeline_create_input_assembly_state_info(
      const VkPrimitiveTopology primitive_topology =
          VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      const VkBool32 primitive_restart = VK_FALSE);

  VkPipelineMultisampleStateCreateInfo m_multisample_state_create_info{};
  void pipeline_create_multisample_state_info(
      const VkBool32 sample_shading = VK_TRUE,
      const VkSampleCountFlagBits rasterization_samples = VK_SAMPLE_COUNT_1_BIT,
      const float min_sample_shading = 1);

  VkPipelineRasterizationStateCreateInfo m_rasterization_state_create_info{};
  void pipeline_create_rasterization_state_info(
      const bool culling = true, const VkBool32 depthClamp = VK_FALSE,
      const VkBool32 rasterizerDiscard = VK_FALSE,
      const float lineWidth = 1.0f);

  VkVertexInputBindingDescription m_vertex_bindings{};
  std::vector<VkVertexInputAttributeDescription> m_vertex_attributes;
  VkPipelineVertexInputStateCreateInfo m_input_state_create_info{};
  template <class vertices>
  void pipeline_create_vertex_input_state_info(
      const uint32_t vertexBindingDescriptionCount = 1) {
    m_vertex_bindings = vertices::get_binding_description();
    m_vertex_attributes = vertices::get_attribute_description();

    m_input_state_create_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    m_input_state_create_info.pNext = nullptr;
    m_input_state_create_info.pVertexAttributeDescriptions =
        m_vertex_attributes.data();
    m_input_state_create_info.pVertexBindingDescriptions = &m_vertex_bindings;
    m_input_state_create_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(m_vertex_attributes.size());
    m_input_state_create_info.vertexBindingDescriptionCount =
        vertexBindingDescriptionCount;

    m_pipeline_create_info.pVertexInputState =
        &m_input_state_create_info; // TODO:
  }

  VkPipelineColorBlendAttachmentState m_color_blend_attachment_state{};
  VkPipelineColorBlendStateCreateInfo m_color_blend_state_create_info{};
  void pipeline_create_color_blend_state_info(
      const VkBool32 blend = VK_TRUE,
      const VkBlendOp colorBlendOp = VK_BLEND_OP_ADD,
      const VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD,
      const VkBool32 logicOp = VK_FALSE);

  VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state_create_info{};
  void pipeline_create_depth_stencil_state_create_info(
      const VkBool32 depthTest = VK_TRUE, const VkBool32 depthWrite = VK_TRUE,
      const VkBool32 depthBounds = VK_FALSE);

  void set_viewport_state(const VkPipelineViewportStateCreateInfo *viewPort);
  
  void create_pipeline_layout(const size_t size_of_push);
  
  void create_pipeline();
};

} // namespace cui::vulkan
