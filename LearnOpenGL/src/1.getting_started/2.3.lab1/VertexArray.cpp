#include "VertexArray.h"

GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:    return GL_FLOAT;
        case ShaderDataType::Float2:   return GL_FLOAT;
        case ShaderDataType::Float3:   return GL_FLOAT;
        case ShaderDataType::Float4:   return GL_FLOAT;
        case ShaderDataType::Mat3:     return GL_FLOAT;
        case ShaderDataType::Mat4:     return GL_FLOAT;
        case ShaderDataType::Int:      return GL_INT;
        case ShaderDataType::Int2:     return GL_INT;
        case ShaderDataType::Int3:     return GL_INT;
        case ShaderDataType::Int4:     return GL_INT;
        case ShaderDataType::Bool:     return GL_BOOL;
    }

    assert(false);
    return 0;
}

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_ID);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::AddVertexBuffer(VertexBuffer *vertexBuf)
{
    glBindVertexArray(m_ID);
    vertexBuf->bind();
    assert(vertexBuf->getLayout().getElements().size() > 0);

    const auto &layout = vertexBuf->getLayout();
    for(const auto & element : layout)
    {
        glEnableVertexAttribArray(m_VertexBufferIndex);
        glVertexAttribPointer(m_VertexBufferIndex,
                              element.getComponentCount(),
                              ShaderDataTypeToOpenGLBaseType(element.type),
            element.normalized ? GL_TRUE : GL_FALSE,
                              layout.getStride(),
                              (const void*)element.offset);
        m_VertexBufferIndex++;
    }

    m_VertexBuffer = vertexBuf;
}

void VertexArray::SetIndexBuffer(IndexBuffer *indexBuf)
{
    glBindVertexArray(m_ID);
    indexBuf->bind();
    m_IndexBuffer = indexBuf;
}