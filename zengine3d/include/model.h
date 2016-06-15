#ifndef MODEL_H
#define MODEL_H

#include "Shader.h"
#include "mesh.h"

#include <vector>
#include <string>
#include <chrono>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Node {
    std::string name;
    std::vector<Mesh> meshes;
    std::vector<Node*> children;
    std::vector<PositionKey> positionKeys;
    std::vector<RotationKey> rotationKeys;
    std::vector<ScalingKey> scalingKeys;
}

struct PositionKey {
    std::chrono::seconds time;
    glm::mat4 transformation;
}

struct RotationKey {
    std::chrono::seconds time;
    glm::mat4 transformation;
}

struct ScalingKey {
    std::chrono::seconds time;
    glm::mat4 transformation;
}

class Model {
    public:
        Model(GLchar* path);

        void Draw(Shader shader);

    private:
        /* data */
        // std::vector<Mesh> meshes;
        std::string directory;
        std::vector<Texture> textures_loaded;
        Node* rootNode;

        void loadModel(std::string path);
        void generateNode(aiNode* node,
                            const aiScene* scene);
        Mesh genertateMesh(aiMesh* mesh,
                           const aiScene* scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
                                                  aiTextureType type,
                                                  std::string typeName);
        void updateAnimation(float timeInSeconds);
        void findAnimationChannel(const aiScene* scene, const aiNode* node);

        void generateKeyFrames(aiNodeAnim* animation, Node* node);
        void generatePositionKeyFrames(aiNodeAnim* animation, Node* node);
        void generateRotationKeyFrames(aiNodeAnim* animation, Node* node);
        void generateScalingKeyFrames(aiNodeAnim* animation, Node* node);

        void generateMeshes(aiScene* scene, Node* node);
    protected:
        /* inherited data */
};

#endif /* end of include guard: MODEL_H */
