#include "app.h"
namespace cui {

void App::clean() {}

exit_state App::execute() {
  

  

  return CUI_EXIT_CLOSE;
}
void App::initialize() {
  LOG("App initialization started." , TEXT_COLOR_NOTIFICATION);
  GET_ELAPSED();
  vulkan::initialize_vulkan();
  vulkan::testing();
  
  
  
  

  
  LOG("App initialization finished at : " + std::to_string(GET_ELAPSED()),TEXT_COLOR_NOTIFICATION);
}
} // namespace cui
