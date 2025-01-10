#version 330 core

out vec4 f_color;

in vec3 f_pos;
in vec3 f_norm;
in vec2 f_tex_coords;

uniform sampler2D texture_diffuse_1;
uniform sampler2D texture_specular_1;
uniform vec3 view_pos;

struct spot_light_t {
    vec3 position;
    vec3 direction;
    float inner_cut;
    float outer_cut;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float k_constant;
    float k_linear;
    float k_quadratic;
};
uniform spot_light_t light;

vec3 calc_spot_light(spot_light_t light, vec3 norm, vec3 view_dir) {
    vec3 light_dir = normalize(light.position - f_pos);
    float theta = dot(light_dir, normalize(-light.direction));
    if (theta < light.outer_cut) {
        return vec3(0);
    }
    float intensity = clamp((theta - light.outer_cut) / (light.inner_cut - light.outer_cut), 0, 1);

    vec3 ambient = light.ambient * vec3(texture(texture_diffuse_1, f_tex_coords));
    vec3 diffuse = light.diffuse * vec3(texture(texture_diffuse_1, f_tex_coords)) * max(dot(norm, light_dir), 0);
    vec3 specular = light.specular * vec3(texture(texture_specular_1, f_tex_coords)) * pow(max(dot(view_dir, reflect(-light_dir, norm)), 0), 32);

    float d = length(light.position - f_pos);
    float attenuation = 1.0 / (light.k_constant + light.k_linear * d + light.k_quadratic * pow(d, 2));
    return (ambient + (diffuse + specular) * intensity) * attenuation * 0.00001 + ambient;
}

void main() {
    f_color = vec4(calc_spot_light(light, normalize(f_norm), normalize(view_pos - f_pos)), 1);
}