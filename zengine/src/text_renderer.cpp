#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.h"
#include "resource_manager.h"

TextRenderer::TextRenderer(GLuint width, GLuint height) {
    assert(glGetError() == GL_NO_ERROR);
    this->TextShader = ResourceManager::LoadShader("./assets/shaders/msdf.vs",
                                                   "./assets/shaders/msdf.fs",
                                                   nullptr,
                                                   "text"
    );

    glm::mat4 mat = glm::ortho(0.0f,
                               static_cast<GLfloat>(width),
                               static_cast<GLfloat>(height),
                               0.0f,
                               -1.0f, 
                               1.0f);

    this->TextShader.SetMatrix4("projection", mat, GL_TRUE);

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    assert(glGetError() == GL_NO_ERROR);
}

void TextRenderer::Load(std::string font) {
    assert(glGetError() == GL_NO_ERROR);
    this->Characters.clear();
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, 10);
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // for (GLubyte c(0); c < 128; c++) {
    GLubyte c(65);
        FT_Load_Char(face, c, FT_LOAD_RENDER);
        // if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        //     std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
        //     continue;
        // }

        Character character = {
            0,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    // }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    assert(glGetError() == GL_NO_ERROR);

    ResourceManager::LoadTexture("./A_msdf.png", false, "msdf");
}

void TextRenderer::RenderText(std::string text,
                              GLfloat x,
                              GLfloat y,
                              GLfloat scale,
                              glm::vec4 color,
                              GLboolean centered) {
    // scale = 30.0;
    assert(glGetError() == GL_NO_ERROR);
    this->TextShader.Use();
    glm::vec4 bg = glm::vec4(0.0);
    this->TextShader.SetVector4f("bgColor", bg);
    glm::vec4 fg = glm::vec4(1.0);
    this->TextShader.SetVector4f("fgColor", fg);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    assert(glGetError() == GL_NO_ERROR);

    // std::string::const_iterator c;
    // for (c = text.begin(); c != text.end(); c++) {
    Character ch = Characters['A'];

    GLfloat xpos = x + ch.Bearing.x * scale;
    // GLfloat ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;
    GLfloat ypos = y + (ch.Size.y - ch.Bearing.y) * scale;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    assert(glGetError() == GL_NO_ERROR);

    GLfloat vertices[6][4] = {
        { xpos,     ypos + h, 0.0, 1.0 },
        { xpos + w, ypos,     1.0, 0.0 },
        { xpos,     ypos,     0.0, 0.0 },

        { xpos,     ypos + h, 0.0, 1.0 },
        { xpos + w, ypos + h, 1.0, 1.0 },
        { xpos + w, ypos,     1.0, 0.0 }
    };

    assert(glGetError() == GL_NO_ERROR);

    glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTexture("msdf").ID);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    assert(glGetError() == GL_NO_ERROR);

    x += (ch.Advance >> 6) * scale;
    // }
    assert(glGetError() == GL_NO_ERROR);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(glGetError() == GL_NO_ERROR);
}
