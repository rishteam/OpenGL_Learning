#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <fmt/printf.h>

bool LoadFileContent(std::string &s, const char *path)
{
    std::ifstream ifs(path);
    if (ifs)
    {
        s.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return true;
    }
    printf("Failed to open file: %s\n", path);
    return false;
}

uint32_t Shader::CompileShader(GLenum type, const char **src)
{
    uint32_t shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, src, nullptr);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, log);
        printf("Error Shader %s compile error\n%s\n", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", log);
        return 0;
    }
    return shader;
}

uint32_t Shader::LinkShaderProgram(uint32_t vertex, uint32_t fragment)
{
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    //
    int success;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, log);
        printf("Error Shader Linking error\n%s\n", log);
        return 0;
    }
    return program;
}

void Shader::setShaderSource(const char *src)
{
    if(src != nullptr)
        m_Src = src;

    int vs = m_Src.find("//!vertex\n");
    int fs = m_Src.find("//!fragment\n");

    if(vs < fs)
    {
        m_VertSource = m_Src.substr(vs, fs-vs);
        m_FragSource = m_Src.substr(fs, m_Src.size()-fs);
    }
    else
    {
        m_FragSource = m_Src.substr(fs, vs-fs);
        m_VertSource = m_Src.substr(vs, m_Src.size()-vs);
    }

    fmt::print("{}\n{}\n\n\n", m_VertSource, m_FragSource);

    compileAndLinkShader();
}

void Shader::setShaderSource(const char *vsSrc, const char *fsSrc)
{
    m_VertSource = vsSrc;
    m_FragSource = fsSrc;
    assert(m_VertSource != "");
    assert(m_FragSource != "");
    compileAndLinkShader();
}

int Shader::getAttribLocation(const std::string &name)
{
    assert(m_ShaderID > 0);
    int loc = glGetAttribLocation(m_ShaderID, name.c_str());
    assert(loc >= 0);
    return loc;
}

int Shader::getUniformLocation(const std::string &name)
{
    assert(m_ShaderID > 0);
    int location = glGetUniformLocation(m_ShaderID, name.c_str());
    assert(location >= 0);
    return location;
}

void Shader::setInt(const std::string &name, int value)
{
    bind();
    glUniform1i(getUniformLocation(name), value);
}
void Shader::setIntArray(const std::string &name, int *values, uint32_t count)
{
    bind();
    glUniform1iv(getUniformLocation(name), count, values);
}
//
void Shader::setFloat(const std::string &name, float value)
{
    bind();
    glUniform1f(getUniformLocation(name), value);
}
void Shader::setFloatArray(const std::string &name, float *values, uint32_t count)
{
    bind();
    glUniform1fv(getUniformLocation(name), count, values);
}
void Shader::setFloat2(const std::string &name, const glm::vec2 &value)
{
    bind();
    glUniform2f(getUniformLocation(name), value.x, value.y);
}
void Shader::setFloat3(const std::string &name, const glm::vec3 &value)
{
    bind();
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}
void Shader::setFloat4(const std::string &name, const glm::vec4 &value)
{
    bind();
    glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}
//
void Shader::setMat2(const std::string &name, const glm::mat2 &matrix)
{
    bind();
    glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::setMat3(const std::string &name, const glm::mat3 &matrix)
{
    bind();
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::setMat4(const std::string &name, const glm::mat4 &matrix)
{
    bind();
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t Shader::compileAndLinkShader()
{
    const char *vertSrc = m_VertSource.c_str();
//    printf("[DEBUG] vertex m_ShaderID\n%s\n\n", vertSrc);
    uint32_t vert = CompileShader(GL_VERTEX_SHADER, &vertSrc);

    const char *fragSrc = m_FragSource.c_str();
//    printf("[DEBUG] fragment m_ShaderID\n%s\n\n", fragSrc);
    uint32_t frag = CompileShader(GL_FRAGMENT_SHADER, &fragSrc);

    assert(vert);
    assert(frag);

    m_ShaderID = LinkShaderProgram(vert, frag);
    assert(m_ShaderID);

    glDeleteShader(vert);
    glDeleteShader(frag);

    return m_ShaderID;
}
