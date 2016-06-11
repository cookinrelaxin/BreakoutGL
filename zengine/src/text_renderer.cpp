#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.h"
#include "resource_manager.h"

TextRenderer::TextRenderer(GLuint width, GLuint height) {
    assert(glGetError() == GL_NO_ERROR);
    this->TextShader = ResourceManager::LoadShader("./assets/shaders/text.vs",
                                                   "./assets/shaders/text.fs",
                                                   nullptr,
                                                   "text"
    );

    glm::mat4 mat = glm::ortho(0.0f,
                               static_cast<GLfloat>(width),
                               static_cast<GLfloat>(height),
                               0.0f
                    );
    this->TextShader.SetMatrix4("projection", mat, GL_TRUE);
    this->TextShader.SetInteger("text", 0);

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

std::map<GLchar, Character> TextRenderer::Load(std::string font, GLuint fontSize) {

    std::cout << "load font: " << font << std::endl;
    std::map<GLchar, Character> fontMap;

    // this->fontSize = fontSize;
    assert(glGetError() == GL_NO_ERROR);
    // this->Characters.clear();
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType library" << std::endl;
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte c(0); c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        fontMap.insert(std::pair<GLchar, Character>(c, character));
        // Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    std::tuple<std::string, GLuint> f(font, fontSize);
    fonts.insert(std::make_pair(f, fontMap));
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    assert(glGetError() == GL_NO_ERROR);

    return fontMap;
}

void TextRenderer::RenderText(std::string text,
                              GLfloat x,
                              GLfloat y,
                              Font f,
                              bool centered,
                              glm::vec4 color) {
    std::map<GLchar, Character> Characters;
    if (fonts.find(f) != fonts.end())
        Characters = fonts.find(f)->second;
    else
        Characters = Load(std::get<0>(f), std::get<1>(f));

    auto centerOffset = [&]() {
        if (centered) {
            auto acc(0);
            for (GLchar c : text)
                acc += Characters[c].Size.x
                    + Characters[c].Bearing.x
                    + Characters[c].Advance >> 6; 
            return acc;
        }
        else
            return 0;
    }();

    assert(glGetError() == GL_NO_ERROR);
    this->TextShader.Use();
    this->TextShader.SetVector4f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    assert(glGetError() == GL_NO_ERROR);

    // std::cout << "font size: " << fontSize << std::endl;
    // std::cout << "font name: " << std::get<0>(f) << std::endl;
    // std::cout << "totalWidth: " << totalWidth << std::endl;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x - (centerOffset / 2);
        GLfloat ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y);

        GLfloat w = ch.Size.x;
        GLfloat h = ch.Size.y;
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

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        assert(glGetError() == GL_NO_ERROR);

        x += (ch.Advance >> 6);
    }
    assert(glGetError() == GL_NO_ERROR);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    assert(glGetError() == GL_NO_ERROR);
}
