#include <iostream>
#include <cmath>
#include <functional>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

#include <imgui.h>
#include <imgui-SFML.h>
#include "shader.h"
#include "VertexArray.h"
#include "camera.h"
#include "player.h"

#define WIRE_MODE 0

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}
// Vertex Shader
const char *vertexShaderSource = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 vModel;
uniform mat4 vView;
uniform mat4 vProjection;

void main()
{
    gl_Position = vProjection * vView * vModel * vec4(aPos, 1.0);
}
)glsl";
// Fragment Shader
const char *fragmentShaderSoucre = R"glsl(
#version 450 core
out vec4 FragColor;


uniform sampler2D tex1;
uniform sampler2D tex2;

uniform vec4 fColor;
uniform float fMix;

void main()
{
    vec4 resultColor = fColor;
    FragColor = resultColor;
}
)glsl";

#include "vertices.h"

glm::vec3 lightPos(1.2, 1.f, 0.f);

const uint32_t screenWidth = 1280, screenHeight = 720;

sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));

glm::vec3 lightColor(0.33f, 0.42f, 0.18f);
glm::vec3 objectColor(1, 0.5, 0.31);
int main()
{
    std::cout << std::filesystem::current_path();
    window.setVerticalSyncEnabled(true);
    // Load OpenGL functions using glad
    if (!gladLoadGL())
    {
        printf("Something went wrong!\n");
        exit(-1);
    }
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::SFML::Init(window);
    window.setActive(true);
    init();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    // Load shader
    Shader shader("shader/8.1.Colors/lighting.vs", "shader/8.1.Colors/lighting.fs");
    Shader shader2("shader/8.1.Colors/light_cube.vs", "shader/8.1.Colors/light_cube.fs");
    VertexArray vertexArray;
    BufferLayout layout = {
        {ShaderDataType::Float3, "aPos"}
    };
    VertexBuffer vertex(vertices, sizeof(vertices));
    vertex.setLayout(layout);
    vertexArray.addVertexBuffer(&vertex);
    FirstPersonView fpsView;

    sf::Clock stClk;
    sf::Clock deltaClock;
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
                glViewport(0, 0, event.size.width, event.size.height);

            fpsView.processEvent(event);
        }
        float dt = stClk.restart().asSeconds();
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("6.2.cube");
        static bool wire_mode = false;
        ImGui::Checkbox("Wire Mode", &wire_mode);
        // tint
        static float tintColor[4] = {1.f, 1.f, 1.f, 1.f};
        ImGui::ColorEdit4("Tint", tintColor, ImGuiColorEditFlags_Float);
        // mix
        static float mix = 0.2f;
        ImGui::SliderFloat("Mix", &mix, 0.f, 1.f);
        // background color
        static float bgColor[4];
        ImGui::ColorEdit4("BG", bgColor, ImGuiColorEditFlags_Float);
        ImGui::End();

        ImGui::Begin("Matrix");
        //
        glm::mat4 model(1.f), view(1.f), projection(1.f);
        ImGui::Text("Model");
        static float rotate_x = -55.0f, rotate_y = 0.f, rotate_z = 0.f;
        float step = 100 * stClk.getElapsedTime().asSeconds();
//        rotate_x = step;
//        rotate_y = step;
//        rotate_z = step;
        ImGui::DragFloat("Rotate x", &rotate_x, 1.0, -360, 360);
        ImGui::DragFloat("Rotate y", &rotate_y, 1.0 , - 360, 360);
        ImGui::DragFloat("Rotate z", &rotate_z, 1.0, - 360, 360);
        model = glm::rotate(model, glm::radians(rotate_x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotate_y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotate_z), glm::vec3(0.0f, 0.0f, 1.0f));

        ImGui::Separator();
        static float trans[3] = {0.0f, 0.0f, -3.0f};
        ImGui::Text("View");
        ImGui::SliderFloat3("Translate", trans, -100.f, 100.f);
        view = glm::translate(view, glm::vec3(trans[0], trans[1], trans[2]));
        //
        ImGui::Text("View");
        view = fpsView.getViewMatrix();
        ImGui::DragFloat3("Camera", glm::value_ptr(fpsView.m_camera.m_pos));
        ImGui::DragFloat("Speed", &fpsView.cameraSpeed);
        ImGui::DragFloat("Sensitivity", &fpsView.sensitivity);
        // Projection matrix
        ImGui::Text("Projection");
        ImGui::DragFloat("FOV", &fpsView.m_camera.m_fov, 1.0f, 0.f, 360.f);
        ImGui::DragFloat("Near", &fpsView.m_camera.m_near, 0.01);
        ImGui::DragFloat("Far", &fpsView.m_camera.m_far, 0.1);
        ImGui::Text("fpsView.m_camera.m_screenRatio = %f", fpsView.m_camera.m_screenRatio);
        projection = fpsView.getProjectionMatrix();

        ImGui::Separator();

        ImGui::Text("Light");
        ImGui::ColorEdit4("Light Color", glm::value_ptr(lightColor));
        ImGui::Text("Object");
        ImGui::ColorEdit4("Object Color", glm::value_ptr(objectColor));
        ImGui::End();


        glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        fpsView.update(dt);
        // Upload uniforms
        shader.bind();
        shader.setMat4("vModel", model);
        shader.setMat4("vView", view);
        shader.setMat4("vProjection", projection);
        shader.setFloat3("lightColor", lightColor);
        shader.setFloat3("objectColor", objectColor);

        vertexArray.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        shader.unbind();


        shader2.bind();
        shader2.setMat4("vView", view);
        shader2.setMat4("vProjection", projection);
        model = glm::mat4(1.f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        shader2.setMat4("vModel", model);
        shader2.setFloat3("lightColor", lightColor);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        vertexArray.unbind();
        shader2.unbind();



        window.pushGLStates();
        {
            ImGui::SFML::Render(window);
        }
        window.popGLStates();
        window.display();
    }

    return 0;
}