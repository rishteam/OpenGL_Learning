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
    Shader phongMultiLightShader("phong_multi_light.glsl");
    Shader lightBulbShader("lightcube.glsl");

    Texture2D boxTex("../../../assets/box.png");
    Texture2D boxSpecularMap("../../../assets/boxOutline.png");
    Texture2D boxEmissionMap("../../../assets/box_emit.png");
    // TODO: fix setTexture for now it's *add* texture
    phongMultiLightShader.setTexture("material.diffuse", boxTex);
    phongMultiLightShader.setTexture("material.specular", boxSpecularMap);
    phongMultiLightShader.setFloat("material.shininess", 64.f);

    // Load vertices
    VertexArray box;
    VertexBuffer *a = new VertexBuffer;
    VertexBuffer *b = new VertexBuffer;
    {
        a->setData(cube_vertices_normal_tex, cube_vertices_size_normal_tex);
        a->setLayout({{ShaderDataType::Float3, "aPos"},
                      {ShaderDataType::Float3, "aNormal"},
                      {ShaderDataType::Float2, "aTexCoords"}});
        box.AddVertexBuffer(a);
    }
    VertexArray boxWithNormal;
    {
        b->setData(cube_vertices_normal, sizeof(cube_vertices_normal));
        b->setLayout({{ShaderDataType::Float3, "aPos"},
                       {ShaderDataType::Float3, "aNormal"}});
        boxWithNormal.AddVertexBuffer(b);
    }
    VertexArray sphere;
    VertexBuffer vb2;
    vb2.setData(sphere_vertices, sizeof(float)*sphere_vertices_size);
    vb2.setLayout({{ShaderDataType::Float3, "aPos"},
                   {ShaderDataType::Float3, "aNormal"}});
    sphere.AddVertexBuffer(&vb2);
    IndexBuffer ib(sphere_indices, sphere_indices_size);
    sphere.SetIndexBuffer(&ib);

    FirstPersonView fpsView(glm::vec3{4.8f, 2.0f, 0.119f}, -20.f, 180.0f);

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
        static glm::vec3 lightPos(0.0f, 0.f, 0.f), lightColor(1.f, 1.f, 1.f);
        static glm::vec3 sunDirection(0.f, -1.f, 0.f);
        static float spotlightCutoff = 12.5f, spotlightOuterCutoff = 17.5f;
        static float lightAmbientStrength = 0.2f, lightDiffuseStrength = 0.5f;
        static glm::vec3 pointLightPositions[] = {
                {0.7f,  0.2f,  2.0f},
                {  2.3f, -3.3f, -4.0f},
                { -4.0f,  2.0f, -12.0f},
                {  0.0f,  0.0f, -3.0f}
        };
        static glm::vec3 spotLightPositions[] = {
                {3.39f, 0.50f, -0.59f}
        };
        static glm::vec3 spotLightDirs[] = {
                {-0.94f, -0.16f, -0.27f}
        };
        static glm::vec3 boxSceneOrigin(0.f, 0.f, 0.f);
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
            ImGui::DragFloat3("Light Position", glm::value_ptr(lightPos), 0.1f);
            ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
            ImGui::Text("diffuse = diff_coff * lightColor\nambient = diffuse * ambient_coff");
            ImGui::DragFloat("ambient coff", &lightAmbientStrength, 0.01f, 0.f, 1.f);
            ImGui::DragFloat("diffuse coff", &lightDiffuseStrength, 0.01f, 0.f, 1.f);
            ImGui::Separator();
            ImGui::DragFloat("Spot Light Cutoff", &spotlightCutoff);
            ImGui::DragFloat("Spot Light Outer Cutoff", &spotlightOuterCutoff);
            ImGui::Separator();
            ImGui::DragFloat3("Sun", glm::value_ptr(sunDirection), 0.1f); ImGui::SameLine();
            if(ImGui::Button("As camera"))
            {
                sunDirection = fpsView.getFrontVector();
            }

            ImGui::Separator();

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
            ImGui::Text("fpsView.m_camera.m_front = %f %f %f", fpsView.getFrontVector().x, fpsView.getFrontVector().y, fpsView.getFrontVector().z);
            projection = fpsView.getProjectionMatrix();
            ImGui::End();
        }

        // Update
        {
            if(!windowFocus)
                fpsView.update(dt);
        }

        // Render
        {
            glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Upload uniforms
            {
                glm::vec3 diffuse = lightColor * lightDiffuseStrength;
                glm::vec3 ambient = diffuse * lightAmbientStrength;
                //
                lightBulbShader.setMat4("vView", view);
                lightBulbShader.setMat4("vProjection", projection);
                //
                // phongMultiLightShader
                phongMultiLightShader.setMat4("vView", view);
                phongMultiLightShader.setMat4("vProjection", projection);
                phongMultiLightShader.setFloat3("fViewPos", fpsView.getPosition());
                phongMultiLightShader.setFloat3("dirLight.dir", sunDirection);
                phongMultiLightShader.setFloat3("dirLight.ambient", ambient);
                phongMultiLightShader.setFloat3("dirLight.diffuse", diffuse);
                phongMultiLightShader.setFloat3("dirLight.specular", 1.0f, 1.0f, 1.0f);

                for(int i = 0; i < 4; i++)
                {
                    phongMultiLightShader.setFloat3("pointLights[" + std::to_string(i) + "].pos", boxSceneOrigin + pointLightPositions[i]);
                    phongMultiLightShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.f);
                    phongMultiLightShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
                    phongMultiLightShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
                    phongMultiLightShader.setFloat3("pointLights[" + std::to_string(i) + "].ambient", ambient);
                    phongMultiLightShader.setFloat3("pointLights[" + std::to_string(i) + "].diffuse", diffuse);
                    phongMultiLightShader.setFloat3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
                }

                for(int i = 0; i < 1; i++)
                {
                    phongMultiLightShader.setFloat3("spotLights[" + std::to_string(i) + "].pos", boxSceneOrigin + spotLightPositions[i]);
                    phongMultiLightShader.setFloat3("spotLights[" + std::to_string(i) + "].dir", spotLightDirs[i]);
                    phongMultiLightShader.setFloat("spotLights[" + std::to_string(i) + "].constant", 1.f);
                    phongMultiLightShader.setFloat("spotLights[" + std::to_string(i) + "].linear", 0.09f);
                    phongMultiLightShader.setFloat("spotLights[" + std::to_string(i) + "].quadratic", 0.032f);
                    diffuse = glm::vec3(1.f, 0.1f, 0.1f) * lightDiffuseStrength;
                    ambient = diffuse * lightAmbientStrength;
                    phongMultiLightShader.setFloat3("spotLights[" + std::to_string(i) + "].ambient", ambient);
                    phongMultiLightShader.setFloat3("spotLights[" + std::to_string(i) + "].diffuse", diffuse);
                    phongMultiLightShader.setFloat3("spotLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
                    phongMultiLightShader.setFloat("spotLights[" + std::to_string(i) + "].cutOff", glm::cos(glm::radians(spotlightCutoff)));
                    phongMultiLightShader.setFloat("spotLights[" + std::to_string(i) + "].outerCutoff", glm::cos(glm::radians(spotlightOuterCutoff)));
                }
            }

            // boxes
            {
                box.Bind();
                static glm::vec3 cubePositions[] = {
                    glm::vec3( 0.0f,  0.0f,  0.0f),
                    glm::vec3( 2.0f,  5.0f, -15.0f),
                    glm::vec3(-1.5f, -2.2f, -2.5f),
                    glm::vec3(-3.8f, -2.0f, -12.3f),
                    glm::vec3( 2.4f, -0.4f, -3.5f),
                    glm::vec3(-1.7f,  3.0f, -7.5f),
                    glm::vec3( 1.3f, -2.0f, -2.5f),
                    glm::vec3( 1.5f,  2.0f, -2.5f),
                    glm::vec3( 1.5f,  0.2f, -1.5f),
                    glm::vec3(-1.3f,  1.0f, -1.5f)
                };

                for(int i = 0; i < 10; i++)
                {
                    model = glm::mat4(1.f);
                    model = glm::translate(model, boxSceneOrigin + cubePositions[i]);
                    float angle = 20.0f * (float)i;
                    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                    phongMultiLightShader.setMat4("vModel", model);
                    phongMultiLightShader.bind();
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    phongMultiLightShader.unbind();
                }
                box.Unbind();
            }

            // Render light source box
            {
                for(auto & pointLightPosition : pointLightPositions)
                {
                    boxWithNormal.Bind();
                    model = glm::mat4(1.f);
                    model = glm::translate(model, boxSceneOrigin + pointLightPosition);
                    model = glm::scale(model, glm::vec3(0.2f));
                    lightBulbShader.setFloat4("fColor", 1.f, 1.f, 1.f, 1.f);
                    lightBulbShader.setMat4("vModel", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    boxWithNormal.Unbind();
                }

                for(auto & spotLightPosition : spotLightPositions)
                {
                    boxWithNormal.Bind();
                    model = glm::mat4(1.f);
                    model = glm::translate(model, boxSceneOrigin + spotLightPosition);
                    model = glm::scale(model, glm::vec3(0.2f));
                    lightBulbShader.setFloat4("fColor", 1.f, 0.f, 0.f, 1.f);
                    lightBulbShader.setMat4("vModel", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                    boxWithNormal.Unbind();
                }
            }
        }

        window.pushGLStates();
        ImGui::SFML::Render(window);
        window.popGLStates();
        window.display();
    }

    return 0;
}