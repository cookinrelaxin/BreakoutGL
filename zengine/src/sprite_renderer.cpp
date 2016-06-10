#include "sprite_renderer.h"

#include <iostream>

SpriteRenderer::SpriteRenderer(Shader &shader) {
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &this->quadVAO);
}

void SpriteRenderer::DrawSprite(Texture2D &texture,
                                glm::vec2 position,
                                glm::vec2 size,
                                GLfloat rotate,
                                glm::vec4 color) {
    glm::mat4 model = [=]() {
        glm::mat4 m;

        //translate to given position
        m = glm::translate(m, glm::vec3(position, 0.0f));

        //center to given position
        m = glm::translate(m, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));

        //rotate to given rotation
        m = glm::rotate(m, rotate, glm::vec3(0.0f, 0.0f, 1.0f));

        //move origin back
        m = glm::translate(m, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        //scale to given scale
        m = glm::scale(m, glm::vec3(size, 1.0f));
        return m;
    }();

    this->shader.Use();
    this->shader.SetMatrix4("model", model);
    this->shader.SetVector4f("spriteColor", color);

    glActiveTexture(GL_TEXTURE0);
    texture.Bind();

    glBindVertexArray(this->quadVAO);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    //reset vertex array
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData() {
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &this->quadVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->quadVAO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //reset buffer and vertex array
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
