#ifndef SHADER
#define SHADER

#include <cassert>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader {
    public:
        GLuint Program;

        Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr) {
            std::string vertexCode;
            std::string fragmentCode;
            std::string geometryCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            std::ifstream gShaderFile;

            vShaderFile.exceptions(std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::badbit);
            gShaderFile.exceptions(std::ifstream::badbit);

            try {
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;

                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();

                vShaderFile.close();
                fShaderFile.close();

                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            }
            catch (std::ifstream::failure e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            }

            if (geometryPath != nullptr) {

                try {
                    gShaderFile.open(geometryPath);
                    std::stringstream gShaderStream;

                    gShaderStream << gShaderFile.rdbuf();

                    gShaderFile.close();

                    geometryCode = gShaderStream.str();
                }
                catch (std::ifstream::failure e) {
                    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
                }

            }

            const GLchar* vShaderCode = vertexCode.c_str();
            const GLchar* fShaderCode = fragmentCode.c_str();
            const GLchar* gShaderCode = geometryCode.c_str();

            GLuint vertex, fragment, geometry;
            GLint success;
            GLchar infoLog[512];

            {
                vertex = glCreateShader(GL_VERTEX_SHADER); 
                glShaderSource(vertex, 1, &vShaderCode, nullptr);
                glCompileShader(vertex);
                glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(vertex, 512, nullptr, infoLog); 
                    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
                }
            }

            {
                fragment = glCreateShader(GL_FRAGMENT_SHADER); 
                glShaderSource(fragment, 1, &fShaderCode, nullptr);
                glCompileShader(fragment);
                glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(fragment, 512, nullptr, infoLog); 
                    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
                }
            }

            if (geometryPath != nullptr) {
                geometry = glCreateShader(GL_GEOMETRY_SHADER); 
                glShaderSource(geometry, 1, &gShaderCode, nullptr);
                glCompileShader(geometry);
                glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(geometry, 512, nullptr, infoLog); 
                    std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
                }
            }
            assert(glGetError() == GL_NO_ERROR);

            this->Program = glCreateProgram();
            glAttachShader(this->Program, vertex);
            glAttachShader(this->Program, fragment);
            if (geometryPath != nullptr) glAttachShader(this->Program, geometry);
            glLinkProgram(this->Program);

            glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
            assert(glGetError() == GL_NO_ERROR);

            if (!success) {
               glGetProgramInfoLog(this->Program, 512, nullptr, infoLog); 
               std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }
            assert(glGetError() == GL_NO_ERROR);

            glDeleteShader(vertex);
            glDeleteShader(fragment);
            if (geometryPath != nullptr) glDeleteShader(geometry);
            assert(glGetError() == GL_NO_ERROR);

        }

        void Use() {
           glUseProgram(this->Program); 
        }

};

#endif /* ifndef SHADER */
