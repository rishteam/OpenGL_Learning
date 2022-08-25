#pragma once
#ifndef LEARNOPENGL_TEXTURE_H
#define LEARNOPENGL_TEXTURE_H

#include <string>

class Texture2D
{
public:
    Texture2D() = default;
    Texture2D(const std::string path);
    ~Texture2D();

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    void Bind(uint32_t slot = 0);
    void Unbind() const;

    uint32_t GetTexID() { return m_TextureID; }

private:
    std::string m_Name;
    std::string m_Path;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_TextureID;
};

class DiffuseTexture : public Texture2D
{
public:
//    DiffuseTexture(const std::string &path, )
};

#endif //LEARNOPENGL_TEXTURE_H
