#include "rasterization_pipeline.h"

namespace cui::renderer {

//TODO:
/*
  Remember to:
    Create default buffers from the renderer that creates matrix, material and other 
    universal buffers.
    
    Populate shaders before pipeline initialization.
*/
void default_rasterization_2d::initialize(vk_device *device) {
  /*
    Create a for pipeline specifics 
  */
  m_descriptor = m_descriptor_sets.create();
  m_descriptor->initialize_layout();
  m_descriptor->allocate_descriptor_set();
  
  pipeline_create_dynamic_state_info();
  pipeline_create_vertex_input_state_info<vertex_2d>();
  pipeline_create_input_assembly_state_info();
  pipeline_create_rasterization_state_info();
  pipeline_create_multisample_state_info();
  pipeline_create_color_blend_state_info();
  pipeline_create_depth_stencil_state_create_info();
  create_pipeline_layout(sizeof(camera_data));
  create_pipeline();
}

void default_rasterization_2d::initialize_buffers() {
  initialize_default_draw_buffers();
}

void default_rasterization_2d::free() {}

void default_rasterization_2d::render(VkCommandBuffer &cmd,
                                      VkRenderPass &render_pass) {}

void default_rasterization_2d::set_view_port_state(
    const VkPipelineViewportStateCreateInfo
        &universal_view_port_state_create_info) {}

void default_rasterization_2d::update_size() {}

} // namespace cui::renderer
