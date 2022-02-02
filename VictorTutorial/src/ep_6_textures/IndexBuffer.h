#pragma once
#ifndef VICTORTUTORIAL_INDEXBUFFER_H
#define VICTORTUTORIAL_INDEXBUFFER_H

#include <glad/glad.h>

class IndexBuffer
{
public:
    IndexBuffer();
    IndexBuffer(GLuint *indices, GLsizeiptr size);
    ~IndexBuffer();
    void Delete();

    void Bind() const;
    void UnBind() const;

    void Store(GLuint *indices, GLsizeiptr size);

    static void Clear();
private:
    GLuint m_ID;
};


#endif //VICTORTUTORIAL_INDEXBUFFER_H
