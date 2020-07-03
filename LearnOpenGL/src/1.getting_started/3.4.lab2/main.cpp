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

    // Load shader
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
        ImGui::End();

        // Update

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.bind();
        vertexArray.bind();
        glDrawElements(GL_TRIANGLES, index.getCount(), GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
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