#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "shader.h"

struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class TextRenderer {
    public:
        std::map<GLchar, Character> Characters;
        Shader TextShader;

        TextRenderer(GLuint width, GLuint height);

        void Load(std::string font, GLuint fontSize);
        void RenderText(std::string text,
                        GLfloat x,
                        GLfloat y,
                        GLfloat scale,
                        glm::vec4 color = glm::vec4(1.0f),
                        GLboolean centered = GL_FALSE
        );
    private:
        GLuint VAO, VBO;
        GLuint fontSize;
};

#endif
