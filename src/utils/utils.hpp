#pragma once
#include "../core/macros.h"

namespace cui::util {

template <class T> struct optional_parameter {
public:
  optional_parameter(const T optional_data) {
    data = optional_data;
    m_exists = true;
  }

  optional_parameter() { m_exists = false; }

  T *get() {
    if (m_exists)
      return &data;
    return nullptr;
  }

  T get_copy() {
    ASSERT(!m_exists,
           "Default argument not initialized can't be copied. Consult for "
           "exists before copying optional parameter.",
           TEXT_COLOR_ERROR);
    return data;
  }

  T get_copy_else(const T else_arg) {
    if (m_exists)
      return data;
    return else_arg;
  }

  const bool exists() { return exists; }

private:
  bool m_exists = false;
  T data;
};

} // namespace cui::util
