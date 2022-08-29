#include "Texture.h"

#include <unordered_map>


#include <SFML/Graphics.hpp>

#include <fmt/printf.h>
#include <cassert>

Texture2D::Texture2D(std::string path)
    : m_Path(path),
      m_Name(path)
{
    Image image(m_Path);

    m_Width = image.GetWidth();
    m_Height = image.GetHeight();

    GLenum format = path.find(".png") != std::string::npos ? GL_RGBA8 : GL_RGB8; // TODO: think more elegant solution
    m_TextureID = CreateTextureGPU(format, m_Width, m_Height, image.GetImagePtr());
}

Texture2D::Texture2D(const std::string name, const std::string path)
    : m_Path(path),
      m_Name(name)
{
    Image image(m_Path);

    m_Width = image.GetWidth();
    m_Height = image.GetHeight();

    GLenum format = path.find(".png") != std::string::npos ? GL_RGBA8 : GL_RGB8; // TODO: think more elegant solution
    m_TextureID = CreateTextureGPU(format, m_Width, m_Height, image.GetImagePtr());
}

Texture2D::~Texture2D()
{
    if(m_TextureID)
        glDeleteTextures(1, &m_TextureID);
}

void Texture2D::Bind(uint32_t slot)
{
    if(m_TextureID <= 0)
    {
        fmt::print("[Error] Failed to Bind texture {}: m_TextureID = {}", m_Name, m_TextureID);
        return;
    }

    glBindTextureUnit(slot, m_TextureID);
}

void Texture2D::Unbind() const
{
    glBindTextureUnit(0, 0);
}

uint32_t Texture2D::CreateTextureGPU(GLenum format, uint32_t width, uint32_t height, const void *imagePtr)
{
    uint32_t textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTextureStorage2D(textureID, 1, format, width, height);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureSubImage2D(textureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imagePtr);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

// TODO: add release
std::unordered_map<std::string, Texture2D*> g_TextureStore;

Texture2D *Texture2D::LoadTexture(const std::string name, const std::string path)
{
    Texture2D *tex;
    if(!g_TextureStore.count(name))
    {
        tex = new Texture2D(name, path);
        g_TextureStore[name] = tex;
    }
    else
    {
        tex = g_TextureStore[name];
    }
    return tex;
}

////////////////////////////////////////////////////////

struct Image::Impl
{
    sf::Image m_Img;
};

Image::Image()
{
    m_Impl = new Image::Impl;
}

Image::Image(const std::string &path)
{
    m_Impl = new Image::Impl;
    assert(m_Impl->m_Img.loadFromFile(path) == true);
}

Image::Image(const Image &other)
{
    m_Impl = new Image::Impl(*other.m_Impl);
}

Image::~Image()
{
    delete m_Impl;
}

Image& Image::operator=(const Image &other)
{
    delete m_Impl;
    m_Impl = new Image::Impl(*other.m_Impl);
    return *this;
}

uint32_t Image::GetWidth()
{
    return m_Impl->m_Img.getSize().x;
}

uint32_t Image::GetHeight()
{
    return m_Impl->m_Img.getSize().y;
}

const void *Image::GetImagePtr()
{
    return (const void*)m_Impl->m_Img.getPixelsPtr();
}
