#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "Camera.h"

extern sf::RenderWindow window;

float getRatio()
{
    auto siz = window.getSize();
    return (float)siz.x / (float)siz.y;
}

class FirstPersonView
{
public:
    FirstPersonView(glm::vec3 pos=glm::vec3{0, 0, 3}, float pitch=0.f, float yaw=0.f)
        : m_camera(pos, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), getRatio())
    {
        m_camera.setPitch(pitch);
        m_camera.setYaw(yaw);
    }

    void processEvent(sf::Event e)
    {
        if (e.type == sf::Event::MouseWheelScrolled)
        {
            mouseWheelDelta = e.mouseWheelScroll.delta;
        }
        else if (e.type == sf::Event::Resized)
        {
            m_camera.m_screenRatio = (float)e.size.width / e.size.height;
        }
        else if (e.type == sf::Event::KeyPressed)
        {
            switch (e.key.code)
            {
            case sf::Keyboard::LAlt:
                isMouseCaptured = !isMouseCaptured;
                sf::Mouse::setPosition({static_cast<int>(window.getSize().x / 2), static_cast<int>(window.getSize().y / 2)}, window);
                break;
            }
        }
        else if(e.type == sf::Event::GainedFocus)
        {
            isWindowFocus = true;
        }
        else if(e.type == sf::Event::LostFocus)
        {
            isWindowFocus = false;
        }
    }
    void update(float dt)
    {
        // Zoom
        if (mouseWheelDelta != 0.f)
        {
            m_camera.m_fov += mouseWheelDelta;
            if (m_camera.m_fov < 1.f)
                m_camera.m_fov = 1.f;
            if (m_camera.m_fov > 45.f)
                m_camera.m_fov = 45.f;
            mouseWheelDelta = 0.f;
        }

        // Mouse move
        if (isMouseCaptured)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            window.setMouseCursorGrabbed(true);
            //
            auto [x, y] = sf::Mouse::getPosition(window);
            ImGui::Begin("Matrix");
            ImGui::Text("mpos = %d %d", x, y);
            ImGui::Text("window size = %d %d", window.getSize().x, window.getSize().y);
            ImGui::End();
            // Get cursor pos
            glm::vec2 nowCursorPos(x, window.getSize().y - y);
            // Get screen center
            glm::vec2 screenCenter = glm::vec2(window.getSize().x / 2, window.getSize().y / 2);
            if (window.getSize().y % 2 != 0)
                screenCenter.y += 1;
            // lock the cursor pos
            sf::Mouse::setPosition({static_cast<int>(window.getSize().x / 2), static_cast<int>(window.getSize().y / 2)}, window);
            // calc the offset
            glm::vec2 off = nowCursorPos - screenCenter;
            ImGui::Begin("Matrix");
            ImGui::Text("%.2f %.2f", off.x, off.y);
            ImGui::End();

            off *= sensitivity;
            m_camera.addPitch(off.y);
            m_camera.addYaw(off.x);
        }
        else
            window.setMouseCursorGrabbed(false);

        glm::vec3 cameraFront = m_camera.getFrontVector();
        glm::vec3 cameraRight = m_camera.getRightVector();
        glm::vec3 cameraUp = m_camera.getUpVector();
        glm::vec3 &cameraPos = m_camera.m_pos;

        if(!isWindowFocus)
            return;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            cameraSpeed = dt * 5;
        else
            cameraSpeed = dt * 3;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cameraPos += cameraSpeed * cameraFront;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cameraPos -= cameraSpeed * cameraFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cameraPos -= cameraSpeed * cameraRight;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cameraPos += cameraSpeed * cameraRight;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            cameraPos += cameraSpeed * cameraUp;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            cameraPos -= cameraSpeed * cameraUp;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            m_camera.addYaw(-1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            m_camera.addYaw(1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            m_camera.addPitch(1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            m_camera.addPitch(-1.f);
    }

    const glm::vec3 getPosition()
    {
        return m_camera.m_pos;
    }
    const float getPitch()
    {
        return m_camera.m_pitch;
    }
    const float getYaw()
    {
        return m_camera.m_yaw;
    }
    const glm::vec3 getFrontVector() const { return m_camera.getFrontVector(); }
    const glm::mat4 getViewMatrix()
    {
        return m_camera.getViewMatrix();
    }
    const glm::mat4 getProjectionMatrix()
    {
        return m_camera.getProjectionMatrix();
    }
    void setMouseCapture(bool capture)
    {
        isMouseCaptured = capture;
    }
    bool getMouseCapture()
    {
        return isMouseCaptured;
    }

// private:
    Camera m_camera;
    float cameraSpeed = 0.05f;
    float sensitivity = 0.05f;

    float mouseWheelDelta = 0.f;
    bool isMouseCaptured = false;
    bool isWindowFocus = true;
};