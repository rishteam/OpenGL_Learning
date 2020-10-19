//
// Created by icejj on 2020/10/19.
//
#pragma once
#ifndef LEARNOPENGL_TEXTURE_H
#define LEARNOPENGL_TEXTURE_H
#include <iostream>
#include <string>


class Texture2D {

public:
    Texture2D() = default;
    Texture2D(const std::string path);
    ~Texture2D();

    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }

    void bind(uint32_t slot = 0);
    void unbind() const;

private:

    std::string path;
    uint32_t width_;
    uint32_t height_;
    uint32_t textureID_;
};



#endif //LEARNOPENGL_TEXTURE_H
