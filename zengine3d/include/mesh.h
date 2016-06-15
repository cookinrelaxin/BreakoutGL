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
    std::vector<GLuint> boneIDs;
    std::vector<float> boneWeights;
} /* optional variable list */;

struct Texture {
    /* data */
    GLuint id;
    std::string type;
    aiString path;
} /* optional variable list */;

class Mesh {
    public:
        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices,
             std::vector<GLuint> indices,
             std::vector<Texture> textures);

        // void updateAnimation(float timeInSeconds);
        void Draw(Shader shader);

    private:
        /* data */
        GLuint VAO, VBO, EBO;
        void setupMesh();
};

#endif /* ifndef MESH */
