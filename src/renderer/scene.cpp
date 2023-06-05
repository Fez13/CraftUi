#include"scene.h"
  
namespace cui::renderer
{

std::unique_ptr<scene> current_scene = nullptr;

scene* get_current_scene(){
  return current_scene.get();
}

void create_default_scene(){
  current_scene = std::make_unique<scene>();
}

  
} // namespace cui::renderer
