#pragma once
#ifndef LEARNOPENGL_MODEL_H
#define LEARNOPENGL_MODEL_H

#include <vector>
#include <string>
#include <filesystem>

#include "Mesh.h"
#include "Object.h"
#include "Shader.h"

class Model : public Object3D
{
public:
    Model() = default;
    Model(const std::string &path);

    void Render(Shader &shader);

    std::string GetPath() { return m_Path; }
    std::string GetDirectory() { return std::filesystem::path(m_Path).parent_path().string(); }

private:
    void LoadModel(const std::string &path);

private:
    std::vector<Mesh*> m_Meshes;
    std::string m_Path;
};

#endif //LEARNOPENGL_MODEL_H
