#pragma once
#include "../../core/macros.h"
#include "extension_labels.h"

namespace cui::entities::extensions {

/*
  @brief Mother class of all extensions.
*/
struct extension {
public:
  virtual bool destroy() {
    LOG("Warning, this is a virtual destructor for a extension, this should be "
        "overwritten, this may be a memory leak.",
        TEXT_COLOR_WARNING);
    return true;
  }

  virtual const extension_label get_label() const {
    LOG("Warning, this is a virtual function for a extension, this should be "
        "overwritten, this may be a memory leak.",
        TEXT_COLOR_WARNING);
    return CUI_EXTENSION_NULL;
  }

  virtual const bool no_discard() const { return false; }

  // Needs to be of class entity
  void *attachment;
};
} // namespace cui::entities::extensions
