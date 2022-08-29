#pragma once
#ifndef LEARNOPENGL_DEBUG_H
#define LEARNOPENGL_DEBUG_H

#include <glad/glad.h>
#include <string_view>
#include <fmt/format.h>
#include <debugbreak.h>

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);

void EnableOpenGLDebug();
void DisableOutputBuffer();

constexpr std::string_view GetFileName(const char *path)
{
    std::string_view s(path);
    auto pos = s.find_last_of("\\");
    if(pos != std::string_view::npos)
        return {path+pos+1};
    return s;
}

#ifndef NDEBUG
#define RL_ASSERT(x, s, ...)                                                \
    do {                                                                    \
        if(!(x)) {                                                          \
            std::cerr << "Assert: " << GetFileName(__FILE__) << ":" << __LINE__ << ' ';  \
            std::cerr << fmt::format(s, ##__VA_ARGS__);                     \
            debug_break();                                                  \
        }                                                                   \
    } while(0)
#else
#define RL_ASSERT(x, s, ...)                                                \
    do {                                                                    \
        if(!(x)) {                                                          \
            std::cerr << "Assert: " << GetFileName(__FILE__) << ":" << __LINE__ << ' ';  \
            std::cerr << fmt::format(s, ##__VA_ARGS__);                     \
        }                                                                   \
    } while(0)
#endif

#endif //LEARNOPENGL_DEBUG_H
