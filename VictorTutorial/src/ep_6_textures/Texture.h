#pragma once
#ifndef VICTORTUTORIAL_TEXTURE_H
#define VICTORTUTORIAL_TEXTURE_H

#include <glad/glad.h>

#include "Shader.h"
#include "Ref.h"

class Texture
{
public:
    Texture(std::filesystem::path path, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType);
    ~Texture();

    void Bind();
    void Unbind();

    void TexUnit(const Ref<Shader> &shader, const char* uniform, GLuint unit);

    void Delete();
private:
    GLuint m_ID{};
    GLenum m_Type;
};

#endif //VICTORTUTORIAL_TEXTURE_H
