#pragma once
#include "../core/macros.h"
#include "extensions/extension.h"
#include "extensions/extension_labels.h"
#include <type_traits>
#include <unordered_map>
#include <vector>

template <class Base, class Derivate> static constexpr bool convertible() {
  return std::is_convertible_v<Derivate *, Base *>;
}

namespace cui::entities {
class entity {
public:
  static entity *create() {
    s_entities.emplace_back();
    return &s_entities[s_entities.size() - 1];
  }

  static void destroy(entity *entity) {
    for (auto &[label, extension] : entity->extensions) {
      if (extension->no_discard())
        continue;

      if (extension->destroy())
        LOG("Error, extension N: " + std::to_string(extension->get_label()) +
                " Was not destroyed, if this is intended set define "
                "function no_discard() as return true.",
            TEXT_COLOR_WARNING);
    }
    for (uint32_t i = 0; i < s_entities.size(); i++) {
      if (&s_entities[i] == entity) {
        s_entities.erase(s_entities.begin() + i);
      }
    }
  }

  /*
  @brief Will return a pointer to the requested extension if it's in the entity,
  other wise will return nullptr.
  @tparam T MUST be a derive class from extensions::extension, if not it will
  throw an error at compile time.
  */
  template <class T> T *get_extension() {

    const bool error = !convertible<extensions::extension, T>();
    ASSERT(error,
           "Error get_extension template argument must be a derived class from "
           "extension.",
           TEXT_COLOR_ERROR);

    if (extensions.contains(T::c_label))
      return static_cast<T *>(extensions[T::c_label]);
    return nullptr;
  }

  std::unordered_map<extensions::extension_label, extensions::extension *>
      extensions;

private:
  inline static std::vector<entity> s_entities = {};
};

#define create_extension_data(label, this_extension_class)                     \
  inline static entities::extensions::extension_label c_label = label;         \
  const extension_label get_label() const override { return c_label; }         \
  inline static std::vector<this_extension_class> s_instances = {};

template <class T> bool remove_from_vector(T *obj, std::vector<T> &vector) {
  uint32_t i = 0;
  for (const auto &t : vector) {
    if (&t == obj) {
      vector.erase(vector.begin() + i);
      return false;
    }
    i++;
  }
  return true;
}

/*
@brief Will attach an extension to the entity if the entity already contains the
extension it will return a pointer to it.
@tparam T MUST be a derive class from extensions::extension, if not it will
throw an error at compile time.
*/
template <class T, typename... args>
T *attach_extension(entity *entt, args... initialization_data) {
  const bool error = !convertible<extensions::extension, T>();
  ASSERT(error,
         "Error extension template argument must be a derived class from "
         "extension.",
         TEXT_COLOR_ERROR);

  if (entt->extensions.contains(T::c_label)) {
    LOG("Entity already has extension: N " + std::to_string(T::c_label) +
            "... If you want to replace the extension with a new one use "
            "replace_extension instead",
        TEXT_COLOR_NOTIFICATION);
  }
  T::s_instances.emplace_back(initialization_data...);
  T *p_extension = &T::s_instances[T::s_instances.size() - 1];
  entt->extensions[T::c_label] = p_extension;
  p_extension->attachment = static_cast<void *>(entt);
  return p_extension;
}

} // namespace cui::entities
