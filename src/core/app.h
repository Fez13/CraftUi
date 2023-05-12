#pragma once
#include <iostream>
#include <memory>
#include <string>

#include"macros.h"

#include"../renderer/renderer.h"
#include"../vulkan/vulkan.h"

namespace cui {

using exit_state = uint32_t;

enum : exit_state {
  CUI_EXIT_ERROR = 1,
  CUI_EXIT_CLOSE = 2,
  CUI_EXIT_RESTART = 3,
  CUI_EXIT_STATE_NULL = 0
};

class App {
public:
  App(){}
  void initialize();

  // Returns EXIT_FAILURE if crash, EXIT_SUCCESS if close and
  exit_state execute();

  void clean();
private:
};

} // namespace cui
