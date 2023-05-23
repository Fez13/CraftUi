#pragma once

#include "vk_device.h"
#include "vk_graphic_device.h"
#include "vk_image.h"
#include "vk_utils.h"

namespace cui::vulkan {

using swap_chain_image = std::pair<VkImage, bool>;


/*
  @brief This class will be mainly used by windows, there is no need for the final user to get here.
*/
class vk_swap_chain {
public:
  /*
  @warning vk_device must not be nullptr, will crash.
  */
  void initialize(vk_device *device, const uint32_t image_count);
  
  void set_surface(VkSurfaceKHR &surface){m_surface = surface;}
  
  uint32_t get_queue_family() const { return m_queue_family; }

  void create_swap_chain();

  void set_image_size(const uint32_t width, const uint32_t height) {
    m_width = width;
    m_height = height;
  }

  void create_depth_frame_buffer(vk_image *depth_image,
                                 VkRenderPass &render_pass);

  VkFormat get_format() const { return m_surface_formate.format; }

  VkExtent2D get_extent() const { return m_extent; }

  void get_next_image(uint32_t &image_index, VkSemaphore finish,
                      VkImageView &image_view);

  VkFramebuffer get_frame_buffer(const uint32_t index) const {
    return m_depth_frame_buffers[index];
  }

  uint32_t get_image_count() const { return m_images_count; }

  uint32_t find_queue_family(); //TODO:

  void free();

  void update_swap_chain();
  void update_images();

private:
  void clear_frame_buffer();
  void clear_images();
  void clear_image_views();
  void create_proprieties();


  struct proprieties {
    VkSurfaceCapabilitiesKHR surface_capabilities;
    std::vector<VkSurfaceFormatKHR> surface_formats;
    std::vector<VkPresentModeKHR> present_modes;
    bool populated = false;
  } m_proprieties;

  VkSurfaceFormatKHR m_surface_formate;
  VkPresentModeKHR m_present_mode;
  VkExtent2D m_extent;
  uint32_t m_images_count;
  VkSwapchainKHR m_swap_chain;

  std::vector<VkImage> m_images;
  std::vector<VkImageView> m_image_views;
  std::vector<VkFramebuffer> m_depth_frame_buffers;

  std::vector<uint32_t> m_queue_families;

  uint32_t m_width = 1;
  uint32_t m_height = 1;

  VkSurfaceKHR m_surface;
  VkSharingMode m_sharing_mode;
  uint32_t m_queues_indices_count;
  uint32_t *m_queue_family_indices = nullptr;
  uint32_t m_queue_family = UINT32_MAX;

  vk_device *m_device;
};
} // namespace cui::vulkan
