#include "Mesh.h"

#define CUBE_VERTICES_NORMAL_TEX_IMPL
#include "cube_vertices_normal_tex.h"

#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
           const std::vector<Texture2D> &textures)
{
    SetData(vertices, indices, textures);
}

void Mesh::Render(Shader &shader)
{
    m_VA.bind();
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, m_Position);
    model = glm::rotate(model, glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, m_Scale);
    shader.setMat4("vModel", model);
    shader.bind();

    if(!m_Indices.empty())
        glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());

    shader.unbind();
    m_VA.unbind();
    shader.unbind();
}

void Mesh::SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices,
                   const std::vector<Texture2D> &textures)
{
    m_Vertices = vertices;
    m_Indices = indices;
    m_Textures = textures;

    Init();
}

bool Mesh::Init()
{
    // Vertex Buffer
    m_VB.setData(m_Vertices[0].ValuePtr(), sizeof(Vertex) * m_Vertices.size());
    m_VB.setLayout({{ShaderDataType::Float3, "aPos"},
                    {ShaderDataType::Float3, "aNormal"},
                    {ShaderDataType::Float2, "aTexCoords"}});
    m_VA.addVertexBuffer(&m_VB);

    // Index Buffer
    m_IB.setData(const_cast<uint32_t *>(&(m_Indices[0])), m_Indices.size());
    m_VA.setIndexBuffer(&m_IB);

    return true;
}

Mesh::Mesh(float *vertices, size_t verticeSize, uint32_t *indices, size_t indiceCount,
           const std::vector<Texture2D> &textures)
{
    Vertex v{};
    for(int i = 0; i < verticeSize; i += 8)
    {
        v.position = {vertices[i+0], vertices[i+1], vertices[i+2]};
        v.normal = {vertices[i+3], vertices[i+4], vertices[i+5]};
        v.texCoords = {vertices[i+6], vertices[i+7]};
        m_Vertices.push_back(v);
    }

    printf("Mesh::Mesh: m_Vertices.size() = %d\n", m_Vertices.size());

    if(indices)
    {
        m_Indices.assign(indices, indices + indiceCount);
    }

    // Vertex Buffer
    m_VB.setData(vertices, sizeof(Vertex) * verticeSize);
    m_VB.setLayout({{ShaderDataType::Float3, "aPos"},
                    {ShaderDataType::Float3, "aNormal"},
                    {ShaderDataType::Float2, "aTexCoords"}});
    m_VA.addVertexBuffer(&m_VB);

    // Index Buffer
    if(indices)
    {
        m_IB.setData(indices, indiceCount);
        m_VA.setIndexBuffer(&m_IB);
    }
}

////////////// static function //////////////////

Mesh *Mesh::LoadMesh(const std::string &file)
{
    return nullptr;
}

////////////////////////////////////////////////

CubeMesh::CubeMesh(Texture2D *diffuse, Texture2D *specular)
    : Mesh(cube_vertices_normal_tex, sizeof(cube_vertices_normal_tex), nullptr, 0, {})
{
    m_Diffuse = diffuse;
    m_Specular = specular;
}

void CubeMesh::Render(Shader &shader)
{
    if(m_Diffuse)
        shader.setTexture("material.diffuse", *m_Diffuse);
    if(m_Specular)
        shader.setTexture("material.specular", *m_Specular);
    Mesh::Render(shader);
}

////////////////////////////////////////////////

SphereMesh::SphereMesh()
{

}
