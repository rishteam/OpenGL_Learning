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
    glGenVertexArrays(1, &vao);
    printf("VertexArray::VertexArray(): vao = %d\n", vao);
}

VertexArray::VertexArray(const VertexArray &va)
{
    glGenVertexArrays(1, &vao);
    if(va.m_vertexBuffer)
        addVertexBuffer(va.m_vertexBuffer);
    if(va.m_indexBuffer)
        setIndexBuffer(va.m_indexBuffer);
}

VertexArray& VertexArray::operator=(const VertexArray &va)
{
    if(vao)
        glDeleteVertexArrays(1, &vao);
    glGenVertexArrays(1, &vao);
    if(va.m_vertexBuffer)
        addVertexBuffer(va.m_vertexBuffer);
    if(va.m_indexBuffer)
        setIndexBuffer(va.m_indexBuffer);
    return *this;
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &vao);
}

void VertexArray::bind() const
{
    glBindVertexArray(vao);
}

void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::addVertexBuffer(VertexBuffer *vertexBuffer)
{
    glBindVertexArray(vao);
    vertexBuffer->bind();
    assert(vertexBuffer->getLayout().getElements().size() > 0);

    const auto &layout = vertexBuffer->getLayout();
    for(const auto & element : layout)
    {
        glEnableVertexAttribArray(m_vertexBufferIndex);
        glVertexAttribPointer(m_vertexBufferIndex,
            element.getComponentCount(),
            ShaderDataTypeToOpenGLBaseType(element.type),
            element.normalized ? GL_TRUE : GL_FALSE,
            layout.getStride(),
            (const void*)element.offset);
        m_vertexBufferIndex++;
    }

    m_vertexBuffer = vertexBuffer;
}

void VertexArray::setIndexBuffer(IndexBuffer *indexBuf)
{
    glBindVertexArray(vao);
    indexBuf->bind();
    m_indexBuffer = indexBuf;
}
