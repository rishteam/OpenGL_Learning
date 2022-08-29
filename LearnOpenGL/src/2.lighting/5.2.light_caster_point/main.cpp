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
    Shader phongEmitShader("phong_emission_map.glsl");
    Shader phongSunShader("phong_sun.glsl");
    Shader phongPointLightShader("phong_point_light.glsl");
    Shader lightBulbShader("lightcube.glsl");

    Texture2D boxTex("../../../assets/box.png");
    Texture2D boxSpecularMap("../../../assets/boxOutline.png");
    Texture2D boxEmissionMap("../../../assets/box_emit.png");
    // TODO: fix setTexture for now it's *add* texture
    phongDiffuseShader.setTexture("material.diffuse", boxTex);
    phongDiffuseShader.setTexture("material.specular", boxSpecularMap);

    phongEmitShader.setTexture("material.diffuse", boxTex);
    phongEmitShader.setTexture("material.specular", boxSpecularMap);
    phongEmitShader.setTexture("material.emission", boxEmissionMap);

    phongSunShader.setTexture("material.diffuse", boxTex);
    phongSunShader.setTexture("material.specular", boxSpecularMap);
//    phongSunShader.setTexture("material.emission", boxEmissionMap);

    phongPointLightShader.setTexture("material.diffuse", boxTex);
    phongPointLightShader.setTexture("material.specular", boxSpecularMap);

    // Load vertices
    VertexArray box;
    VertexBuffer *a = new VertexBuffer;
    VertexBuffer *b = new VertexBuffer;
    {
        a->setData(cube_vertices_normal_tex, sizeof(cube_vertices_normal_tex));
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
        static glm::vec3 phongSpherePos(3.f, 0.f, 0.f);
        static glm::vec3 lightPos(0.0f, 0.f, 0.f), lightColor(1.f, 1.f, 1.f);
        static glm::vec3 sunDirection(0.f, -1.f, 0.f);
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
            ImGui::DragFloat3("Sun", glm::value_ptr(sunDirection), 0.1f); ImGui::SameLine();
            if(ImGui::Button("As camera"))
            {
                sunDirection = fpsView.getFrontVector();
            }

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
            ImGui::Text("fpsView.m_camera.m_front = %f %f %f", fpsView.getFrontVector().x, fpsView.getFrontVector().y, fpsView.getFrontVector().z);
            projection = fpsView.getProjectionMatrix();
            ImGui::End();
        }

        // Update
        {
            if(!windowFocus)
                fpsView.update(dt);


            if(rotate)
            {
                lightClk += dt;
                lightPos.x = 3.f + 3 * cos(lightClk);
                lightPos.z = 0.f + 3 * sin(lightClk);
            }

            if(moveX)
            {
                move_dt.x += dt;
                lightPos.x = 5 * sin(move_dt.x);
            }
            if(moveY)
            {
                move_dt.y += dt;
                lightPos.y = 5 * sin(move_dt.y);
            }
            if(moveZ)
            {
                move_dt.z += dt;
                lightPos.z = 5 * sin(move_dt.z);
            }

        }

        // Render
        {
            glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Upload uniforms
            {
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
                phongShader.setFloat3("light.ambient", ambient);
                phongShader.setFloat3("light.diffuse", diffuse);
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
                phongDiffuseShader.setFloat3("light.ambient", ambient); // 通常比較低強度
                phongDiffuseShader.setFloat3("light.diffuse", diffuse); // 設成希望光的顏色，将光照调暗了一些以搭配场景
                phongDiffuseShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f); // 通常是白色
                // phongEmitShader
                phongEmitShader.setMat4("vView", view);
                phongEmitShader.setMat4("vProjection", projection);
                phongEmitShader.setFloat3("fViewPos", fpsView.getPosition());
                phongEmitShader.setFloat("material.shininess", 64.f);
                phongEmitShader.setFloat3("light.pos", lightPos);
                phongEmitShader.setFloat3("light.ambient", ambient); // 通常比較低強度
                phongEmitShader.setFloat3("light.diffuse", diffuse); // 設成希望光的顏色，将光照调暗了一些以搭配场景
                phongEmitShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f); // 通常是白色
                // phongSunShader
                phongSunShader.setMat4("vView", view);
                phongSunShader.setMat4("vProjection", projection);
                phongSunShader.setFloat3("fViewPos", fpsView.getPosition());
                phongSunShader.setFloat("material.shininess", 64.f);
                phongSunShader.setFloat3("light.direction", sunDirection);
                phongSunShader.setFloat3("light.ambient", ambient); // 通常比較低強度
                phongSunShader.setFloat3("light.diffuse", diffuse); // 設成希望光的顏色，将光照调暗了一些以搭配场景
                phongSunShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f); // 通常是白色
                // phongPointLightShader
                phongPointLightShader.setMat4("vView", view);
                phongPointLightShader.setMat4("vProjection", projection);
                phongPointLightShader.setFloat3("fViewPos", fpsView.getPosition());
                phongPointLightShader.setFloat("material.shininess", 64.f);
                phongPointLightShader.setFloat3("light.pos", lightPos);
                phongPointLightShader.setFloat3("light.ambient", ambient); // 通常比較低強度
                phongPointLightShader.setFloat3("light.diffuse", diffuse); // 設成希望光的顏色，将光照调暗了一些以搭配场景
                phongPointLightShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f); // 通常是白色
                phongPointLightShader.setFloat("light.constant", 1.f);
                phongPointLightShader.setFloat("light.linear", 0.09f);
                phongPointLightShader.setFloat("light.quadratic", 0.032f);
            }

            // Render phong-shaded sphere
            {
                sphere.Bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, phongSpherePos);
//                model = glm::rotate(model, glm::radians(0.0), glm::vec3(0.f, 1.f, 0.f));
                phongShader.setMat4("vModel", model);
                glDrawElements(GL_TRIANGLES, sphere_indices_size, GL_UNSIGNED_INT, 0);
                sphere.Unbind();
            }

            // phong with diffuse & specular map
//            {
//                box.Bind();
//                model = glm::mat4(1.f);
//                model = glm::translate(model, glm::vec3(-3.f, 0.f, 0.f));
//                model = glm::rotate(model, lightClk/*glm::radians(0.0)*/, glm::vec3(0.f, 1.f, 0.f));
//                phongDiffuseShader.setMat4("vModel", model);
//                glDrawArrays(GL_TRIANGLES, 0, 36);
//                box.Unbind();
//            }

            // phong with emission map
//            {
//                box.Bind();
//                model = glm::mat4(1.f);
//                model = glm::translate(model, glm::vec3(-3.f, 0.f, 1.5f));
//                model = glm::rotate(model, lightClk/*glm::radians(0.0)*/, glm::vec3(0.f, 1.f, 0.f));
//                phongEmitShader.setMat4("vModel", model);
//                glDrawArrays(GL_TRIANGLES, 0, 36);
//                box.Unbind();
//            }

            // point light
            {
                box.Bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, glm::vec3(-3.f, 0.f, 0.f));
                model = glm::rotate(model, lightClk/*glm::radians(0.0)*/, glm::vec3(0.f, 1.f, 0.f));
                phongPointLightShader.setMat4("vModel", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                box.Unbind();
            }

            // boxes get lighted by sun
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
                static glm::vec3 origin(10.f, 0.f, 0.f);
                for(int i = 0; i < 10; i++)
                {
                    model = glm::mat4(1.f);
                    model = glm::translate(model, origin + cubePositions[i]);
                    float angle = 20.0f * (float)i;
                    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                    phongSunShader.setMat4("vModel", model);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                box.Unbind();
            }

            // Render light source box
            {
                boxWithNormal.Bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, lightPos);
                model = glm::scale(model, glm::vec3(0.2f));
                lightBulbShader.setMat4("vModel", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                boxWithNormal.Unbind();
            }
        }

        window.pushGLStates();
        ImGui::SFML::Render(window);
        window.popGLStates();
        window.display();
    }

    return 0;
}