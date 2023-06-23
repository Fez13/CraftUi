#include"scene.h"
  
namespace cui::renderer
{

std::unique_ptr<scene> current_scene = nullptr;

uint32_t scene::create_draw_call(const mesh *mesh,
                        const pipeline_info::pipeline_label pipeline,
                        const material *material, const glm::mat4 *mat) {
#if MAP_FOR_DRAW_CALLS == true

    m_draw_calls[pipeline][mesh].count += 1;
    m_draw_calls[pipeline][mesh].materials.push_back(material);
    m_draw_calls[pipeline][mesh].transforms.push_back(mat);
#else
    m_draw_calls[pipeline].emplace_back(mesh, mat, material);
    return m_draw_calls[pipeline].size() - 1;
#endif
  }


 void scene::get_draw_calls(const pipeline_info::pipeline_label pipeline,
                      VkDrawIndexedIndirectCommand *const draw_calls,
                      glm::mat4 *const matrices, int *const material_index,uint32_t *draw_call_count) {

    // TODO:
    //  -Default mesh
    //  -MAP_FOR_DRAW_CALLS

    // Based on what chat-gpt said, i will create a batches and the copy all to
    // the gpu at the same time

    // These batches should probably be created only one time.
    
    if(m_draw_calls[pipeline].size() == 0)
      return;
    
    VkDrawIndexedIndirectCommand *draw_calls_batch =
        (VkDrawIndexedIndirectCommand *)std::malloc(
            sizeof(VkDrawIndexedIndirectCommand) *
            m_draw_calls[pipeline].size());

    glm::mat4 *matrix_batch = (glm::mat4 *)std::malloc(
        sizeof(glm::mat4) * m_draw_calls[pipeline].size());

    int *material_index_batch =
        (int *)std::malloc(sizeof(int) * m_draw_calls[pipeline].size()+1);

    for (uint32_t i = 0; i < m_draw_calls[pipeline].size(); i++) {
      const mesh *mesh = m_draw_calls[pipeline][i].c_mesh;
      draw_calls_batch[i].firstIndex = mesh->get_indices_start();
      draw_calls_batch[i].indexCount = mesh->get_indices_count();
      draw_calls_batch[i].firstInstance = i;
      draw_calls_batch[i].instanceCount = 1;
      draw_calls_batch[i].vertexOffset = mesh->get_vertices_start();

      memcpy(&matrix_batch[i], m_draw_calls[pipeline][i].c_transform,
             sizeof(glm::mat4));
      matrices[i];
      // TODO: this should use uint
      material_index_batch[i] =
          (m_draw_calls[pipeline][i].c_material == nullptr)
              ? 0
              : int(m_draw_calls[pipeline][i].c_material->get_index());
    }

    // Copy all to gpu

    memcpy(material_index, material_index_batch,
           sizeof(int) * m_draw_calls[pipeline].size());
    memcpy(matrices, matrix_batch,
           sizeof(glm::mat4) * m_draw_calls[pipeline].size());
    memcpy(draw_calls, draw_calls_batch,
           sizeof(VkDrawIndexedIndirectCommand) *
               m_draw_calls[pipeline].size());

    free(material_index_batch);
    free(matrix_batch);
    free(draw_calls_batch);
    *draw_call_count = m_draw_calls[pipeline].size();
    m_draw_calls[pipeline].clear();
  }


scene* get_current_scene(){
  return current_scene.get();
}

void create_default_scene(){
  current_scene = std::make_unique<scene>();
}

  
} // namespace cui::renderer
