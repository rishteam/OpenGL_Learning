#pragma once
#ifndef VICTORTUTORIAL_UTILS_H
#define VICTORTUTORIAL_UTILS_H

#include <filesystem>
#include <string>
#include <fstream>

#include "Ref.h"

std::string ReadFileContent(const std::filesystem::path &target);

struct Image
{
    int width, height, numChannel;
    unsigned char *bytes;

    Image() = default;
    ~Image();
};

Ref<Image> LoadImage(const std::filesystem::path &path);

#endif //VICTORTUTORIAL_UTILS_H
