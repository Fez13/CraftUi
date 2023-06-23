#include "rasterization_pipeline.h"
#include"../../vendor/glm/gtx/rotate_vector.hpp"
#include"../../vendor/glm/gtx/vector_angle.hpp"
#include "../../vendor/glm/gtc/matrix_transform.hpp"
namespace cui::renderer {

//TODO:
/*
  Remember to:
    Create default buffers from the renderer that creates matrix, material and other 
    universal buffers.
    
    Populate shaders before pipeline initialization.
*/
void default_rasterization_2d::initialize() {
  //Material descriptor
  material::set_descriptor_positions(&m_descriptor_sets);
  
  //Pipeline descriptor

  m_descriptor_sets.lock_array();
  
  pipeline_create_dynamic_state_info();
  pipeline_create_vertex_input_state_info<vertex_2d>();
  pipeline_create_input_assembly_state_info();
  pipeline_create_rasterization_state_info();
  pipeline_create_multisample_state_info();
  pipeline_create_color_blend_state_info();
  pipeline_create_depth_stencil_state_create_info();
  create_pipeline_layout(sizeof(camera_data));
  create_pipeline();
  initialize_default_mesh();
  
}

void default_rasterization_2d::initialize_buffers() {
  initialize_default_draw_buffers();
}

void default_rasterization_2d::free() {}

void default_rasterization_2d::render(VkCommandBuffer &cmd) {
	const VkDeviceSize offset[] = {0};
		
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
  material::bind_image_array(m_descriptor_sets.get(1));
	m_descriptor_sets.bind_all(m_pipeline_layout,cmd);
	
	//Texture::bindDefaultBuffer(m_mainDescriptorSet.get(), 1, 0);
	//Material::bindDefaultBuffer(m_mainDescriptorSet.get(), 2);
			
	//memcpy(&m_camera_data.view_matrix, m_camera_data_p.view_matrix, sizeof(glm::mat4));
	//memcpy(&projection, m_camera_data_p.projection_matrix, sizeof(glm::mat4));
	vkCmdPushConstants(cmd, m_pipeline_layout, VK_SHADER_STAGE_ALL, 0, sizeof(camera_data), &m_camera_data);
	uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
	VkDeviceSize indirect_offset = sizeof(VkDrawIndexedIndirectCommand) * 0;
	vkCmdDrawIndexedIndirect(cmd, m_draw_calls_buffer.get_buffer(), indirect_offset, 1, draw_stride);
}

//////

void default_rasterization_text::initialize() {
  //Material descriptor
  material::set_descriptor_positions(&m_descriptor_sets);
  
  //Pipeline descriptor
  vulkan::vk_descriptor_set *array = m_descriptor_sets.create();
  array->create_binding_uniform_buffer(0);
  m_descriptor_sets.lock_array();
    
  pipeline_create_dynamic_state_info();
  pipeline_create_vertex_input_state_info<vertex_2d>();
  pipeline_create_input_assembly_state_info();
  pipeline_create_rasterization_state_info();
  pipeline_create_multisample_state_info();
  pipeline_create_color_blend_state_info();
  pipeline_create_depth_stencil_state_create_info();
  create_pipeline_layout(sizeof(camera_data));
  create_pipeline();
  initialize_default_mesh();
  
}

void default_rasterization_text::initialize_buffers() {
  initialize_default_draw_buffers();
  
   m_special_data_buffer =
      vk_buffer(m_device, sizeof(rasterization_text_data) * MAX_DRAW_COUNT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_SHARING_MODE_EXCLUSIVE);
  m_special_data_buffer.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  special_draw_call_data =
      m_special_data_buffer.get_memory_location<rasterization_text_data>();
      
  m_descriptor_sets.get(2)->update_binding_uniform_buffer(0,&m_special_data_buffer,1);
}

void default_rasterization_text::free() {}

void default_rasterization_text::render(VkCommandBuffer &cmd) {
	const VkDeviceSize offset[] = {0};
		
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
  material::bind_image_array(m_descriptor_sets.get(1));
	m_descriptor_sets.bind_all(m_pipeline_layout,cmd);			
	//memcpy(&m_camera_data.view_matrix, m_camera_data_p.view_matrix, sizeof(glm::mat4));
	//memcpy(&projection, m_camera_data_p.projection_matrix, sizeof(glm::mat4));
	vkCmdPushConstants(cmd, m_pipeline_layout, VK_SHADER_STAGE_ALL, 0, sizeof(camera_data), &m_camera_data);
	uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
	VkDeviceSize indirect_offset = sizeof(VkDrawIndexedIndirectCommand) * 0;
	vkCmdDrawIndexedIndirect(cmd, m_draw_calls_buffer.get_buffer(), indirect_offset, 1, draw_stride);
}

} // namespace cui::renderer
