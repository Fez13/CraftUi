#pragma once
#include <cstdlib>
#include <unordered_map>
#include <iostream>
namespace cui::renderer::pipeline_info {

using pipeline_label = uint16_t;
enum : pipeline_label {
  CUI_PIPELINE_RASTERIZATION_DEFAULT,
  CUI_PIPELINE_RASTERIZATION_2D_DEFAULT,
  CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT,
  CUI_PIPELINE_RASTERIZATION_SHADOW_DEFAULT,
  CUI_PIPELINE_ENUM_MAX = __UINT16_MAX__,
};

using pipeline_type = uint16_t;
enum : pipeline_type {
  CUI_PIPELINE_TYPE_2D,
  CUI_PIPELINE_TYPE_3D,
  CUI_PIPELINE_SHADOW,
  CUI_PIPELINE_TYPE_NULL,
  CUI_PIPELINE_TYPE_ENUM_MAX = __UINT16_MAX__,
};

struct pipeline_data {
  const uint16_t type;
  const char *name;
  const size_t push_constant_size;
  const bool uses_special_draw_calls;
  const bool ubiquitous;
};

const std::unordered_map<pipeline_label, pipeline_data> pipelines_data = {
    {CUI_PIPELINE_RASTERIZATION_DEFAULT,
     {CUI_PIPELINE_TYPE_3D, "CUI_PIPELINE_RASTERIZATION_DEFAULT", 128, false,
      false}},

    {CUI_PIPELINE_RASTERIZATION_2D_DEFAULT,
     {CUI_PIPELINE_TYPE_2D, "CUI_PIPELINE_RASTERIZATION_2D_DEFAULT", 128, false,
      false}},

    {CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT,
     {CUI_PIPELINE_TYPE_2D, "CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT", 148,
      true, false}},

    {CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT,
     {CUI_PIPELINE_TYPE_2D, "CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT", 148,
      true, false}},
    
    {CUI_PIPELINE_RASTERIZATION_SHADOW_DEFAULT,
     {CUI_PIPELINE_TYPE_3D, "CUI_PIPELINE_RASTERIZATION_SHADOW_DEFAULT", 128,
      true, true}}};

} // namespace cui::renderer::pipeline_info
