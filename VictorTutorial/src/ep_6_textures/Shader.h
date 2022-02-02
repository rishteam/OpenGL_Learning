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

    // TODO: DELETE THIS
    GLuint GetID() const { return m_ID; }
private:
    GLuint m_ID;

    void CheckError(GLuint shader, const char *type);
};


#endif //VICTORTUTORIAL_SHADER_H
