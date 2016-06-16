#ifndef MODEL_H
#define MODEL_H

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
    GLuint id;
    std::string type;
    aiString path;
};

class Model {
    public:
        Model();

        bool LoadMesh(const std::string& Filename);

        void Render(Shader& shader);

        uint NumBones() const {
            return m_NumBones;
        }

        void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms);

    private:
        #define NUM_BONES_PER_VERTEX 4

        struct BoneInfo {
            glm::mat4 BoneOffset;
            glm::mat4 FinalTransformation;
        };

        struct VertexBoneData {
            uint IDs[NUM_BONES_PER_VERTEX];
            float Weights[NUM_BONES_PER_VERTEX];

            void AddBoneData(uint BoneID, float Weight);
        };


    void CalcInterpolatedScaling(aiVector3D& Out,
                                 float AnimationTime,
                                 const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedRotation(aiQuaternion& Out,
                                  float AnimationTime,
                                  const aiNodeAnim* pNodeAnim);

    void CalcInterpolatedPosition(aiVector3D& Out,
                                  float AnimationTime,
                                  const aiNodeAnim* pNodeAnim);

    uint FindScaling(float AnimationTime,
                     const aiNodeAnim* pNodeAnim);
    uint FindRotation(float AnimationTime,
                      const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime,
                      const aiNodeAnim* pNodeAnim);

    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation,
                                   const std::string NodeName);

    void ReadNodeHeirarchy(float AnimationTime,
                           const aiNode* pNode,
                           const glm::mat4& ParentTransform);

    bool InitFromScene(const aiScene* pScene,
                       const std::string& Filename);

    void InitMesh(uint MeshIndex,
                  const aiMesh* paiMesh,
                  std::vector<glm::vec3>& Positions,
                  std::vector<glm::vec3>& Normals,
                  std::vector<glm::vec2>& TexCoords,
                  std::vector<VertexBoneData>& Bones,
                  std::vector<unsigned int>& Indices);

    void LoadBones(uint MeshIndex,
                   const aiMesh* paiMesh,
                   std::vector<VertexBoneData>& Bones);

    bool InitMaterials(const aiScene* pScene,
                       const std::string& Filename);

    std::map<std::string, Texture> loadMaterialTextures(const aiMaterial* mat,
                                              aiTextureType type,
                                              std::string typeName);

#define INVALID_MATERIAL 0xFFFFFFFF

enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs
};

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];

    struct MeshEntry {
        MeshEntry() {
            NumIndices    = 0;
            BaseVertex    = 0;
            BaseIndex     = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;

        std::vector<std::string> textures;
    };

    std::string directory;

    std::vector<MeshEntry> m_Entries;
    // std::vector<Texture> m_Textures;
    std::map<std::string, Texture> m_Textures;
    // std::vector<Texture> loaded_textures;

    std::map<std::string,uint> m_BoneMapping; // maps a bone name to its index
    uint m_NumBones;
    std::vector<BoneInfo> m_BoneInfo;
    glm::mat4 m_GlobalInverseTransform;

    const aiScene* m_pScene;
    Assimp::Importer m_Importer;
};

#endif
