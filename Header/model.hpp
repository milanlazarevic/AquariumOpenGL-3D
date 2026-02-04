#ifndef MODEL_HPP
#define MODEL_HPP

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "shader.hpp"

// forward declaration
unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
    Model(const std::string& path, bool gamma = false);

    void Draw(Shader& shader);
    std::vector<Mesh> meshes;
    void calculateBoundingBox();
    float minX, maxX, minY, maxY, minZ, maxZ;

private:
    // model data
    std::vector<Texture> textures_loaded;
    std::string directory;
    bool gammaCorrection;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    void getWorldBounds(float& outMinX, float& outMaxX,
                       float& outMinY, float& outMaxY,
                       float& outMinZ, float& outMaxZ);

    std::vector<Texture> loadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        const std::string& typeName
    );
};

#endif // MODEL_HPP
