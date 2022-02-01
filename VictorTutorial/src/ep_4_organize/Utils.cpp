#include "Utils.h"

#include <fstream>
#include <iostream>

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
