#pragma once
#ifndef LEARNOPENGL_TEXTURE_H
#define LEARNOPENGL_TEXTURE_H

#include <glad/glad.h>
#include <string>

struct Image
{
public:
    Image();
    Image(const std::string &path);
    Image(const Image &other);
    ~Image();

    Image& operator=(const Image& other);

    uint32_t GetWidth();
    uint32_t GetHeight();
    const void* GetImagePtr();

private:
    struct Impl;
    Impl *m_Impl = nullptr;
};

class Texture2D
{
public:
    Texture2D() = default;
    Texture2D(const std::string path);
    Texture2D(const std::string name, const std::string path);
    ~Texture2D();

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    void Bind(uint32_t slot = 0);
    void Unbind() const;

    uint32_t GetTexID() const { return m_TextureID; }
    std::string GetName() const { return m_Name; }
    std::string GetPath() const { return m_Path; }

public:
    static Texture2D* LoadTexture(const std::string name, const std::string path);

private:
    static uint32_t CreateTextureGPU(GLenum format, uint32_t width, uint32_t height, const void *imagePtr);

private:
    std::string m_Name;
    std::string m_Path;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_TextureID;
};

#endif //LEARNOPENGL_TEXTURE_H
