#include "graphics/shader_loader.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

namespace CW
{

    uint32_t compile_shader(const char* shaderPath, uint32_t type)
    {
        std::string shaderCode;
        std::ifstream shaderFile;

        shaderFile.exceptions(std::ifstream::badbit);
        try {
            shaderFile.open(shaderPath);
            std::stringstream shaderStream;

            shaderStream << shaderFile.rdbuf();

            shaderFile.close();

            shaderCode = shaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            std::cerr << e.what() << std::endl;
            return 0;
        }

        const char* shaderCodePtr = shaderCode.c_str();

        uint32_t shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderCodePtr, nullptr);
        glCompileShader(shader);

        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cerr << "SHADER::"
                << (type == GL_VERTEX_SHADER ? "VERTEX" : (type == GL_FRAGMENT_SHADER ? "FRAGMENT" : "COMPUTE"))
                << ": Compilation failed" << std::endl;
            std::cerr << infoLog << std::endl;
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    uint32_t link_program(const uint32_t* shaderIDs, size_t num)
    {
        uint32_t id = glCreateProgram();

        for (int i = 0; i < num; i++)
        {
            glAttachShader(id, shaderIDs[i]);
        }
        glLinkProgram(id);

        int success;
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cerr << "SHADER::PROGRAM: Linking failed" << std::endl;
            std::cerr << infoLog << std::endl;

            for (int i = 0; i < num; i++)
            {
                glDeleteShader(shaderIDs[i]);
            }
            
            glDeleteProgram(id);
            return 0;
        }

        for (int i = 0; i < num; i++)
        {
            glDeleteShader(shaderIDs[i]);
        }

        return id;
    }

} // CW