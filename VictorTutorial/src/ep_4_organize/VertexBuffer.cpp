#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &m_ID);
}

VertexBuffer::VertexBuffer(GLfloat *vertices, GLsizeiptr size)
{
    glGenBuffers(1, &m_ID);
    Store(vertices, size);
}

VertexBuffer::~VertexBuffer()
{
    if(m_ID)
        Delete();
}

void VertexBuffer::Delete()
{
    glDeleteBuffers(1, &m_ID);
    m_ID = 0;
}

void VertexBuffer::Store(GLfloat *vertices, GLsizeiptr size)
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Clear()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
