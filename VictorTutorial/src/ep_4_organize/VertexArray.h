#pragma once
#ifndef VICTORTUTORIAL_VERTEXARRAY_H
#define VICTORTUTORIAL_VERTEXARRAY_H

#include <glad/glad.h>

#include "Ref.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class VertexArray
{
public:
    VertexArray();
    ~VertexArray();
    void Delete();

    void Bind() const;
    void Unbind() const;

    void BindVertexBuffer(const Ref<VertexBuffer> &vbo);
    void BindIndexBuffer(const Ref<IndexBuffer> &ibo);
private:
    GLuint m_ID;
};


#endif //VICTORTUTORIAL_VERTEXARRAY_H
