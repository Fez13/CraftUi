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
  uint frame_rate;
  uint counter;
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
  float albedo_color_strength;
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

float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

void main() { 
  int index = material_material_object_index.materialIndex[instance_index_in];
  
  vert_color_out = material_material_array.data[index].albedo_color * material_material_array.data[index].albedo_color_strength;
  
  if(material_material_array.data[index].albedo_texture_index != -1){
    vert_color_out += texture(sampler2D(material_texture_array[material_material_array.data[index].albedo_texture_index],material_image_sampler),texture_cordinate_in) * (1.0f - material_material_array.data[index].albedo_color_strength);
  }
  
  
  vec2 char_texture_position = texture_cordinate_in + (vec2(1675,1) / 2048.0);

    if(char_texture_position.x > (1701 / 2048.0) || char_texture_position.y > (32  / 2048.0)){
        vert_color_out = vec4(0,0,0,0);
        return;
    }
  vert_color_out = texture(sampler2D(material_texture_array[material_material_array.data[index].albedo_texture_index],material_image_sampler),char_texture_position);
  
}
