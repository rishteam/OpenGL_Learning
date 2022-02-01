#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
{
    glGenBuffers(1, &m_ID);
}

IndexBuffer::IndexBuffer(GLuint *indices, GLsizeiptr size)
{
    Store(indices, size);
}

IndexBuffer::~IndexBuffer()
{
    if(m_ID)
        Delete();
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void IndexBuffer::UnBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::Store(GLuint *indices, GLsizeiptr size)
{
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void IndexBuffer::Clear()
{
    GLint previous = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previous);

    if(previous)
        glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    if(previous)
        glBindVertexArray(previous);
}

void IndexBuffer::Delete()
{
    glDeleteBuffers(1, &m_ID);
    m_ID = 0;
}
