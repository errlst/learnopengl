#version 330 core

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_norm;
layout(location = 2) in vec2 v_tex_coords;

out vec3 f_pos;
out vec3 f_norm;
out vec2 f_tex_coords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(v_pos, 1);
    f_pos = v_pos;
    f_norm = v_norm;
    f_tex_coords = v_tex_coords;
}