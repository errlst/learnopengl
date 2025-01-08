#version 330 core
out vec4 frag_color;

struct material_t {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct light_t {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float k_constant;
    float k_linear;
    float k_quadratic;
};

in vec3 frag_pos;
in vec3 frag_normal;
in vec2 frag_tex_coord;

uniform vec3 view_pos;
uniform material_t material;
uniform light_t light;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, frag_tex_coord));

    vec3 norm = normalize(frag_normal);
    vec3 light_dir = normalize(light.position - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, frag_tex_coord));

    vec3 viewDir = normalize(view_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(viewDir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, frag_tex_coord));

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.k_constant + light.k_linear * distance + light.k_quadratic * (distance * distance));

    frag_color = vec4((ambient + diffuse + specular) * attenuation, 1.0);
}