#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <tuple>

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

typedef std::tuple<std::string, GLuint> Font;

class TextRenderer {
    public:
        // std::map<GLchar, Character> Characters;
        std::map<Font, std::map<GLchar, Character>> fonts;
        Shader TextShader;

        TextRenderer(GLuint width, GLuint height);

        std::map<GLchar, Character> Load(std::string font, GLuint fontSize);

        void RenderText(std::string text,
                        GLfloat x,
                        GLfloat y,
                        Font f,
                        bool centered = false,
                        glm::vec4 color = glm::vec4(1.0f));
    private:
        GLuint VAO, VBO;
};

#endif
