#include "Texture.h"

#include <glad/glad.h>
#include <SFML/Graphics.hpp>

#include <fmt/printf.h>
#include <cassert>

Texture2D::Texture2D(std::string path)
{
    sf::Image image;
    assert(image.loadFromFile(path) == true);

    // image.flipVertically();

    m_Width = image.getSize().x;
    m_Height = image.getSize().y;
    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    if(path.find(".png") != std::string::npos)
        glTextureStorage2D(m_TextureID, 1, GL_RGBA8, m_Width, m_Height);
    else
        glTextureStorage2D(m_TextureID, 1, GL_RGB8, m_Width, m_Height);

    glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)image.getPixelsPtr());
    glBindTexture(GL_TEXTURE_2D, 0);
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
        fmt::print("[Error] Failed to Bind texture {}: m_TextureID = 0", m_Name);
        return;
    }

    glBindTextureUnit(slot, m_TextureID);
}

void Texture2D::Unbind() const
{
    glBindTextureUnit(0, 0);
}

