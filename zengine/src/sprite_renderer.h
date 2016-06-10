#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "shader.h"

class SpriteRenderer {
    public:
        SpriteRenderer(Shader &shader);
        ~SpriteRenderer();

        void DrawSprite(Texture2D &texture,
                        glm::vec2 position,
                        glm::vec2 size = glm::vec2(10, 10),
                        GLfloat rotate = 0.0f,
                        glm::vec4 color = glm::vec4(1.0f));

    private:
        Shader shader;
        GLuint quadVAO;

        void initRenderData();
};

#endif
