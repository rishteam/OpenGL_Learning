#include <iostream>
#include <vector>
#include <iterator>
#include <random>
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "shader.h"
#include "buffer.h"
#include "VertexArray.h"

#define WIRE_MODE 0

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}

float randf(float low, float upper)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    return low + (upper - low) * dis(gen);
}

float vertices[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
};

uint32_t indices[] = {
    0, 1, 2
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(24,8 ,4 ,4 ,4));
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

    // Load m_ShaderID
    Shader shader("a.vs", "a.fs");
    uint32_t program = shader.getShaderID();
    // Load vertices
    VertexArray vertexArray;
    BufferLayout bufferLayout = {
        {ShaderDataType::Float3, "aPos"}
    };
    VertexBuffer vertex(vertices, sizeof(vertices));
    vertex.setLayout(bufferLayout);
    vertexArray.addVertexBuffer(&vertex);
    IndexBuffer index(indices, sizeof(indices) / sizeof(uint32_t));
    vertexArray.setIndexBuffer(&index);

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
        }
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("hello, world");
            static bool wire_mode = false;
            ImGui::Checkbox("Wire Mode", &wire_mode);

            auto [x, y] = sf::Mouse::getPosition(window);
            ImGui::Text("%d, %d", x, y);

            static float color[4];
            ImGui::ColorEdit4("Color", color, ImGuiColorEditFlags_Float);

            // Square offset
            static float off[4];
            ImGui::SliderFloat4("Offset", off, -1, 1);
            // Rotate
            static float rotate_degree = 0.f;
            ImGui::SliderFloat("Rotate", &rotate_degree, -360, 360);
            if (ImGui::Button("Reset"))
            {
                std::fill(off, off + 4, 0.f);
                rotate_degree = 0.f;
            }
        ImGui::End();

        // Update

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.bind();
        shader.setFloat4("uColor", glm::vec4(color[0], color[1], color[2], color[3]));
        shader.setFloat("rotate", rotate_degree);
        shader.setFloat4("posOffset", glm::vec4(off[0], off[1], off[2], off[3]));
        // m_ShaderID.setFloat
        vertexArray.bind();
        glDrawElements(GL_TRIANGLES, index.getCount(), GL_UNSIGNED_INT, 0);
        vertexArray.unbind();
        shader.unbind();

        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        window.pushGLStates();
        {
            ImGui::SFML::Render(window);
        }
        window.popGLStates();
        window.display();
    }

}