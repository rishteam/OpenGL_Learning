#pragma once
#ifndef LEARNOPENGL_DEBUG_H
#define LEARNOPENGL_DEBUG_H

#include <glad/glad.h>

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);

void EnableOpenGLDebug();
void DisableOutputBuffer();

#endif //LEARNOPENGL_DEBUG_H
