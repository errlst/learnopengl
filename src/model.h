#pragma once

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "mesh.h"

class model_t {
  public:
    auto load(std::string_view path) -> void {
        auto importer = Assimp::Importer{};
        auto scene = importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << importer.GetErrorString() << "\n";
            exit(-1);
        }
        m_directory = path.substr(0, path.find_last_of("/"));
        process_node(scene->mRootNode, scene);
    }

    auto draw(const shader_t &shader) -> void {
        for (auto &mesh : m_meshes) {
            mesh.draw(shader);
        }
    }

  private:
    auto process_node(const aiNode *node, const aiScene *scene) -> void {
        for (auto i = 0; i < node->mNumMeshes; ++i) {
            process_mesh(scene->mMeshes[node->mMeshes[i]], scene);
        }
        for (auto i = 0; i < node->mNumChildren; ++i) {
            process_node(node->mChildren[i], scene);
        }
    }

    auto process_mesh(const aiMesh *mesh, const aiScene *scene) -> void {
        auto vertices = std::vector<vertex_t>{};
        auto indices = std::vector<GLuint>{};
        auto textures = std::vector<texture_t>{};

        for (auto i = 0; i < mesh->mNumVertices; ++i) {
            // 一个顶点可能有多个贴图通道，这里只加载一个通道
            vertices.emplace_back(vertex_t{glm::vec3{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z},
                                           glm::vec3{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z},
                                           mesh->mTextureCoords[0] ? glm::vec2{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y} : glm::vec2{0, 0}});
        }

        for (auto i = 0; i < mesh->mNumFaces; ++i) {
            for (auto j = 0; j < mesh->mFaces[i].mNumIndices; ++j) {
                indices.emplace_back(mesh->mFaces[i].mIndices[j]);
            }
        }

        load_texture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE, textures);
        load_texture(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_SPECULAR, textures);

        m_meshes.emplace_back(vertices, indices, textures);
    }

    auto load_texture(const aiMaterial *material, aiTextureType type, std::vector<texture_t> &textures) -> void {
        for (auto i = 0; i < material->GetTextureCount(type); ++i) {
            auto s = aiString{};
            material->GetTexture(type, i, &s);

            textures.push_back(texture_t{
                .id = shader_t::load_texture(m_directory + "/" + s.C_Str()),
                .type = type == aiTextureType_DIFFUSE ? texture_type_e::diffuse : texture_type_e::specular});
        }
    }

  private:
    std::vector<mesh_t> m_meshes;
    std::string m_directory;
};