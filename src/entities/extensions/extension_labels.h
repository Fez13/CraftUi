#pragma once
#include <iostream>

namespace cui::entities::extensions {
using extension_label = uint8_t;
enum : extension_label {
  CUI_EXTENSION_DRAWABLE,
  CUI_EXTENSION_TRANSFORM,
  CUI_EXTENSION_NULL = UINT8_MAX
};
} // namespace cui::entities::extensions
