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

void renderer::update_extents() {
  if (m_depth_image.get_image() != nullptr)
    m_depth_image.free();

  m_depth_image = vulkan::vk_image(
      "DEVICE_KHR", VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_D32_SFLOAT,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT,
      m_swap_chain->get_extent().width, m_swap_chain->get_extent().height);

  update_view_port();

  for (auto &[label, pipeline] : m_occasional_pipelines) {
    pipeline->set_view_port_state(m_universal_view_port_state);
    pipeline->update_size();
  }
  for (auto &[label, pipeline] : m_ubiquitous_pipelines) {
    pipeline->set_view_port_state(m_universal_view_port_state);
    pipeline->update_size();
  }
}

void renderer::create_render_passes() {
  // Draw render pass
  {

    VkAttachmentDescription attachments_depth =
        vulkan::vkcAttachmentDescription(
            m_depth_image.get_format(), VK_ATTACHMENT_LOAD_OP_CLEAR,
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

  update_extents();
  create_render_passes();

  m_swap_chain->create_depth_frame_buffer(m_depth_image.get_image_view(),
                                          m_draw_render_pass);
  m_device->create_fence(VK_FENCE_CREATE_SIGNALED_BIT);
  LOG("Renderer initialization done!", TEXT_COLOR_NOTIFICATION);
}

void renderer::set_default_descriptor_positions(vulkan::vk_descriptor_set_array* dsta){
  vulkan::vk_descriptor_set* dst = dsta->create();
  dst->create_binding_uniform_buffer(0,VK_SHADER_STAGE_ALL); //Default buffer
  dst->create_binding_uniform_buffer(1,VK_SHADER_STAGE_ALL); //Transforms
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