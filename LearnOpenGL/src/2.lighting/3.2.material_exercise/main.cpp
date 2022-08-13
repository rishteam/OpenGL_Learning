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

struct Material
{
    std::string name;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

// From http://devernay.free.fr/cours/opengl/materials.html
std::unordered_map<std::string, Material> materialMap = {
    {"emerald", {"emerald", {0.021500, 0.174500, 0.021500}, {0.075680, 0.614240, 0.075680}, {0.633000, 0.727811, 0.633000}, 0.600000*128}},
    {"jade", {"jade", {0.135000, 0.222500, 0.157500}, {0.540000, 0.890000, 0.630000}, {0.316228, 0.316228, 0.316228}, 0.100000*128}},
    {"obsidian", {"obsidian", {0.053750, 0.050000, 0.066250}, {0.182750, 0.170000, 0.225250}, {0.332741, 0.328634, 0.346435}, 0.300000*128}},
    {"pearl", {"pearl", {0.250000, 0.207250, 0.207250}, {1.000000, 0.829000, 0.829000}, {0.296648, 0.296648, 0.296648}, 0.088000*128}},
    {"ruby", {"ruby", {0.174500, 0.011750, 0.011750}, {0.614240, 0.041360, 0.041360}, {0.727811, 0.626959, 0.626959}, 0.600000*128}},
    {"turquoise", {"turquoise", {0.100000, 0.187250, 0.174500}, {0.396000, 0.741510, 0.691020}, {0.297254, 0.308290, 0.306678}, 0.100000*128}},
    {"brass", {"brass", {0.329412, 0.223529, 0.027451}, {0.780392, 0.568627, 0.113725}, {0.992157, 0.941176, 0.807843}, 0.217949*128}},
    {"bronze", {"bronze", {0.212500, 0.127500, 0.054000}, {0.714000, 0.428400, 0.181440}, {0.393548, 0.271906, 0.166721}, 0.200000*128}},
    {"chrome", {"chrome", {0.250000, 0.250000, 0.250000}, {0.400000, 0.400000, 0.400000}, {0.774597, 0.774597, 0.774597}, 0.600000*128}},
    {"copper", {"copper", {0.191250, 0.073500, 0.022500}, {0.703800, 0.270480, 0.082800}, {0.256777, 0.137622, 0.086014}, 0.100000*128}},
    {"gold", {"gold", {0.247250, 0.199500, 0.074500}, {0.751640, 0.606480, 0.226480}, {0.628281, 0.555802, 0.366065}, 0.400000*128}},
    {"silver", {"silver", {0.192250, 0.192250, 0.192250}, {0.507540, 0.507540, 0.507540}, {0.508273, 0.508273, 0.508273}, 0.400000*128}},
    {"black_plastic", {"black_plastic", {0.000000, 0.000000, 0.000000}, {0.010000, 0.010000, 0.010000}, {0.500000, 0.500000, 0.500000}, 0.250000*128}},
    {"cyan_plastic", {"cyan_plastic", {0.000000, 0.100000, 0.060000}, {0.000000, 0.509804, 0.509804}, {0.501961, 0.501961, 0.501961}, 0.250000*128}},
    {"green_plastic", {"green_plastic", {0.000000, 0.000000, 0.000000}, {0.100000, 0.350000, 0.100000}, {0.450000, 0.550000, 0.450000}, 0.250000*128}},
    {"red_plastic", {"red_plastic", {0.000000, 0.000000, 0.000000}, {0.500000, 0.000000, 0.000000}, {0.700000, 0.600000, 0.600000}, 0.250000*128}},
    {"white_plastic", {"white_plastic", {0.000000, 0.000000, 0.000000}, {0.550000, 0.550000, 0.550000}, {0.700000, 0.700000, 0.700000}, 0.250000*128}},
    {"yellow_plastic", {"yellow_plastic", {0.000000, 0.000000, 0.000000}, {0.500000, 0.500000, 0.000000}, {0.600000, 0.600000, 0.500000}, 0.250000*128}},
    {"black_rubber", {"black_rubber", {0.020000, 0.020000, 0.020000}, {0.010000, 0.010000, 0.010000}, {0.400000, 0.400000, 0.400000}, 0.078125*128}},
    {"cyan_rubber", {"cyan_rubber", {0.000000, 0.050000, 0.050000}, {0.400000, 0.500000, 0.500000}, {0.040000, 0.700000, 0.700000}, 0.078125*128}},
    {"green_rubber", {"green_rubber", {0.000000, 0.050000, 0.000000}, {0.400000, 0.500000, 0.400000}, {0.040000, 0.700000, 0.040000}, 0.078125*128}},
    {"red_rubber", {"red_rubber", {0.050000, 0.000000, 0.000000}, {0.500000, 0.400000, 0.400000}, {0.700000, 0.040000, 0.040000}, 0.078125*128}},
    {"white_rubber", {"white_rubber", {0.050000, 0.050000, 0.050000}, {0.500000, 0.500000, 0.500000}, {0.700000, 0.700000, 0.700000}, 0.078125*128}},
    {"yellow_rubber", {"yellow_rubber", {0.050000, 0.050000, 0.000000}, {0.500000, 0.500000, 0.400000}, {0.700000, 0.700000, 0.040000}, 0.078125*128}},
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

    // Load m_ShaderID
    Shader phongShader("phong.glsl");
//    phongShader.setFloat3("fObjectColor", 1.0f, 0.5f, 0.31f);

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
    float lightClk = 0.f;
    bool running = true;
    while (running)
    {
        static bool isImGuiWindowFocus = false, isImGuiWindowHover = false;
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

            if(!isImGuiWindowHover)
                fpsView.processEvent(event);
        }
        float dt = stClk.restart().asSeconds();

        // ImGui Update
        static float bgColor[4];
        static glm::vec3 phongSpherePos(3.f, 0.f, 0.f);
        static Material phongSphereMaterial = materialMap["emerald"];
        static const char *selectedMaterialName = materialMap["emerald"].name.c_str();
        static glm::vec3 lightPos(0.0f, 0.f, 0.f), lightColor(1.f, 1.f, 1.f);
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
            isImGuiWindowFocus = ImGui::IsWindowFocused();
            isImGuiWindowHover = ImGui::IsWindowHovered();
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
            ImGui::Separator();
            {
                if(ImGui::BeginCombo("Sphere Material", selectedMaterialName))
                {
                    for (auto &&[name, m]: materialMap)
                    {
                        if (ImGui::Selectable(name.c_str()))
                        {
                            phongSphereMaterial = m;
                            selectedMaterialName = name.c_str();
                        }
                    }
                    ImGui::EndCombo();
                }
            }
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
            if(!isImGuiWindowFocus)
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
            phongShader.setFloat3("material.ambient", phongSphereMaterial.ambient);
            phongShader.setFloat3("material.diffuse", phongSphereMaterial.diffuse);
            phongShader.setFloat3("material.specular", phongSphereMaterial.specular);
            phongShader.setFloat("material.shininess", phongSphereMaterial.shininess);

            phongShader.setFloat3("light.pos", lightPos);
            // Using http://devernay.free.fr/cours/opengl/materials.html
            phongShader.setFloat3("light.ambient",  1.0f, 1.0f, 1.0f);
            phongShader.setFloat3("light.diffuse",  1.0f, 1.0f, 1.0f);
            phongShader.setFloat3("light.specular", 1.0f, 1.0f, 1.0f); // 通常是白色
            //
            lightSurfaceShader.setMat4("vView", view);
            lightSurfaceShader.setMat4("vProjection", projection);

            // Draw phong-shaded sphere
            {
                sphere.bind();
                model = glm::mat4(1.f);
                model = glm::translate(model, phongSpherePos);
                model = glm::rotate(model, /*glm::radians(*/0.0f/*)*/, glm::vec3(0.f, 1.f, 0.f));
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