#ifndef MESH
#define MESH

#include "shader.h"

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/scene.h>

struct Vertex {
    /* data */
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec4 boneIDs;
    glm::vec4 boneWeights;

    void addBoneData(GLuint boneID, float boneWeight);
} /* optional variable list */;

struct Texture {
    /* data */
    GLuint id;
    std::string type;
    aiString path;
} /* optional variable list */;

class Mesh {
    public:
        const GLuint MAX_BONES = 100;
        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;
        std::vector<Bone> bones;
        Bone* rootBone;

        Mesh(std::vector<Vertex> vertices,
             std::vector<GLuint> indices,
             std::vector<Bone> bones,
             std::vector<Texture> textures);

        // void updateAnimation(float timeInSeconds);
        void Draw(Shader shader);

    private:
        /* data */
        GLuint VAO, VBO, EBO;
        void setupMesh();
};

#endif /* ifndef MESH */
