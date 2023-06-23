#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec2 position_in;
layout(location = 1) in vec2 texture_cordinate_in;

layout(location = 0) out vec4 frag_color_out;
layout(location = 1) out vec2 texture_cordinate_out;
layout(location = 2) out vec3 fragment_world_position_out;
layout(location = 3) out flat int instance_index_out;

layout(push_constant) uniform push_data_buffer {
  mat4 projection;
  mat4 view;
}
push_data;

layout(set = 0, binding = 0) uniform default_buffer_buffer {
  int window_height;
  int window_width;
  float time;
  uint frame_rate;
  uint counter;
} default_buffer;

// Set 0 : default renderer buffers
layout(set = 0, binding = 1) uniform default_matrix_array_buffer {
  mat4 matrices[];
}
default_matrix_array;

struct special_draw_data{
  vec4 data;
};

layout(set = 0, binding = 1) uniform special_draw_data_array_buffer {
  special_draw_data data[];
}
special_draw_data_array;


void main() {
  //
  vec4 word_position = default_matrix_array.matrices[gl_InstanceIndex] *
                       vec4(position_in, 0.0f, 1.0f);
  word_position.x *= (float(default_buffer.window_height) / float(default_buffer.window_width));
  
  texture_cordinate_out = texture_cordinate_in;
  fragment_world_position_out = word_position.xyz;
  gl_Position = word_position; // (push_data.projection * push_data.view) * 
}
