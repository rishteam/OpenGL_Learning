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
    int width = 0, height = 0, numChannel = 0;
    unsigned char *bytes = nullptr;

    Image() = default;
    ~Image();
};

Ref<Image> LoadImage(const std::filesystem::path &path);

#endif //VICTORTUTORIAL_UTILS_H
