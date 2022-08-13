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
#include "cube_vertices_normal_tex.h"
#include "sphere_vertices_normal.h"

uint32_t screenWidth = 1280, screenHeight = 720;
sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4, sf::ContextSettings::Attribute::Debug));

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

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

    // Load Shaders
    Shader phongShader("phong.glsl");
    Shader phongDiffuseShader("phong_specular_map.glsl");
    Shader lightBulbShader("lightcube.glsl");

    Texture2D boxTex("../../../assets/box.png");
    Texture2D boxSpecularMap("../../../assets/boxOutline.png");
    // TODO: fix setTexture for now it's *add* texture
    phongDiffuseShader.setTexture("material.diffuse", boxTex);
    phongDiffuseShader.setTexture("material.specular", boxSpecularMap);

    // Load vertices
    VertexArray box;
    VertexBuffer *a = new VertexBuffer;
    VertexBuffer *b = new VertexBuffer;
    {
        a->setData(cube_vertices_normal_tex, sizeof(cube_vertices_normal_tex));
        a->setLayout({{ShaderDataType::Float3, "aPos"},
                      {ShaderDataType::Float3, "aNormal"},
                      {ShaderDataType::Float2, "aTexCoords"}});
        box.addVertexBuffer(a);
    }
    VertexArray light;
    {
        b->setData(cube_vertices_normal, sizeof(cube_vertices_normal));
        b->setLayout({{ShaderDataType::Float3, "aPos"},
                       {ShaderDataType::Float3, "aNormal"}});
        light.addVertexBuffer(b);
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
    float lightClk = 0.f;
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
        static bool windowFocus = false;
        static glm::vec3 phongSpherePos(3.f, 0.f, 0.f);
        static glm::vec3 lightPos(0.0f, 0.f, 0.f), lightColor(1.f, 1.f, 1.f);
        static float lightAmbientStrength = 0.2f, lightDiffuseStrength = 0.5f;
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
            windowFocus = ImGui::IsWindowFocused();
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
            ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.1f);
            ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
            ImGui::Text("diffuse = diff_coff * lightColor\nambient = diffuse * ambient_coff");
            ImGui::DragFloat("ambient coff", &lightAmbientStrength, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("diffuse coff", &lightDiffuseStrength, 0.01f, 0.f, 1.f);

            ImGui::Separator();
            ImGui::DragFloat3("Sphere Position", glm::value_ptr(phongSpherePos));

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
            if(!windowFocus)
                fpsView.update(dt);

            lightClk += dt;

            if(rotate)
            {
                lightPos.x = 3.f + 3 * cos(lightClk);
                lightPos.z = 0.f + 3 * sin(lightClk);
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

            phongShader.setFloat3("fViewPos", fpsView.getPosition());
            Material m = {
                {1.0f, 0.5f, 0.31f},
                {1.0f, 0.5f, 0.31f},
                {0.5f, 0.5f, 0.5f},
                32
            };
            phongShader.setFloat3("material.ambient", m.ambient);
            phongShader.setFloat3("material.diffuse", m.diffuse);
            phongShader.setFloat3("material.specular", m.specular);
            phongShader.setFloat("material.shininess", m.shininess);

            phongShader.setFloat3("light.pos", lightPos);
            glm::vec3 diffuse = lightColor * lightDiffuseStrength;
            glm::vec3 ambient = diffuse * lightAmbientStrength;
            phongShader.setFloat3("light.ambient",  ambient);
            phongShader.setFloat3("light.diffuse",  diffuse);
            phongShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f);
            //
            lightBulbShader.setMat4("vView", view);
            lightBulbShader.setMat4("vProjection", projection);
            //
            phongDiffuseShader.setMat4("vView", view);
            phongDiffuseShader.setMat4("vProjection", projection);
            phongDiffuseShader.setFloat3("fViewPos", fpsView.getPosition());

            phongDiffuseShader.setFloat("material.shininess", 64.f);
            phongDiffuseShader.setFloat3("light.pos", lightPos);
            diffuse = lightColor * lightDiffuseStrength;
            ambient = diffuse * lightAmbientStrength;
            phongDiffuseShader.setFloat3("light.ambient",  ambient); // 通常比較低強度
            phongDiffuseShader.setFloat3("light.diffuse",  diffuse); // 設成希望光的顏色，将光照调暗了一些以搭配场景
            phongDiffuseShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f); // 通常是白色

            // Draw phong-shaded sphere
            {
                sphere.bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, phongSpherePos);
//                model = glm::rotate(model, glm::radians(0.0), glm::vec3(0.f, 1.f, 0.f));
                phongShader.setMat4("vModel", model);
                glDrawElements(GL_TRIANGLES, sphere_indices_size, GL_UNSIGNED_INT, 0);
                sphere.unbind();
            }

            // test
            {
                box.bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(-3.f, 0.f, 0.f));
//                model = glm::rotate(model, glm::radians(0.0), glm::vec3(0.f, 1.f, 0.f));
                phongDiffuseShader.setMat4("vModel", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                box.unbind();
            }

            // Draw light source box
            {
                light.bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, lightPos);
                model = glm::scale(model, glm::vec3(0.2f));
                lightBulbShader.setMat4("vModel", model);
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