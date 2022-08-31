#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/**
 * Load material textures
 * @param mat assimp Material
 * @param type assimp Texture Type
 * @param typeName texture type name e.g. `texture_diffuseN`
 */
void LoadMaterialTextures(const std::string &dir,
                          std::vector<Texture2D*> &texList, aiMaterial *mat,
                          aiTextureType type, const std::string &typeName)
{
    for(int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // TODO: optimize the duplicate textures from loading again

        std::string path = dir + "/" + std::string(str.C_Str());
        std::string name = typeName + std::to_string(i+1);
        auto tex = Texture2D::LoadTexture(name, path);
        texList.push_back(tex);
    }
}

/**
 * Process mesh
 * @param node
 * @param scene
 * @return
 */
Mesh* ProcessMesh(Model *model, aiMesh *mesh, const aiScene *scene)
{
    Mesh *m = new Mesh;

    if(!mesh->HasPositions())
    {
        std::cerr << "Model: the mesh doesn't have position (mVertices)" << '\n';
        return nullptr;
    }

    for(int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v;
        v.position.x = mesh->mVertices[i].x;
        v.position.y = mesh->mVertices[i].y;
        v.position.z = mesh->mVertices[i].z;

        if(mesh->HasNormals())
        {
            v.normal.x = mesh->mNormals[i].x;
            v.normal.y = mesh->mNormals[i].y;
            v.normal.z = mesh->mNormals[i].z;
        }

        if(mesh->HasTextureCoords(0))
        {
            v.texCoords.x = mesh->mTextureCoords[0][i].x;
            v.texCoords.y = mesh->mTextureCoords[0][i].y;
            // TODO: tangent and bitangent
        }
        else
            v.texCoords = {0.f, 0.f};
        //
        m->m_Vertices.push_back(v);
    }

    // Iterate through all faces
    for(int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // Get all indices
        for(int j = 0; j < face.mNumIndices; j++)
            m->m_Indices.push_back(face.mIndices[j]);
    }

    // Materials
    if(mesh->mMaterialIndex < 0)
    {
        std::cerr << "Model: the mesh doesn't have material (mesh->mMaterialIndex < 0)" << '\n';
    }
    else
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture2D*> diffuseMaps;
        LoadMaterialTextures(model->GetDirectory(), diffuseMaps, material, aiTextureType_DIFFUSE, "texture_diffuse");
        std::vector<Texture2D*> specularMaps;
        LoadMaterialTextures(model->GetDirectory(), specularMaps, material, aiTextureType_SPECULAR, "texture_specular");
        // TODO: normal, height map

        // Insert to texture list
        m->m_Textures.insert(m->m_Textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        m->m_Textures.insert(m->m_Textures.end(), specularMaps.begin(), specularMaps.end());
    }

    m->Init();

    return m;
}

/**
 * Load a Model recursively
 * @param node
 * @param scene
 */
void ProcessNode(Model *model, std::vector<Mesh*> &meshes, aiNode *node, const aiScene *scene)
{
    // Load all meshes of current node
    for(int i = 0; i < node->mNumMeshes; i++)
    {
        auto meshIdx = node->mMeshes[i];
        aiMesh *nodeMesh = scene->mMeshes[meshIdx];
        meshes.push_back(ProcessMesh(model, nodeMesh, scene));
    }

    // Load children nodes
    for(int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(model, meshes, node->mChildren[i], scene);
    }
}

/////////////////////////////////////////////////

Model::Model(const std::string &path)
    : m_Path(path)
{
    LoadModel(m_Path);
}

void Model::Render(Shader &shader)
{
    for(Mesh *m : m_Meshes)
    {
        // TODO: refactor this s**t
        auto a = m->GetPosition();
        auto b = m->GetRotation();
        auto c = m->GetScale();

        m->SetPosition(m_Position + m->GetPosition());
        m->SetRotation(m_Rotation + m->GetRotation());
        m->SetScale(m_Scale * m->GetScale());

//        printf("(%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f) (%.2f, %.2f, %.2f)\n",
//               m->GetPosition().x, m->GetPosition().y, m->GetPosition().z,
//               m->GetRotation().x, m->GetRotation().y, m->GetRotation().z,
//               m->GetScale().x, m->GetScale().y, m->GetScale().z);

        m->SetPosition(a);
        m->SetRotation(b);
        m->SetScale(c);

        m->Render(shader);
    }
}

void Model::LoadModel(const std::string &path)
{
    m_Path = path;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate /*| aiProcess_FlipUVs*/);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Assimp: " << importer.GetErrorString() << '\n';
        return;
    }

    ProcessNode(this, m_Meshes, scene->mRootNode, scene);
}
