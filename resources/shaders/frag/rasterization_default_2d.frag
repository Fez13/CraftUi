#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec4 frag_color_in;
layout(location = 1) in vec2 texture_cordinate_in;
layout(location = 2) in vec3 fragment_world_position_in;
layout(location = 3) in flat int instance_index_in;

layout(location = 0) out vec4 vert_color_out;

layout(push_constant) uniform push_data_buffer {
  mat4 projection;
  mat4 view;
}
push_data;

// Set 0 : default renderer buffers

layout(set = 0, binding = 0) uniform default_buffer_buffer {
  int window_height;
  int window_width;
  float time;
  int frame_rate;
  float random_number;
}
default_buffer;

layout(set = 0, binding = 1) uniform default_matrix_array_buffer {
  mat4 matrices[1000000];
}
default_matrix_array;

// Set 1 : material system

layout(set = 1, binding = 0) uniform sampler material_image_sampler;

layout(set = 1, binding = 1) uniform texture2D material_texture_array[];

struct material_data {
  int albedo_texture_index;
  int metallic_texture_index;
  int normal_texture_index;
  float metallic_strength;
  vec4 albedo_color;
};

layout(std430, set = 1, binding = 2) uniform material_material_array_buffer {
  material_data data[1000];
}
material_material_array;

layout(set = 1,
       binding = 3) uniform material_material_object_index_buffer {
  int materialIndex[1000000];
}
material_material_object_index;

// Pipeline set

//

void main() { vert_color_out = vec4(1, 1, 1, 1); }
