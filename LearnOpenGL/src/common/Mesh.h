#pragma once
#ifndef LEARNOPENGL_MESH_H
#define LEARNOPENGL_MESH_H

#include <vector>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Vertex.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Object.h"

// TODO: 每創建一個 Mesh object 就會有頂點資料重複儲存的問題

class Mesh : public Object3D
{
public:
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    std::vector<Texture2D*> m_Textures;

public:
    Mesh() = default;
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture2D*> &textures);

    virtual void Render(Shader &shader);
    void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture2D*> &textures);
    bool Init();

    static Mesh* LoadMesh(const std::string &file);

protected:
    // TODO: Remove this
    Mesh(float *vertices, size_t verticeSize, uint32_t *indices, size_t indiceCount, const std::vector<Texture2D*> &textures);
private:

    VertexArray m_VA;
    VertexBuffer m_VB;
    IndexBuffer m_IB;
};

class CubeMesh : public Mesh
{
public:
    CubeMesh(Texture2D *diffuse=nullptr, Texture2D *specular=nullptr);

    virtual void Render(Shader &shader);

private:
    Texture2D *m_Diffuse = nullptr;
    Texture2D *m_Specular = nullptr;
};

// TODO: Impl
class SphereMesh : public Mesh
{
public:
    SphereMesh();
};

#endif //LEARNOPENGL_MESH_H
