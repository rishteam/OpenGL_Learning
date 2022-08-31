#pragma once
#ifndef LEARNOPENGL_OBJECT_H
#define LEARNOPENGL_OBJECT_H

#include <glm/glm.hpp>

class Object3D
{
public:
    Object3D() = default;

    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetRotation() const { return m_Rotation; }
    const glm::vec4& GetColor() const { return m_Color; }
    const glm::vec3& GetScale() const { return m_Scale; }
    void SetPosition(const glm::vec3 &pos) { m_Position = pos; }
    void SetPosition(float x, float y, float z) { SetPosition({x, y, z}); }
    void SetRotation(const glm::vec3 &mRotation) { m_Rotation = mRotation; }
    void SetRotation(float x, float y, float z) { SetRotation({x, y, z}); }
    void SetColor(const glm::vec4 &mColor) { m_Color = mColor; }
    void SetColor(float r, float g, float b, float a) { SetColor({r, g, b, a}); }
    void SetScale(const glm::vec3 &mScale) { m_Scale = mScale; }
    void SetScale(float x, float y, float z) { SetScale({x, y, z}); }

protected:
    glm::vec3 m_Position{0.f};
    glm::vec3 m_Rotation{0.f}; // degree
    glm::vec4 m_Color{1.f, 1.f, 1.f, 1.f};
    glm::vec3 m_Scale{1.f};
};

#endif //LEARNOPENGL_OBJECT_H
