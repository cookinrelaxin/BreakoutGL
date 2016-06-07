#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"
#include "shader.h"

class PostProcessor {
    public:
        Shader PostProcessingShader;
        Texture2D Texture;
        GLuint Width, Height;
        GLboolean Confuse, Chaos, Shake;

        PostProcessor(Shader shader, GLuint width, GLuint height);

        void BeginRender();
        void EndRender();
        void Render(GLfloat time);
    private:
        GLuint MSFBO, FBO;
        GLuint RBO;
        GLuint VAO;

        void initRenderData();
};

#endif
