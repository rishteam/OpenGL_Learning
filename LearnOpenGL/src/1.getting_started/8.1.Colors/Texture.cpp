//
// Created by icejj on 2020/10/19.
//

#include "Texture.h"
#include <glad/glad.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

Texture2D::Texture2D(std::string path) {

    sf::Image image;
    if(!image.loadFromFile(path)) printf("[Texture2D] Error on loading image");

    // image.flipVertically();

    width_ = image.getSize().x;
    height_ = image.getSize().y;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);
    glBindTexture(GL_TEXTURE_2D, textureID_);
    glTextureStorage2D(textureID_, 1, GL_RGBA8, width_, height_);

    glTextureParameteri(textureID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureSubImage2D(textureID_, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)image.getPixelsPtr());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind(uint32_t slot) {

    glBindTextureUnit(slot, textureID_);
}

void Texture2D::unbind() const{

    glBindTextureUnit(0, 0);
}