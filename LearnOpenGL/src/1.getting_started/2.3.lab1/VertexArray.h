#pragma once

#include <cstdint>

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

private:
    uint32_t vao;

};
