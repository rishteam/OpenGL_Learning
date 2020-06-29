#include "shader.h"

bool LoadFileContent(std::string &s, const char *path)
{
    std::ifstream ifs(path);
    if (ifs)
    {
        s.assign((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return true;
    }
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