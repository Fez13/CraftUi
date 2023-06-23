#pragma once

#include "../renderer/window.h"
#include "../vulkan/vk_device.h"
#include "../vulkan/vk_image.h"
#include "../vulkan/vk_instance.h"
#include "../vulkan_config/vulkan_config.hpp"
#include "glfw_utils.h"

namespace cui::glfw {

// Window and swap chain abstraction for windows, mac and linux
class glfw_window : public renderer::window {

public:
  glfw_window() = default;
  void set_window_size(const uint32_t width, const uint32_t height,
                       const bool update_glfw_window = true);

  /*
    @brief Updates the window refresh rate.
  */
  void set_window_refresh_rate(const uint32_t rate);
  
  /*
   Will initialize the window, this shall only be called 1 time.
   @brief This function will check for a device named DEVICE_KHR, if it finds
   one, it will set it as it's main device, otherwise it will create a device
   by that name with all necessary features.
   @warning If there is a device named DEVICE_KHR without he necessary
   capabilities there will be unexpected behavior.
   @param all Takes the default width,height and refresh_rate of the window.
   */
  void initialize(const uint32_t width, const uint32_t height,
                  const uint32_t refresh_rate) override;

  /*
   @brief Returns the main queue family of the swap chain.
   @warning Will return UINT32_MAX if not initialized.
  */
  uint32_t get_queue_family() override {
    if (m_swap_chain.get_queue_family() == UINT32_MAX) {
      LOG("Warning null queue family was returned.", TEXT_COLOR_WARNING);
      return UINT32_MAX;
    }
    return m_swap_chain.get_queue_family();
  }

  const int get_time() const override { return glfwGetTime(); }

  void free();

  GLFWwindow *get_glfw_window() { return m_window; }

  static glfw_window *get_main() { return s_main_window; }

private:
  void create_surface();
  uint32_t m_width = 1;
  uint32_t m_height = 1;
  GLFWwindow *m_window;

  VkSurfaceKHR m_surface;

  inline static glfw_window *s_main_window = nullptr;
};
} // namespace cui::glfw
