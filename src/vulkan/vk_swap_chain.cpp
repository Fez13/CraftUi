#include "vk_swap_chain.h"

namespace cui::vulkan {
void vk_swap_chain::initialize(vk_device *device, const uint32_t image_count) {
  m_device = device;
  m_images_count = image_count;
  create_swap_chain();
}

void vk_swap_chain::create_swap_chain() {
  ASSERT(m_device == nullptr,
         "Error you need to initialize the swap chain before creating it.",
         TEXT_COLOR_ERROR);
  ASSERT(m_surface == VK_NULL_HANDLE,
         "Error you need to initialize the swap chain before creating it.",
         TEXT_COLOR_ERROR);
         
  create_proprieties();
  find_queue_family();
  m_device->add_queue({"GRAPHIC", get_queue_family()});
  m_queue_families = vk_device_manager::get().get_all_used_families();


  bool presents_feature[2] = {false, false};

  for (const auto &format : m_proprieties.surface_formats)
    if (format.format == VK_FORMAT_R8G8B8A8_SRGB &&
        format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      m_surface_formate = format;
      presents_feature[0] = true;
    }

  if (!presents_feature[0]) {
    m_surface_formate = m_proprieties.surface_formats[0];
    LOG("A not optimal format has been chose for a swap chain",
        TEXT_COLOR_WARNING);
  }
  
  for (const auto &presentMode : m_proprieties.present_modes)
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      m_present_mode = presentMode;
      presents_feature[1] = true;
    }

  if (!presents_feature[1]) {
    m_present_mode = VK_PRESENT_MODE_FIFO_KHR;
    LOG("A not optimal present mode has been chose for a swap chain",
        TEXT_COLOR_WARNING);
  }


  update_swap_chain();
}

uint32_t vk_swap_chain::find_queue_family() {
  if (m_queue_family != UINT32_MAX)
    return m_queue_family;

  uint32_t family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(
      vk_graphic_device::get().get_device(), &family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(
      vk_graphic_device::get().get_device(), &family_count,
      queue_families.data());

  for (uint32_t i = 0; i < queue_families.size(); i++) {
    VkBool32 presenting_support = VK_FALSE;

    if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      vkGetPhysicalDeviceSurfaceSupportKHR(
          vk_graphic_device::get().get_device(), i, m_surface,
          &presenting_support);

    if (presenting_support == VK_FALSE)
      continue;
    m_queue_family = i;
    return m_queue_family;
  }
  ASSERT(true, "There is not queue family with requested capabilities",
         TEXT_COLOR_ERROR)
  return 0;
}

void vk_swap_chain::create_proprieties() {
  VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
               vk_graphic_device::get().get_device(), m_surface,
               &m_proprieties.surface_capabilities),
           "Error creating swap chain capabilities");

  uint32_t format_count = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
               vk_graphic_device::get().get_device(), m_surface, &format_count,
               nullptr),
           "Error creating swap chain capabilities");

  m_proprieties.surface_formats.resize(format_count);
  VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
               vk_graphic_device::get().get_device(), m_surface, &format_count,
               m_proprieties.surface_formats.data()),
           "Error creating swap chain capabilities");

  uint32_t present_mode_count = 0;
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
               vk_graphic_device::get().get_device(), m_surface,
               &present_mode_count, nullptr),
           "Error creating swap chain capabilities");

  m_proprieties.present_modes.resize(present_mode_count);
  VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
               vk_graphic_device::get().get_device(), m_surface,
               &present_mode_count, m_proprieties.present_modes.data()),
           "Error creating swap chain capabilities");
}

VkPresentInfoKHR vk_swap_chain::get_submit_image_info(const uint32_t &index,
                                                      VkSemaphore wait[]) {
  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = wait;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &m_swap_chain;
  presentInfo.pImageIndices = &index;
  presentInfo.pResults = nullptr;
  presentInfo.pNext = nullptr;
  return presentInfo;
}

void vk_swap_chain::update_swap_chain() {
  clear_image_views();
  create_proprieties();
  if (m_proprieties.surface_capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    m_extent = m_proprieties.surface_capabilities.currentExtent;
  } else {
    // If the screen coordinates and the screen pixels do not match, we have to
    // fix the error

    VkExtent2D extent2D{static_cast<uint32_t>(m_height),
                        static_cast<uint32_t>(m_width)};

    m_extent.width = std::clamp(
        extent2D.width, m_proprieties.surface_capabilities.minImageExtent.width,
        m_proprieties.surface_capabilities.maxImageExtent.width);

    m_extent.height =
        std::clamp(extent2D.height,
                   m_proprieties.surface_capabilities.minImageExtent.height,
                   m_proprieties.surface_capabilities.maxImageExtent.height);
    LOG("Not optimal extent has been chose", TEXT_COLOR_WARNING);
  }

  // Sets a number of images in the swap chain
  uint32_t image_count = m_images_count;

  if (m_images_count == 0)
    image_count = m_proprieties.surface_capabilities.minImageCount;
  else if (m_images_count > m_proprieties.surface_capabilities.maxImageCount &&
           m_proprieties.surface_capabilities.maxImageCount != 0)
    image_count = m_proprieties.surface_capabilities.minImageCount;
  else if (m_images_count < m_proprieties.surface_capabilities.minImageCount)
    image_count = m_proprieties.surface_capabilities.minImageCount;

  m_images_count = image_count;

  VkSwapchainCreateInfoKHR swap_chain_create_info = vkcSwapchainCreateInfoKHR(
      m_surface, m_images_count, m_surface_formate.format,
      m_surface_formate.colorSpace, m_extent,
      m_proprieties.surface_capabilities.currentTransform, m_present_mode,m_swap_chain);

  if (m_queue_families.size() != 1) {
    swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swap_chain_create_info.queueFamilyIndexCount = m_queue_families.size();
    swap_chain_create_info.pQueueFamilyIndices = m_queue_families.data();
  } else {
    swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swap_chain_create_info.queueFamilyIndexCount = 1;
    swap_chain_create_info.pQueueFamilyIndices = m_queue_families.data();
  }
  
  VK_CHECK(vkCreateSwapchainKHR(m_device->get_device(), &swap_chain_create_info,
                                nullptr, &m_swap_chain),
           "Fail creating the swapChain");
  update_images();
}

void vk_swap_chain::update_images() {

  
  // Update images
  uint32_t image_count = 0;
  vkGetSwapchainImagesKHR(m_device->get_device(), m_swap_chain, &image_count,
                          nullptr);

  m_images.resize(image_count);
  vkGetSwapchainImagesKHR(m_device->get_device(), m_swap_chain, &image_count,
                          m_images.data());
  // Update views
  VkImageViewCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  info.format = m_surface_formate.format;

  info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  info.subresourceRange.baseMipLevel = 0;
  info.subresourceRange.levelCount = 1;
  info.subresourceRange.baseArrayLayer = 0;
  info.subresourceRange.layerCount = 1;

  // clear_image_views();
  m_image_views.resize(m_images.size());
  for (uint32_t i = 0; i < m_image_views.size(); i++)
    create_image_view(VK_IMAGE_VIEW_TYPE_2D, m_surface_formate.format, m_device,
                      m_image_views[i], m_images[i], VK_IMAGE_ASPECT_COLOR_BIT,
                      0, 0, 1, 1);
}

void vk_swap_chain::create_depth_frame_buffer(
    const VkImageView &depth_image_view, VkRenderPass &render_pass) {
  clear_frame_buffer();
  m_depth_frame_buffers.resize(m_images_count);

  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = render_pass;
  framebufferInfo.attachmentCount = 2;
  framebufferInfo.width = m_width;
  framebufferInfo.height = m_height;
  framebufferInfo.layers = 1;
  framebufferInfo.pNext = nullptr;

  for (uint32_t i = 0; i < m_depth_frame_buffers.size(); i++) {
    VkImageView attachments[2] = {m_image_views[i], depth_image_view};
    framebufferInfo.pAttachments = attachments;

    VK_CHECK(vkCreateFramebuffer(m_device->get_device(), &framebufferInfo,
                                 nullptr, &m_depth_frame_buffers[i]),
             "Error creating frame buffers...");
  }
}

void vk_swap_chain::get_next_image(uint32_t &image_index, VkSemaphore finish) {
  vkAcquireNextImageKHR(m_device->get_device(), m_swap_chain, UINT64_MAX,
                        finish, VK_NULL_HANDLE, &image_index);
}

void vk_swap_chain::clear_frame_buffer() {
  for (const auto &frame_buffer : m_depth_frame_buffers)
    vkDestroyFramebuffer(m_device->get_device(), frame_buffer, nullptr);
  m_depth_frame_buffers.clear();
}

void vk_swap_chain::clear_image_views() {
  for (const auto &image_view : m_image_views)
    vkDestroyImageView(m_device->get_device(), image_view, nullptr);
  m_image_views.clear();
}

void vk_swap_chain::clear_images() {
  for (const auto &image : m_images)
    vkDestroyImage(m_device->get_device(), image, nullptr);
  m_images.clear();
}

void vk_swap_chain::free() {
  vkDeviceWaitIdle(m_device->get_device());
  clear_frame_buffer();
  clear_image_views();
  vkDestroySwapchainKHR(m_device->get_device(), m_swap_chain, nullptr);
  vkDestroySurfaceKHR(vk_instance::get().get_instance(), m_surface, nullptr);
}

} // namespace cui::vulkan
