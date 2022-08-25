#pragma once
#ifndef LEARNOPENGL_MESH_H
#define LEARNOPENGL_MESH_H

#include <vector>
#include <glm/glm.hpp>

#include "Texture.h"
#include "Vertex.h"
#include "Shader.h"
#include "VertexArray.h"

// TODO: 每創建一個 Mesh object 就會有頂點資料重複儲存的問題

class Mesh
{
public:
    std::vector<Vertex> m_Vertices;
    std::vector<uint32_t> m_Indices;
    std::vector<Texture2D> m_Textures;
    glm::vec3 m_Position;
    glm::vec3 m_Rotation; // degree
    glm::vec4 m_Color{1.f, 1.f, 1.f, 1.f};
    glm::vec3 m_Scale{1.f};

public:
    Mesh() = default;
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture2D> &textures);

    virtual void Render(Shader &shader);
    void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices, const std::vector<Texture2D> &textures);
    bool Init();

    static Mesh* LoadMesh(const std::string &file);

public:
    // TODO: Add non-glm setter
    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3 &GetRotation() const { return m_Rotation; }
    const glm::vec4 &GetColor() const { return m_Color; }
    const glm::vec3 &GetScale() const { return m_Scale; }
    void SetPosition(const glm::vec3 &pos) { m_Position = pos; }
    void SetPosition(float x, float y, float z) { SetPosition({x, y, z}); }
    void SetRotation(const glm::vec3 &mRotation) { m_Rotation = mRotation; }
    void SetRotation(float x, float y, float z) { SetRotation({x, y, z}); }
    void SetColor(const glm::vec4 &mColor) { m_Color = mColor; }
    void SetColor(float r, float g, float b, float a) { SetColor({r, g, b, a}); }
    void SetScale(const glm::vec3 &mScale) { m_Scale = mScale; }
    void SetScale(float x, float y, float z) { SetScale({x, y, z}); }

protected:
    // TODO: Remove this
    Mesh(float *vertices, size_t verticeSize, uint32_t *indices, size_t indiceCount, const std::vector<Texture2D> &textures);
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
    Texture2D *m_Diffuse;
    Texture2D *m_Specular;
};

// TODO: Impl
class SphereMesh : public Mesh
{
public:
    SphereMesh();
};

#endif //LEARNOPENGL_MESH_H
