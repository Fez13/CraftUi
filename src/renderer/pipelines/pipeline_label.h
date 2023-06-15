#pragma once
#include <cstdlib>
#include <iostream>
#include <unordered_map>
namespace cui::renderer::pipeline_info {

using pipeline_label = uint8_t;
enum : pipeline_label {
  CUI_PIPELINE_RASTERIZATION_DEFAULT,
  CUI_PIPELINE_RASTERIZATION_2D_DEFAULT,
  CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT,
  CUI_PIPELINE_RASTERIZATION_SHADOW_DEFAULT,
  CUI_PIPELINE_ENUM_MAX = UINT8_MAX,
};

using pipeline_types = uint8_t;
enum : pipeline_types {
  CUI_PIPELINE_TYPE_2D,
  CUI_PIPELINE_TYPE_3D,
  CUI_PIPELINE_SHADOW,
  CUI_PIPELINE_TYPE_NULL,
  CUI_PIPELINE_TYPE_ENUM_MAX = UINT8_MAX,
};

using geometry_types = uint8_t;
enum : geometry_types {
  CUI_GEOMETRY_TYPE_3D = 0,
  CUI_GEOMETRY_TYPE_2D = 1,
  CUI_GEOMETRY_TYPE_NULL = UINT8_MAX
};

struct pipeline_data_struct {
  const pipeline_types type;
  const geometry_types geometry_type;
  const char *name;
  const size_t push_constant_size;
  const bool uses_special_draw_calls;
  const bool ubiquitous;
  const bool has_default_model;
};

const std::unordered_map<pipeline_label, pipeline_data_struct> pipelines_data = {
    {CUI_PIPELINE_RASTERIZATION_DEFAULT,
     {CUI_PIPELINE_TYPE_3D, CUI_GEOMETRY_TYPE_3D,
      "CUI_PIPELINE_RASTERIZATION_DEFAULT", 128, false, false, false}},

    {CUI_PIPELINE_RASTERIZATION_2D_DEFAULT,
     {CUI_PIPELINE_TYPE_2D, CUI_GEOMETRY_TYPE_2D,
      "CUI_PIPELINE_RASTERIZATION_2D_DEFAULT", 128, false, false, false}},

    {CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT,
     {CUI_PIPELINE_TYPE_2D, CUI_GEOMETRY_TYPE_2D,
      "CUI_PIPELINE_RASTERIZATION_TEXT_DEFAULT", 148, true, false, false}},

    {CUI_PIPELINE_RASTERIZATION_SHADOW_DEFAULT,
     {CUI_PIPELINE_TYPE_3D, CUI_GEOMETRY_TYPE_3D,
      "CUI_PIPELINE_RASTERIZATION_SHADOW_DEFAULT", 128, true, true, false}}};

} // namespace cui::renderer::pipeline_info
