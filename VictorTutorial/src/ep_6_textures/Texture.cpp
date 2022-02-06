#include <filesystem>

#include "Texture.h"
#include "Utils.h"

#include <iostream>

Texture::Texture(std::filesystem::path path, GLenum textureType, GLenum slot, GLenum format, GLenum pixelType)
{
    auto image = LoadImage(path);

    m_Type = textureType;

    glGenTextures(1, &m_ID);
    glActiveTexture(slot);
    glBindTexture(m_Type, m_ID);

    glTexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(m_Type, 0, GL_RGBA, image->width, image->height, 0, format, pixelType, image->bytes);

    glGenerateMipmap(m_Type);
    glBindTexture(m_Type, 0);
}

Texture::~Texture()
{
    if(m_ID)
        Delete();
}

void Texture::TexUnit(const Ref<Shader> &shader, const char *uniform, GLuint unit)
{
    shader->Activate();
    glUniform1i(glGetUniformLocation(shader->GetID(), uniform), unit);
}

void Texture::Delete()
{
    glDeleteTextures(1, &m_ID);
}

void Texture::Bind()
{
    glBindTexture(m_Type, m_ID);
}

void Texture::Unbind()
{
    glBindTexture(m_Type, 0);
}
