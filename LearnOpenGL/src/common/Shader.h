#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <Texture.h>

bool LoadFileContent(std::string &s, const char *path);

class Shader
{
public:
    Shader() = default;
    Shader(const char *shaderPath)
    {
        assert(LoadFileContent(m_Src, shaderPath) == true);

        setShaderSource(nullptr);
    }
    Shader(const char *vertPath, const char *fragPath)
    {
        assert(LoadFileContent(m_VertSource, vertPath) == true);
        assert(LoadFileContent(m_FragSource, fragPath) == true);

        const char *vertSrc = m_VertSource.c_str();
//        printf("[DEBUG] vertex m_ShaderID\n%s\n\n", vertSrc);
        uint32_t vert = CompileShader(GL_VERTEX_SHADER, &vertSrc);
        const char *fragSrc = m_FragSource.c_str();
//        printf("[DEBUG] fragment m_ShaderID\n%s\n\n", fragSrc);
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

    void setShaderSource(const char *src); // combine vs and fs
    void setShaderSource(const char *vsSrc, const char *fsSrc);

    void bind()
    {
        glUseProgram(m_ShaderID);

        int id = 0;
        for(auto && [name, tex] : m_TextureMap)
        {
            setInt(name, id);
            tex.Bind(id);
            id++;
        }
    }
    void unbind()
    {
        glUseProgram(0);
    }
    uint32_t getShaderID() const { return m_ShaderID; }

    int getAttribLocation(const std::string &name);
    int getUniformLocation(const std::string &name);

    void setTexture(const std::string &name, const Texture2D &tex);

    void setInt(const std::string &name, int value);
    void setIntArray(const std::string &name, int *values, uint32_t count);
    //
    void setFloat(const std::string &name, float value);
    void setFloatArray(const std::string &name, float *values, uint32_t count);
    void setFloat2(const std::string &name, const glm::vec2 &value);
    void setFloat2(const std::string &name, float x, float y) { setFloat2(name, glm::vec2(x, y)); }
    void setFloat3(const std::string &name, const glm::vec3 &value);
    void setFloat3(const std::string &name, float x, float y, float z) { setFloat3(name, glm::vec3(x, y, z)); }
    void setFloat4(const std::string &name, const glm::vec4 &value);
    void setFloat4(const std::string &name, float x, float y, float z, float w) { setFloat4(name, glm::vec4(x, y, z, w)); }
    //
    void setMat2(const std::string &name, const glm::mat2 &matrix);
    void setMat3(const std::string &name, const glm::mat3 &matrix);
    void setMat4(const std::string &name, const glm::mat4 &matrix);

    static uint32_t CompileShader(GLenum type, const char **src);
    static uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment);

private:
    uint32_t compileAndLinkShader();

    std::unordered_map<std::string, Texture2D> m_TextureMap;
    std::string m_VertSource, m_FragSource;
    std::string m_Src;
    uint32_t m_ShaderID = 0;
};