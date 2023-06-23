#include "renderer.h"
namespace cui::renderer {
renderer renderer::s_instance;

void renderer::update_view_port() {
  m_view_port.x = 0.0f;
  m_view_port.y = 0.0f;
  m_view_port.width = static_cast<float>(m_swap_chain->get_extent().width);
  m_view_port.height = static_cast<float>(m_swap_chain->get_extent().height);
  m_view_port.minDepth = 0.0f;
  m_view_port.maxDepth = 1.0f;

  m_scissor.offset = {0, 0};
  m_scissor.extent = m_swap_chain->get_extent();

  m_universal_view_port_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  m_universal_view_port_state.viewportCount = 1;
  m_universal_view_port_state.scissorCount = 1;
  m_universal_view_port_state.pViewports = &m_view_port;
  m_universal_view_port_state.pScissors = &m_scissor;
}

// TODO
#define DEPTH_IMAGE_FORMAT VK_FORMAT_D32_SFLOAT

void renderer::update_extents() {
  m_device->wait_to_finish("GRAPHIC");
  if (m_depth_image.get_image() != nullptr) {
    m_depth_image.free();
  }

  m_depth_image = vulkan::vk_image(
      "DEVICE_KHR", VK_IMAGE_TILING_OPTIMAL, DEPTH_IMAGE_FORMAT,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT,
      m_swap_chain->get_extent().width, m_swap_chain->get_extent().height);

  update_view_port();
/* TODO
  for (auto &[label, pipeline] : m_occasional_pipelines) {
    pipeline->set_viewport_state(&m_universal_view_port_state);
    pipeline->update_size();
  }
*/
  for (auto &[label, pipeline] : m_ubiquitous_pipelines) {
    pipeline->set_viewport_state(&m_universal_view_port_state);
    pipeline->update_size();
  }

  m_swap_chain->create_depth_frame_buffer(m_depth_image.get_image_view(),
                                          m_draw_render_pass);
}

void renderer::create_render_passes() {
  // Draw render pass
  {

    VkAttachmentDescription attachments_depth =
        vulkan::vkcAttachmentDescription(
            DEPTH_IMAGE_FORMAT, VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    VkAttachmentDescription attachments_draw = vulkan::vkcAttachmentDescription(
        m_swap_chain->get_format(), VK_ATTACHMENT_LOAD_OP_CLEAR,
        VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    std::vector<VkAttachmentDescription> descriptions = {attachments_draw,
                                                         attachments_depth};

    VkAttachmentReference color_attachment{};
    color_attachment.attachment = 0;
    color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment{};
    depth_attachment.attachment = 1;
    depth_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<VkSubpassDescription> sub_pass_descriptor = {{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment,
        .pDepthStencilAttachment = &depth_attachment,
    }};

    std::vector<VkSubpassDependency> sub_pass_dependencies;
    sub_pass_dependencies.emplace_back();
    sub_pass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    sub_pass_dependencies[0].dstSubpass = 0;
    sub_pass_dependencies[0].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_pass_dependencies[0].srcAccessMask = 0;
    sub_pass_dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_pass_dependencies[0].dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_create_info =
        vulkan::vkRenderPassCreateInfo(descriptions, sub_pass_descriptor,
                                       sub_pass_dependencies);
    VK_CHECK(vkCreateRenderPass(m_device->get_device(),
                                &render_pass_create_info, nullptr,
                                &m_draw_render_pass),
             "Error creating a render pass.");
  }
  // Shadow render pass
  {

    std::vector<VkAttachmentDescription> descriptions = {
        vulkan::vkcAttachmentDescription(
            SHADOW_PASS_DEPTH_FORMAT, VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)};

    VkAttachmentReference depth_attachment = {};
    depth_attachment.attachment = 0;
    depth_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<VkSubpassDescription> sub_pass_descriptor = {
        {.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
         .colorAttachmentCount = 0,
         .pDepthStencilAttachment = &depth_attachment}};

    std::vector<VkSubpassDependency> sub_pass_dependencies;
    sub_pass_dependencies.emplace_back();
    sub_pass_dependencies.emplace_back();

    sub_pass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    sub_pass_dependencies[0].dstSubpass = 0;
    sub_pass_dependencies[0].srcStageMask =
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sub_pass_dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    sub_pass_dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    sub_pass_dependencies[0].dstAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    sub_pass_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    sub_pass_dependencies[1].srcSubpass = 0;
    sub_pass_dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    sub_pass_dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    sub_pass_dependencies[1].dstStageMask =
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sub_pass_dependencies[1].srcAccessMask =
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    sub_pass_dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    sub_pass_dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo render_pass_create_info =
        vulkan::vkRenderPassCreateInfo(descriptions, sub_pass_descriptor,
                                       sub_pass_dependencies);

    VK_CHECK(vkCreateRenderPass(m_device->get_device(),
                                &render_pass_create_info, nullptr,
                                &m_shadow_render_pass),
             "Error creating a render pass.");
  }
}

void renderer::create_semaphore() {
  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(m_device->get_device(), &semaphoreCreateInfo, nullptr,
                    &m_image_wait);
  vkCreateSemaphore(m_device->get_device(), &semaphoreCreateInfo, nullptr,
                    &m_render_wait);
}

void renderer::initialize(window *window) {
  ASSERT(window == nullptr,
         "Error, window can't be a nullptr, window may not be initialized.",
         TEXT_COLOR_ERROR);
  ASSERT(!vulkan::vk_device_manager::get().contains("DEVICE_KHR"),
         "Error, a swap chain must be initialized before creating a renderer.",
         TEXT_COLOR_ERROR);
  m_window = window;
  m_swap_chain = window->get_swap_chain();

  // Get device data and queue
  m_device = vulkan::vk_device_manager::get().get_device("DEVICE_KHR");
  m_graphic_queue = m_device->get_queue("GRAPHIC");

  create_render_passes();
  create_semaphore();
  update_extents();

  m_device->create_fence(VK_FENCE_CREATE_SIGNALED_BIT);
  m_command_buffer = m_device->create_command_buffers(
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1,
      m_device->get_queue("GRAPHIC").command_pool),
      
  m_default_buffer = vulkan::vk_buffer(m_device,sizeof(default_buffer_data),VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_SHARING_MODE_EXCLUSIVE);
  m_default_buffer.initialize_buffer_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  m_default_buffer.get_memory_location<default_buffer_data>();
  LOG("Renderer initialization done!", TEXT_COLOR_NOTIFICATION);
}

void renderer::draw() {
  m_counter++;
  // Check for window size changes
  if (m_window->get_update()) {
    update_extents();
  }

  

  //Update default buffer
  
  default_buffer_data* data = m_default_buffer.get_memory_location<default_buffer_data>();
  data->frame_rate = m_window->get_swap_chain()->get_refresh_rate();
  data->window_height = m_window->get_swap_chain()->get_extent().height;
  data->window_width = m_window->get_swap_chain()->get_extent().width;
  data->time = m_window->get_time();
  data->counter = m_counter; //TODO random number


  uint32_t image_index = 0;
  m_window->get_swap_chain()->get_next_image(image_index, m_image_wait);

  vkWaitForFences(m_device->get_device(), 1, &m_device->get_fence(), VK_TRUE,
                  UINT64_MAX);
  vkResetFences(m_device->get_device(), 1, &m_device->get_fence());

  vkResetCommandBuffer(m_command_buffer, 0);
  m_device->reset_command_pool("GRAPHIC");
  record_command_buffer(image_index);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {m_image_wait};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &m_command_buffer;

  VkSemaphore signalSemaphores[] = {m_render_wait};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  VK_CHECK(vkQueueSubmit(m_device->get_queue("GRAPHIC").queue, 1, &submitInfo,
                         m_device->get_fence()),
           "Fail submitting work to the gpu");

  VkPresentInfoKHR presentInfoKhr =
      m_window->get_swap_chain()->get_submit_image_info(image_index,
                                                        signalSemaphores);

  vkQueuePresentKHR(m_device->get_queue("GRAPHIC").queue, &presentInfoKhr);
}

void renderer::begin_render_pass(const uint32_t index) {
  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = m_draw_render_pass;
  render_pass_info.framebuffer =
      m_window->get_swap_chain()->get_frame_buffer(index);

  // Should be dynamic
  render_pass_info.renderArea.offset = {0, 0};
  render_pass_info.renderArea.extent = m_window->get_swap_chain()->get_extent();

  VkClearValue clear_values[2] = {};
  clear_values[0].color = {m_clear_color.x,m_clear_color.y,m_clear_color.z,m_clear_color.w};
  clear_values[1].depthStencil = {1.0f, 0};

  render_pass_info.clearValueCount = 2;
  render_pass_info.pClearValues = clear_values;

  vkCmdBeginRenderPass(m_command_buffer, &render_pass_info,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void renderer::record_command_buffer(const uint32_t index) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  VK_CHECK(vkBeginCommandBuffer(m_command_buffer, &beginInfo),
           "Error recording command buffer, index: " + std::to_string(index));

  for (auto &&[label, pipeline] : m_ubiquitous_pipelines) {
    get_current_scene()->get_draw_calls(
        label, pipeline->m_draw_calls_mapped,
        pipeline->m_matrices_array_mapped->matrices,
        pipeline->m_material_index_mapped->index, &pipeline->m_draw_call_count);
  }
  begin_render_pass(index);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(m_swap_chain->get_extent().width);
  viewport.height = static_cast<float>(m_swap_chain->get_extent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_command_buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = m_swap_chain->get_extent();
  vkCmdSetScissor(m_command_buffer, 0, 1, &scissor);

  VkDeviceSize offset = 0;

  get_current_scene()->scene_mesh_manager.bind_indexes(m_command_buffer);
  for (auto &&[label, pipeline] : m_ubiquitous_pipelines) {
    if (pipelines_data.at(label).geometry_type == CUI_GEOMETRY_TYPE_2D) {
      get_current_scene()->scene_mesh_manager.bind_vertices_2d(m_command_buffer,
                                                               &offset);
    } else if (pipelines_data.at(label).geometry_type == CUI_GEOMETRY_TYPE_3D) {
      get_current_scene()->scene_mesh_manager.bind_vertices_3d(m_command_buffer,
                                                               &offset);
    } else {
      ASSERT(true, "Error, pipeline vertex_type = null", TEXT_COLOR_ERROR);
    }
    pipeline->render(m_command_buffer);
    pipeline->m_draw_call_count = 0;
  }
  vkCmdEndRenderPass(m_command_buffer);

  VK_CHECK(vkEndCommandBuffer(m_command_buffer),
           "Error recording command buffer");
}

} // namespace cui::renderer

/* //TODO
  Scene ->
                     for existing pipeline      for existing mesh
  std::unordered_map<pipeline_label,std::vector<mesh_draw_calls>>

  struct mesh_draw_calls{
    uint32_t count;
    std::vector<uint32_t> matrix;
    std::vector<uint32_t> texture;
    std::vector<Transform*> transforms; //<- upload to gpu
    std::vector<vk_buffer*> special_data;
  };


*/