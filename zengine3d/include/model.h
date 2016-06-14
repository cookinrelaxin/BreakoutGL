#ifndef MODEL_H
#define MODEL_H

#include "Shader.h"
#include "mesh.h"

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
    public:
        Model(GLchar* path);

        void Draw(Shader shader);

    private:
        /* data */
        std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;

        void loadModel(std::string path);
        void processNode(aiNode* node,
                         const aiScene* scene);
        Mesh processMesh(aiMesh* mesh,
                         const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                                  aiTextureType type,
                                                  std::string typeName);
    protected:
        /* inherited data */
};

#endif /* end of include guard: MODEL_H */
