#pragma once
#ifndef VICTORTUTORIAL_VERTEXBUFFER_H
#define VICTORTUTORIAL_VERTEXBUFFER_H

#include <glad/glad.h>

class VertexBuffer
{
public:
    VertexBuffer();
    VertexBuffer(GLfloat *vertices, GLsizeiptr size);
    ~VertexBuffer();
    void Delete();

    void Store(GLfloat *vertices, GLsizeiptr size);

    void Bind() const;
    void Unbind() const;

    static void Clear();
private:
    GLuint m_ID;
};


#endif //VICTORTUTORIAL_VERTEXBUFFER_H
