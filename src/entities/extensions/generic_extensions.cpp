#include "generic_extensions.h"

namespace cui::entities::extensions {

void drawable::set_pipeline(const renderer::pipeline_label pipeline_label) {
  if (m_mesh != nullptr) {
    ASSERT(renderer::pipelines_data.at(pipeline_label).geometry_type !=
               m_mesh->get_geometry(),
           "A geometry of type N: " + std::to_string(m_mesh->get_geometry()) +
               " cant be draw with a pipeline of type N: " +
               std::to_string(
                   renderer::pipelines_data.at(pipeline_label).geometry_type),
           TEXT_COLOR_ERROR);
  }
  m_pipeline_label = pipeline_label;
}

void drawable::draw() {
  ASSERT(m_mesh == nullptr &&
             !renderer::pipelines_data.at(m_pipeline_label).has_default_model,
         "Error, drawable with a null mesh has tried to be draw in a pipeline "
         "without 'has_default_model', if the pipeline does contain a default "
         "model set this to true in pipeline_label.h",
         TEXT_COLOR_ERROR);
  
  
  if(m_pipeline_label == renderer::CUI_PIPELINE_ENUM_MAX)
    return;
  
  if (m_should_draw == CUI_SHOULD_DRAW_ALWAYS) {
    m_transform->update_main_matrix();
    renderer::get_current_scene()->create_draw_call(
        m_mesh, m_pipeline_label, m_material, &m_transform->get_main_matrix());
  } else if (m_should_draw == CUI_SHOULD_DRAW_NEVER) {
  } else if (m_should_draw == CUI_SHOULD_DRAW_FUNCTION) {
    // TODO
    if (false) {
      m_transform->update_main_matrix();
      renderer::get_current_scene()->create_draw_call(
          m_mesh, m_pipeline_label, m_material, &m_transform->get_main_matrix());
    }
  }
}

  

} // namespace cui::entities::extensions