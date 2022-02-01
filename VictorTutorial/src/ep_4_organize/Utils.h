#pragma once
#ifndef VICTORTUTORIAL_UTILS_H
#define VICTORTUTORIAL_UTILS_H

#include <filesystem>
#include <string>
#include <fstream>


std::string ReadFileContent(const std::filesystem::path &target);

#endif //VICTORTUTORIAL_UTILS_H
