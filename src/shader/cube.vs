#version 330 core
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex_coord;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 light_pos;

void main() {
    frag_pos = vec3(model * vec4(v_pos, 1.0));
    frag_normal = v_normal;
    frag_tex_coord = v_tex_coord;
    gl_Position = projection * view * model * vec4(v_pos, 1.0);
}