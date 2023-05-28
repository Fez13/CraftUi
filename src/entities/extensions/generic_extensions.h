#pragma once

#include <vector>

#include "../../vendor/glm/glm.hpp"
#include "../../vendor/glm/gtc/quaternion.hpp"
#include "../../vendor/glm/gtx/quaternion.hpp"
#include "../entity.h"
#include "extension.h"
#include "extension_labels.h"

/*
  This will include some examples of extensions that will also be used all over
  the engine. In order to create a new extension first you'll need include
  entity.h, extension.h and extension_labels.h, then go to
  src/entities/extensions/extension_labels.h : enum : extension_label and add a
  new label that is not already being used.
  Then create a class derived from the class "extension" as public, and include
  the macro, create_extension_data(label,this_extension_class) you'll need to
  give as arguments, the label you created earlier and the name of the class you
  are creating, in the first class would be "transform", this will define a
  s_instancies, c_label and a function get_label(), s_instancies will ve a
  static vector that'll contain all instancies of your extension, c_label will
  be the label you assigned to the class and get_label() is a function that will
  return c_label. You can define this by your own but *ALL EXTENSIONS MUST HAVE
  THIS VARIABLES*. Finally define virtual function destroy() which should remove
  the current extension "*this" from s_instancies, There is a helper function
  for this in entity.h that is being used in the examples below.
*/

namespace cui::entities::extensions {

struct updated_vec_3 {
public:
  updated_vec_3(const glm::vec3 new_data) : m_data(new_data) {}
  updated_vec_3() = default;

  updated_vec_3 operator+(const updated_vec_3 &other) const {
    m_updated = true;
    return updated_vec_3(m_data + other.m_data);
  }

  updated_vec_3 operator-(const updated_vec_3 &other) const {
    m_updated = true;
    return updated_vec_3(m_data - other.m_data);
  }

  updated_vec_3 operator*(const updated_vec_3 &other) const {
    m_updated = true;
    return updated_vec_3(m_data * other.m_data);
  }

  updated_vec_3 &operator+=(const updated_vec_3 &other) {
    m_data += other.m_data;
    m_updated = true;
    return *this;
  }

  updated_vec_3 &operator-=(const updated_vec_3 &other) {
    m_data -= other.m_data;
    m_updated = true;
    return *this;
  }

  updated_vec_3 &operator*=(const updated_vec_3 &other) {
    m_data *= other.m_data;
    m_updated = true;
    return *this;
  }

  const bool is_updated() {
    const bool was_updated = m_updated;
    m_updated = false;
    return was_updated;
  }

  const glm::vec3 &data() { return m_data; }

private:
  glm::vec3 m_data;
  mutable bool m_updated;
};

struct transform : public extension {

  create_extension_data(CUI_EXTENSION_TRANSFORM, transform);

  bool destroy() override {
    return remove_from_vector<transform>(this,s_instances);
  }

  void update_main_matrix() {
    const bool position = m_position.is_updated();
    const bool rotation = m_rotation.is_updated();
    const bool scale = m_scale.is_updated();

    if (!(position || rotation || scale))
      return;

    m_main_matrix = glm::translate(glm::mat4(1.0f), m_position.data());
    m_main_matrix =
        glm::rotate(m_main_matrix, glm::radians(m_rotation.data().x),
                    glm::vec3(1.0f, 0, 0));
    m_main_matrix = glm::rotate(
        m_main_matrix, glm::radians(m_rotation.data().y), {0, 1.0f, 0});
    m_main_matrix = glm::rotate(
        m_main_matrix, glm::radians(m_rotation.data().z), {0, 0, 1.0f});
    m_main_matrix = glm::scale(m_main_matrix, m_scale.data());
  }

  const glm::mat4 &get_main_matrix() { return m_main_matrix; }

private:
  glm::mat4 m_main_matrix;
  updated_vec_3 m_position;
  updated_vec_3 m_rotation;
  updated_vec_3 m_scale;
};

} // namespace cui::entities::extensions