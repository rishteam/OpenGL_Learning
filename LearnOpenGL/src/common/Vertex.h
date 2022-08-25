#pragma once
#ifndef LEARNOPENGL_VERTEX_H
#define LEARNOPENGL_VERTEX_H

#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    float* ValuePtr() { return reinterpret_cast<float *>(&position); }
};

#endif //LEARNOPENGL_VERTEX_H
