#pragma once

#include "shader.h"
#include <glm/glm.hpp>
#include <vector>

struct vertex_t {
    glm::vec3 pos;
    glm::vec3 norm;
    glm::vec2 tex_coords;
};
static_assert(sizeof(vertex_t) == 8 * sizeof(float));

enum class texture_type_e {
    diffuse,
    specular,
};

struct texture_t {
    GLuint id;
    texture_type_e type;
};

class mesh_t {
  public:
    mesh_t(const std::vector<vertex_t> &vertices,
           const std::vector<GLuint> &indices,
           const std::vector<texture_t> &textures) : m_vertices{vertices}, m_indices{indices}, m_textures{textures} {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        std::cout << "mesh vertices count : " << m_vertices.size() << " , indices count : " << m_indices.size() << " , texture count " << m_textures.size() << "\n";

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vertex_t), m_vertices.data(), GL_STATIC_DRAW);

        GLuint ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex_t), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex_t), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex_t), (void *)(6 * sizeof(float)));

        glBindVertexArray(0);
    }

    auto draw(const shader_t &shader) -> void {
        auto diffuse_tex_n = 1, specular_tex_n = 1;
        for (auto i = 0; i < m_textures.size(); ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i].id);

            switch (m_textures[i].type) {
            case texture_type_e::diffuse:
                shader.set_uniform("texture_diffuse_" + std::to_string(diffuse_tex_n++), i);
                break;
            case texture_type_e::specular:
                shader.set_uniform("texture_specular_" + std::to_string(specular_tex_n++), i);
                break;
            }
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

  private:
    std::vector<vertex_t> m_vertices;
    std::vector<GLuint> m_indices;
    std::vector<texture_t> m_textures;
    GLuint m_vao;
};