#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

bool LoadFileContent(std::string &s, const char *path);

class Shader
{
public:
    Shader(const char *vertPath, const char *fragPath)
    {
        assert(LoadFileContent(vertSource, vertPath) == true);
        assert(LoadFileContent(fragSource, fragPath) == true);

        const char *vertSrc = vertSource.c_str();
        uint32_t vert = CompileShader(GL_VERTEX_SHADER, &vertSrc);
        const char *fragSrc = fragSource.c_str();
        uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, &fragSrc);
        assert(vert && frag);

        program = LinkShaderProgram(vert, frag);
        assert(program);

        glDeleteShader(vert);
        glDeleteShader(frag);
    }
    ~Shader()
    {
        glDeleteProgram(program);
    }

    void bind()
    {
        glUseProgram(program);
    }
    void unbind()
    {
        glUseProgram(0);
    }
    uint32_t getShaderID() const { return program; }

    static uint32_t CompileShader(GLenum type, const char **src);
    static uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment);
private:
    std::string vertSource, fragSource;
    uint32_t program = 0;
};