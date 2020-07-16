#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * @brief General Camera Container providing a camera system with perspective projection
 * The camera rotation is implemented using Euler Angles
 */
class Camera
{
public:
    Camera(const glm::vec3 &pos, const glm::vec3 &look, const glm::vec3 orient,
            float screenRatio)
        : m_pos(pos), m_front(glm::normalize(look - pos)), m_worldUp(orient),
          m_up(0.f), m_right(0.f),
          m_view(1.f),
          m_projection(1.f)
    {
        m_screenRatio = screenRatio;
    }

    void setPosition(const glm::vec3 &pos) { m_pos = pos; }
    void setPosition(float x, float y, float z) { setPosition(glm::vec3(x, y, z)); }
    const glm::vec3& getFrontVector() const { return m_front; }
    const glm::vec3& getUpVector() const { return m_up; }
    const glm::vec3& getRightVector() const { return m_right; }

    void setPitch(float deg)
    {
        m_pitch = deg;
        // limit
        if(m_pitch > 89.f)
            m_pitch = 89.f;
        if(m_pitch < -89.f)
            m_pitch = -89.f;
    }
    void addPitch(float deg)
    {
        setPitch(m_pitch + deg);
    }
    void setYaw(float deg)
    {
        m_yaw = deg;
        if(m_yaw > 360.f || m_yaw < -360.f)
            m_yaw = glm::mod(m_yaw, 360.f);
    }
    void addYaw(float deg)
    {
        setYaw(m_yaw + deg);
    }

    const glm::mat4 &getViewMatrix()
    {
        update();
        return m_view;
    }

    const glm::mat4& getProjectionMatrix()
    {
        update();
        return m_projection;
    }

    glm::vec3 m_pos;   // camera position
    glm::vec3 m_front; // camera front (-z)
    glm::vec3 m_up;    // camera up (y)
    glm::vec3 m_right; // camera right (x)
    //
    glm::vec3 m_worldUp;
    //
    glm::mat4 m_view;
    float m_pitch = 0.f;
    float m_yaw = -90.f;
    float m_speed = 0.05f;
    //
    glm::mat4 m_projection;
    float m_fov = 45.f;
    float m_near = 0.1f;
    float m_far = 100.f;
    float m_screenRatio = 0.f;

    void update()
    {
        // Update front vector
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        newFront.y = sin(glm::radians(m_pitch));
        newFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_front = glm::normalize(newFront);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
        // Update the view matrix
        m_view = glm::lookAt(m_pos, m_pos + m_front, m_worldUp);
        // Update the projection matrix
        m_projection = glm::perspective(glm::radians(m_fov), m_screenRatio, m_near, m_far);
    }
};