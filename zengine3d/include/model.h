#ifndef MODEL_H
#define MODEL_H

#define NUM_BONES_PER_VERTEX 4

#include <map>
#include <vector>

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <shader.h>

struct Texture {
    GLuint id = 0;
    std::string type = "undefined type";
    aiString path = aiString("undefined path");
    void print();
};

class Model {
    public:
        Model(const std::string& fileName)
            : VAO(0)
            , numBones(0)
            , m_pScene(nullptr) {
                loadMesh(fileName);
                glGenTextures(1, &defaultTexture);
                glBindTexture(GL_TEXTURE_2D, defaultTexture);
                unsigned char data[3]{255, 255, 255};

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                assert(glGetError() == GL_NO_ERROR);

                glBindTexture(GL_TEXTURE_2D, 0);
            }

        void render(Shader& shader);
        void renderShadows(Shader& shader);

        void boneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms);

    private:

        struct BoneInfo {
            glm::mat4 boneOffset;
            glm::mat4 finalTransformation;
        };

        struct VertexBoneData {
            uint IDs[NUM_BONES_PER_VERTEX];
            float weights[NUM_BONES_PER_VERTEX];

            void addBoneData(uint boneID, float weight);
        };

    bool loadMesh(const std::string& fileName);

    void calcInterpolatedScaling(aiVector3D& out,
                                 float animationTime,
                                 const aiNodeAnim* pNodeAnim);

    void calcInterpolatedRotation(aiQuaternion& out,
                                  float animationTime,
                                  const aiNodeAnim* pNodeAnim);

    void calcInterpolatedPosition(aiVector3D& out,
                                  float animationTime,
                                  const aiNodeAnim* pNodeAnim);

    uint findScaling(float animationTime,
                     const aiNodeAnim* pNodeAnim);
    uint findRotation(float animationTime,
                      const aiNodeAnim* pNodeAnim);
    uint findPosition(float animationTime,
                      const aiNodeAnim* pNodeAnim);

    const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation,
                                   const std::string nodeName);

    void readNodeHierarchy(float animationTime,
                           const aiNode* pNode,
                           const glm::mat4& parentTransform);

    bool initFromScene(const aiScene* pScene,
                       const std::string& fileName);

    void initMesh(uint meshIndex,
                  const aiMesh* paiMesh,
                  std::vector<glm::vec3>& positions,
                  std::vector<glm::vec3>& normals,
                  std::vector<glm::vec3>& tangents,
                  std::vector<glm::vec3>& bitangents,
                  std::vector<glm::vec2>& texCoords,
                  std::vector<VertexBoneData>& bones,
                  std::vector<unsigned int>& indices);

    void loadBones(uint meshIndex,
                   const aiMesh* paiMesh,
                   std::vector<VertexBoneData>& bones);

    bool initMaterials(const aiScene* pScene,
                       const std::string& fileName);

    std::map<std::string, Texture> loadMaterialTextures(const aiMaterial* mat,
                                                        aiTextureType type,
                                                        std::string typeName);

#define INVALID_MATERIAL 0xFFFFFFFF

enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TANGENT_VB,
    BITANGENT_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs
};

    GLuint VAO;
    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry {
        MeshEntry() {
            numIndices    = 0;
            baseVertex    = 0;
            baseIndex     = 0;
            materialIndex = INVALID_MATERIAL;
            diffuseColor = glm::vec3(0.0f, 0.0f, 0.0f);
            specularColor = glm::vec3(0.0f, 0.0f, 0.0f);
            shininess = 0.0f;
        }

        unsigned int numIndices;
        unsigned int baseVertex;
        unsigned int baseIndex;
        unsigned int materialIndex;

        glm::vec3 diffuseColor;
        glm::vec3 specularColor;

        float shininess;

        std::vector<std::string> textures;
    };

    std::string directory;
    bool flipAxes;

    std::vector<MeshEntry> meshes;
    // std::vector<Texture> m_Textures;
    std::map<std::string, Texture> m_Textures;
    // std::vector<Texture> loaded_textures;

    std::map<std::string,uint> m_BoneMapping; // maps a bone name to its index
    uint numBones;
    std::vector<BoneInfo> m_BoneInfo;
    glm::mat4 m_GlobalInverseTransform;

    const aiScene* m_pScene;
    Assimp::Importer m_Importer;

    GLuint defaultTexture;
};

#endif
