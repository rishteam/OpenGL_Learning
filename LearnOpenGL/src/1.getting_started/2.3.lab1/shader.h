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
        assert(LoadFileContent(m_VertSource, vertPath) == true);
        assert(LoadFileContent(m_FragSource, fragPath) == true);

        const char *vertSrc = m_VertSource.c_str();
        uint32_t vert = CompileShader(GL_VERTEX_SHADER, &vertSrc);
        const char *fragSrc = m_FragSource.c_str();
        uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, &fragSrc);
        assert(vert && frag);

        m_ShaderID = LinkShaderProgram(vert, frag);
        assert(m_ShaderID);

        glDeleteShader(vert);
        glDeleteShader(frag);
    }
    ~Shader()
    {
        glDeleteProgram(m_ShaderID);
    }

    void bind()
    {
        glUseProgram(m_ShaderID);
    }
    void unbind()
    {
        glUseProgram(0);
    }
    uint32_t getShaderID() const { return m_ShaderID; }

    static uint32_t CompileShader(GLenum type, const char **src);
    static uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment);
private:
    std::string m_VertSource, m_FragSource;
    uint32_t m_ShaderID = 0;
};