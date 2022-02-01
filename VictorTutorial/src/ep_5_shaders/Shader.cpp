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
        CheckError(vertexShader, "VERTEX");
        glAttachShader(m_ID, vertexShader);
    }

    if(std::filesystem::exists(fs))
    {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fragmentShaderSource = ReadFileContent(fs);
        const char *fss = fragmentShaderSource.c_str();
        glShaderSource(fragmentShader, 1, &fss, NULL);
        glCompileShader(fragmentShader);
        CheckError(fragmentShader, "FRAGMENT");
        glAttachShader(m_ID, fragmentShader);
    }

    glLinkProgram(m_ID);
    CheckError(m_ID, "PROGRAM");
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

void Shader::CheckError(GLuint shader, const char *type)
{
    // Stores status of compilation
    GLint hasCompiled;
    // Character array to store error message in
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
        }
    }
}
