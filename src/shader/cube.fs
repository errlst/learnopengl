#version 330 core
out vec4 frag_color;

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_tex_coord;

uniform vec3 view_pos;

struct material_t {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform material_t material;

struct parallel_light_t {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform parallel_light_t parallel_light;

vec3 calc_parallel_light(parallel_light_t light, vec3 norm, vec3 view_dir) {
    vec3 light_dir = normalize(-light.direction);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, frag_tex_coord));
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse, frag_tex_coord)) * max(dot(norm, light_dir), 0);
    vec3 specular = light.specular * vec3(texture(material.specular, frag_tex_coord)) * pow(max(dot(view_dir, reflect(-light_dir, norm)), 0), material.shininess);
    return (ambient + diffuse + specular);
}

struct point_light_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float k_constant;
    float k_linear;
    float k_quadratic;
};
uniform point_light_t point_light;

vec3 calc_point_light(point_light_t light, vec3 norm, vec3 view_dir) {
    vec3 light_dir = normalize(light.position - frag_pos);
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, frag_tex_coord));
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse, frag_tex_coord)) * max(dot(norm, light_dir), 0);
    vec3 specular = light.specular * vec3(texture(material.specular, frag_tex_coord)) * pow(max(dot(view_dir, reflect(-light_dir, norm)), 0), material.shininess);

    float d = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.k_constant + light.k_linear * d + light.k_quadratic * pow(d, 2));
    return (ambient + diffuse + specular) * attenuation;
}

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

uniform spot_light_t spot_light;

vec3 calc_spot_light(spot_light_t light, vec3 norm, vec3 view_dir) {
    vec3 light_dir = normalize(light.position - frag_pos);
    float theta = dot(light_dir, normalize(-light.direction));
    if (theta < light.outer_cut) {
        return vec3(0);
    }
    float intensity = clamp((theta - light.outer_cut) / (light.inner_cut - light.outer_cut), 0, 1);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, frag_tex_coord));
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuse, frag_tex_coord)) * max(dot(norm, light_dir), 0);
    vec3 specular = light.specular * vec3(texture(material.specular, frag_tex_coord)) * pow(max(dot(view_dir, reflect(-light_dir, norm)), 0), material.shininess);

    float d = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.k_constant + light.k_linear * d + light.k_quadratic * pow(d, 2));
    return (ambient + (diffuse + specular) * intensity) * attenuation;
}

void main() {
    vec3 norm = normalize(frag_normal);
    vec3 view_dir = normalize(view_pos - frag_pos);
    frag_color = vec4(calc_parallel_light(parallel_light, norm, view_dir) + calc_point_light(point_light, norm, view_dir) + calc_spot_light(spot_light, norm, view_dir), 1);
}