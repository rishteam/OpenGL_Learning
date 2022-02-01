#pragma once
#ifndef VICTORTUTORIAL_SHADER_H
#define VICTORTUTORIAL_SHADER_H

#include <glad/glad.h>
#include <string>

class Shader
{
public:
    Shader();
    Shader(const std::string &vsPath, const std::string &fsPath);
    ~Shader();

    void Load(const std::string &vsPath, const std::string &fsPath);

    void Activate() const;
    void Deactivate() const;

    void Delete();
private:
    GLuint m_ID;
};


#endif //VICTORTUTORIAL_SHADER_H
