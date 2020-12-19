#pragma once

#include <memory>
#include <cstdint>
#include <cassert>

#include "Buffer.h"

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type);

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void addVertexBuffer(VertexBuffer *vertexBuf);
    void setIndexBuffer(IndexBuffer *indexBuf);
private:
    uint32_t vao;
    VertexBuffer* m_vertexBuffer = nullptr;
    IndexBuffer *m_indexBuffer = nullptr;
    uint32_t m_vertexBufferIndex = 0;
};
