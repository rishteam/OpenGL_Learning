#include "Shader.h"
#include "Utils.h"

#include <filesystem>
#include <iostream>

Shader::Shader()
{
    m_ID = glCreateProgram();
}

Shader::Shader(const std::string &vsPath, const std::string &fsPath)
{
    m_ID = glCreateProgram();
    Load(vsPath, fsPath);
}

Shader::~Shader()
{
    if(m_ID)
        Delete();
}

void Shader::Load(const std::string &vsPath, const std::string &fsPath)
{
    std::filesystem::path vs(vsPath), fs(fsPath);
    GLuint vertexShader = 0, fragmentShader = 0;

    if(std::filesystem::exists(vs))
    {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vertexShaderSource = ReadFileContent(vs);
        const char *vss = vertexShaderSource.c_str();
        glShaderSource(vertexShader, 1, &vss, NULL);
        glCompileShader(vertexShader);
        glAttachShader(m_ID, vertexShader);
    }

    if(std::filesystem::exists(fs))
    {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragmentShaderSource = ReadFileContent(fs);
        const char *fss = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fss, NULL);
        glCompileShader(fragmentShader);
        glAttachShader(m_ID, fragmentShader);
    }

    glLinkProgram(m_ID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate() const
{
    glUseProgram(m_ID);
}

void Shader::Deactivate() const
{
    glUseProgram(0);
}

void Shader::Delete()
{
    glDeleteShader(m_ID);
    m_ID = 0;
}
