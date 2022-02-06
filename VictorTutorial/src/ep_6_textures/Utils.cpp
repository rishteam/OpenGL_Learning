#include "Utils.h"

#include <fstream>
#include <iostream>

#include <stb_image.h>

std::string ReadFileContent(const std::filesystem::path &target)
{
    std::filesystem::path path;
    // TODO: replace this with std::filesystem::absolute
    if(!target.is_absolute())
    {
        path = std::filesystem::current_path() / target;
    }
    std::ifstream file(path);
    if(!file)
    {
        std::cout << "Failed to open file: " << path.string() << '\n';
        return "";
    }


    std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>())); // read contents
    return content;
}

Ref<Image> LoadImage(const std::filesystem::path &path)
{
    Ref<Image> image = MakeRef<Image>();
    stbi_set_flip_vertically_on_load(true);

    if(!std::filesystem::exists(path))
    {
        printf("Failed to load image: %s. Reason: Do not exist", path.string().c_str());
        return image;
    }

    image->bytes = stbi_load(path.string().c_str(), &image->width, &image->height, &image->numChannel, 0);

    if(!image->bytes)
    {
        printf("Failed to load image: %s. Reason: stbi_load() error", path.string().c_str());
        return image;
    }

    return image;
}

Image::~Image()
{
    if(bytes)
        stbi_image_free(bytes);
}
