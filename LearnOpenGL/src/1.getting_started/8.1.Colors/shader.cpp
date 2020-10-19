#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

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

int Shader::getLocationByName(const std::string &name)
{
    assert(program > 0);
    int location = glGetUniformLocation(program, name.c_str());
    return location;
}

void Shader::setInt(const std::string &name, int value)
{
    glUniform1i(getLocationByName(name), value);
}
void Shader::setIntArray(const std::string &name, int *values, uint32_t count)
{
    glUniform1iv(getLocationByName(name), count, values);
}
//
void Shader::setFloat(const std::string &name, float value)
{
    glUniform1f(getLocationByName(name), value);
}
void Shader::setFloat2(const std::string &name, const glm::vec2 &value)
{
    glUniform2f(getLocationByName(name), value.x, value.y);
}
void Shader::setFloat3(const std::string &name, const glm::vec3 &value)
{
    glUniform3f(getLocationByName(name), value.x, value.y, value.z);
}
void Shader::setFloat4(const std::string &name, const glm::vec4 &value)
{
    glUniform4f(getLocationByName(name), value.x, value.y, value.z, value.w);
}
//
void Shader::setMat2(const std::string &name, const glm::mat2 &matrix)
{
    glUniformMatrix2fv(getLocationByName(name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::setMat3(const std::string &name, const glm::mat3 &matrix)
{
    glUniformMatrix3fv(getLocationByName(name), 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::setMat4(const std::string &name, const glm::mat4 &matrix)
{
    glUniformMatrix4fv(getLocationByName(name), 1, GL_FALSE, glm::value_ptr(matrix));
}