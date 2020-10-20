//
// Created by lpc0503 on 2020/8/13.
//

#ifndef BREAKOUT_TEXTURE2D_H
#define BREAKOUT_TEXTURE2D_H
#pragma once

#include <iostream>
#include <string>

class Texture2D {

public:
    Texture2D() = default;
    Texture2D(const std::string& path);
    ~Texture2D();

    uint32_t getWidth() const { return width_; }
    uint32_t getHeight() const { return height_; }

    void bind(uint32_t slot = 0) const;
    void unbind() const;

private:

    std::string path_;
    uint32_t width_;
    uint32_t height_;
    uint32_t textureID_;
};

#endif //BREAKOUT_TEXTURE2D_H
