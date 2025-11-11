#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.hpp>
#include <utils.hpp>

namespace fs = std::filesystem;


class Model{
public:
    Model(const std::string &path){
        loadModel(path);
    }

    Model(const char *path){
        loadModel(std::string(path));
    };

    void Draw(Shader &shader);
    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded; // 记录加载过的纹理，避免每个mesh都加载重复的纹理
    

private:
    std::string directory; // 模型文件所在目录

    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

inline void Model::Draw(Shader &shader){
    for(auto &mesh: meshes){
        mesh.Draw(shader);
    }
}

inline void Model::loadModel(const std::string &path){
    Assimp::Importer importer;
    // const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate); // assimp加载模型时不设置翻转UV，在stb_image实际读取数据时翻转

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = fs::path(path).parent_path().string();

    processNode(scene->mRootNode, scene);
}

inline void Model::processNode(aiNode *node, const aiScene *scene){
    // 处理节点所有网格
    for(int i = 0; i<node->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.emplace_back(processMesh(mesh, scene));
    }
    // 递归处理子节点
    for(int i = 0; i<node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

// 从aiMesh的Mesh数据结构中提取数据，生成自定义Mesh对象
inline Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // 处理顶点
    for(int i = 0; i<mesh->mNumVertices; i++){
        Vertex vertex;
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

        if(mesh->mTextureCoords[0]){ // 有纹理坐标
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }else{
            vertex.TexCoords = {0.0f, 0.0f};
        }
        vertices.push_back(vertex);
    }
    // 处理索引
    for(int i = 0; i<mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(int j = 0; j<face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    // 处理材质 
    if(mesh->mMaterialIndex >= 0){
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

inline std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName){
    std::vector<Texture> textures;
    for(int i = 0; i< mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);

        auto it = std::find_if(textures_loaded.begin(), textures_loaded.end(),
            [&str](const Texture& tex){
                return tex.path == str.C_Str();
            });
        if(it == textures_loaded.end()){
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture); // 存储已加载的纹理
        }else{
            textures.push_back(*it);
        }
    }
    return textures;
}




