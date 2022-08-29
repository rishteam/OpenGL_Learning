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
    VertexArray(const VertexArray &va);
    ~VertexArray();

    VertexArray& operator=(const VertexArray& va);

    void Bind() const;
    void Unbind() const;

    void AddVertexBuffer(VertexBuffer *vertexBuf);
    void SetIndexBuffer(IndexBuffer *indexBuf);
private:
    uint32_t m_ID;
    VertexBuffer* m_VertexBuffer = nullptr;
    IndexBuffer *m_IndexBuffer = nullptr;
    uint32_t m_VertexBufferIndex = 0;
};
