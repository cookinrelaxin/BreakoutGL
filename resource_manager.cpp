#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

//#include <SOIL.h>
#include <stb_image.h>


#include <iostream>
#include <string>

//
// PUBLIC ~~~
//

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;

Shader ResourceManager::LoadShader(const GLchar *vShaderFile,
                                   const GLchar *fShaderFile,
                                   const GLchar *gShaderFile,
                                   std::string name) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file,
                                       GLboolean alpha,
                                       std::string name) {
    Texture2D tex = loadTextureFromFile(file, alpha);
    Textures[name] = tex;
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name) {
    return Textures[name];
}

void ResourceManager::Clear() {
    for (auto s : Shaders)
        glDeleteProgram(s.second.ID);
    for (auto t : Textures)
        glDeleteTextures(1, &t.second.ID);
    return;
}

//
// PRIVATE ~~~
//

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile,
                                           const GLchar *fShaderFile,
                                           const GLchar *gShaderFile) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    try {
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;

            gShaderStream << geometryShaderFile.rdbuf();

            geometryShaderFile.close();

            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();
    const GLchar* gShaderCode = geometryCode.c_str();

    Shader shader;
    shader.Compile(vShaderCode,
                   fShaderCode,
                   gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file,
                              GLboolean alpha) {
    Texture2D texture;
    if (alpha) {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    int width, height, channels;

    unsigned char* image = stbi_load(file,
                                     &width,
                                     &height,
                                     &channels,
                                     0);

    // if (width != height) {
    //     std::string err("failed loading image: ");
    //     err += file;
    //     err += ": invalid dimensions";
    //     throw std::runtime_error(err);
    // }
    if (image == NULL) {
        std::cout << "Failed to load image: " << file << std::endl;
        std::cout << "last result: " << stbi_failure_reason() << std::endl;
    }
    texture.Generate(width, height, image);

    stbi_image_free(image);
    return texture;
}
