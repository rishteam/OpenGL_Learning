#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include <fmt/core.h>

#include <Player.h>

#include <VertexArray.h>
#include <Buffer.h>
#include <Shader.h>
#include <Debug.h>
#include <Texture.h>

#include "cube_vertices_normal.h"
#include "sphere_vertices_normal.h"

uint32_t screenWidth = 1280, screenHeight = 720;
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4, sf::ContextSettings::Attribute::Debug));

int main()
{
    window.setVerticalSyncEnabled(true);
    // Load OpenGL functions using glad
    if (!gladLoadGL())
    {
        printf("Something went wrong!\n");
        exit(-1);
    }
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    (void)ImGui::SFML::Init(window);
    window.setActive(true);
    window.setMouseCursorVisible(false);

    EnableOpenGLDebug();
    DisableOutputBuffer();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Load m_ShaderID
    Shader phongShader("phong.glsl");
    phongShader.setFloat3("fObjectColor", 1.0f, 0.5f, 0.31f);

    Shader lightSurfaceShader("lightcube.glsl");

    // Load vertices
    VertexArray box;
    VertexBuffer *vb = new VertexBuffer;
    {
        vb->setData(vertices, sizeof(vertices));
        vb->setLayout({{ShaderDataType::Float3, "aPos"},
                      {ShaderDataType::Float3, "aNormal"}});
        box.addVertexBuffer(vb);
    }
    VertexArray light;
    {
        vb->setData(vertices, sizeof(vertices));
        vb->setLayout({{ShaderDataType::Float3, "aPos"},
                       {ShaderDataType::Float3, "aNormal"}});
        light.addVertexBuffer(vb);
    }
    VertexArray sphere;
    VertexBuffer vb2;
    vb2.setData(sphere_vertices, sizeof(float)*sphere_vertices_size);
    vb2.setLayout({{ShaderDataType::Float3, "aPos"},
                   {ShaderDataType::Float3, "aNormal"}});
    sphere.addVertexBuffer(&vb2);
    IndexBuffer ib(sphere_indices, sphere_indices_size);
    sphere.setIndexBuffer(&ib);

    FirstPersonView fpsView(glm::vec3{-3.f, 1.f, 0.05f}, -15.f, -0.5f);

    sf::Clock stClk;
    sf::Clock deltaClock;
    float accClk = 0.f;
    bool running = true;
    while (running)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                running = false;
            else if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);
                screenWidth = event.size.width;
                screenHeight = event.size.height;
            }
            fpsView.processEvent(event);
        }
        float dt = stClk.restart().asSeconds();

        // ImGui Update
        static float bgColor[4];
        static bool wire_mode = false;
        static float ambientStrength = 0.1;
        static float specularStrength = 0.5;
        static float shininess = 32;
        static glm::vec3 phongSpherePos(3.f, 0.f, 1.f);
        static glm::vec3 lightPos(0.0f, 0.f, 0.f);
        static glm::vec4 lightColor(1.f, 1.f, 1.f, 1.f);
        static bool rotate = false;
        static bool moveX = false, moveY = false, moveZ = false;
        static glm::vec3 move_dt;
        glm::mat4 model(1.f), view(1.f), projection(1.f);
        {
            ImGui::SFML::Update(window, deltaClock.restart());

            {
                ImGui::Begin("Help", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
                ImGui::Text("2.2.3.calculate_phong_in_view_space");
                ImGui::Text("WASD - Move the camera");
                ImGui::Text("Space/Ctrl - Go up / down");
                ImGui::Text("Left Alt - Enable/Disable mouse capture");
                ImGui::Text("Esc - quit");
                ImGui::End();
            }

            ImGui::Begin("Matrix");
            if(ImGui::Button("Rotate"))
                rotate = !rotate;
            if(ImGui::Button("Move X"))
                moveX = !moveX;
            ImGui::SameLine();
            if(ImGui::Button("Move Y"))
                moveY = !moveY;
            ImGui::SameLine();
            if(ImGui::Button("Move Z"))
                moveZ = !moveZ;
            ImGui::DragFloat3("Position", glm::value_ptr(lightPos), 0.1f);
            ImGui::ColorEdit4("Light Color", glm::value_ptr(lightColor), ImGuiColorEditFlags_Float);
            ImGui::DragFloat("Ambient", &ambientStrength, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("Specular Intensity", &specularStrength, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("Specular Shininess", &shininess);
            ImGui::Separator();
            ImGui::DragFloat3("phong sphere", glm::value_ptr(phongSpherePos));

            ImGui::ColorEdit4("BG", bgColor, ImGuiColorEditFlags_Float);
            // Model matrix

            ImGui::Text("Model");
            //
            ImGui::Separator();
            // View matrix
            ImGui::Text("View");
            view = fpsView.getViewMatrix();
            ImGui::DragFloat3("Camera", glm::value_ptr(fpsView.m_camera.m_pos));
            ImGui::DragFloat("Speed", &fpsView.cameraSpeed);
            ImGui::DragFloat("Sensitivity", &fpsView.sensitivity);
            ImGui::Text("Pitch: %f Yaw: %f", fpsView.getPitch(), fpsView.getYaw());
            // Projection matrix
            ImGui::Text("Projection");
            ImGui::DragFloat("FOV", &fpsView.m_camera.m_fov, 1.0f, 0.f, 360.f);
            ImGui::DragFloat("Near", &fpsView.m_camera.m_near, 0.01);
            ImGui::DragFloat("Far", &fpsView.m_camera.m_far, 0.1);
            ImGui::Text("fpsView.m_camera.m_screenRatio = %f", fpsView.m_camera.m_screenRatio);
            projection = fpsView.getProjectionMatrix();
            ImGui::End();
        }

        // Update
        {
            fpsView.update(dt);

            if(rotate)
            {
                accClk += dt;
            }

            if(moveX)
            {
                move_dt.x += dt;
                lightPos.x = 1 * sin(move_dt.x);
            }
            if(moveY)
            {
                move_dt.y += dt;
                lightPos.y = 1 * sin(move_dt.y);
            }
            if(moveZ)
            {
                move_dt.z += dt;
                lightPos.z = 1 * sin(move_dt.z);
            }
        }

        // Draw
        {
            glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Upload uniforms
            phongShader.setMat4("vView", view);
            phongShader.setMat4("vProjection", projection);
            phongShader.setFloat("fAmbientStrength", ambientStrength);
            phongShader.setFloat("fSpecularStrength", specularStrength);
            phongShader.setFloat3("vLightPos", lightPos);
            phongShader.setFloat3("fLightColor", glm::vec3(lightColor));
            phongShader.setFloat("fShininess", shininess);
            //
            lightSurfaceShader.setMat4("vView", view);
            lightSurfaceShader.setMat4("vProjection", projection);

            // Draw phong-shaded sphere
            {
                sphere.bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, phongSpherePos);
                model = glm::rotate(model, /*glm::radians(*/accClk/*)*/, glm::vec3(0.f, 1.f, 0.f));
                phongShader.setMat4("vModel", model);
                glDrawElements(GL_TRIANGLES, sphere_indices_size, GL_UNSIGNED_INT, 0);
                sphere.unbind();
            }

            // Draw light source box
            {
                light.bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, lightPos);
                model = glm::scale(model, glm::vec3(0.2f));
                lightSurfaceShader.setMat4("vModel", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                light.unbind();
            }
        }

        window.pushGLStates();
        ImGui::SFML::Render(window);
        window.popGLStates();
        window.display();
    }

    return 0;
}