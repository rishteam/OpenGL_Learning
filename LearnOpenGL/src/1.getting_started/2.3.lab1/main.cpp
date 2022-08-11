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

struct Color3
{
    float r, g, b;
};
struct Point3
{
    float x, y, z;
};
union Vertex
{
    Color3 color;
    Point3 point;
    float p[3];
};
std::vector<Vertex> pointList;

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(
        24, // depthBits
        8,  // stencilBits
        4,  // antialiasingLevel
        4,  // majorVersion
        4   // minorVersion
    ));
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
    Shader shader("a.vert", "a.frag");
    uint32_t program = shader.getShaderID();
    // Load vertices
    VertexArray vertexArray;
    BufferLayout bufferLayout = {
        {ShaderDataType::Float3, "aPos"},
        {ShaderDataType::Float3, "color"}
    };
    VertexBuffer vertex;
    vertex.setLayout(bufferLayout);
    vertexArray.addVertexBuffer(&vertex);

    bool click = false;

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
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                click = true;
            }
        }
        // ImGui Update
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("hello, world");
            static bool wire_mode = false;
            ImGui::Checkbox("Wire Mode", &wire_mode);
            static bool blend = false;
            if(ImGui::Checkbox("Blend", &blend))
            {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else
                glDisable(GL_BLEND);

            auto [x, y] = sf::Mouse::getPosition(window);
            ImGui::Text("%d, %d", x, y);
        ImGui::End();
        ImGui::Begin("Point List");
            ImGui::Text("size = %d", pointList.size());
            for (int i = 0; i < pointList.size(); i += 2)
            {
                auto &p = pointList[i];
                auto &c = pointList[i + 1];
                ImGui::Text("Point %d: (%.2f, %.2f, %.2f), (%.1f, %.1f, %.1f)", i / 2,
                            p.point.x, p.point.y, p.point.z,
                            c.color.r, c.color.g, c.color.g);
            }
        ImGui::End();
        static std::vector<Vertex> tmp;
        ImGui::Begin("Tmp");
            ImGui::Text("size = %d", tmp.size());
            for (int i = 0; i < tmp.size(); i += 2)
            {
                auto &p = tmp[i];
                auto &c = tmp[i + 1];
                ImGui::Text("Point %d: (%.2f, %.2f, %.2f), (%.1f, %.1f, %.1f)", i / 2,
                            p.point.x, p.point.y, p.point.z,
                            c.color.r, c.color.g, c.color.g);
            }
        ImGui::End();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            for(int a = 0; a < 100; a++)
            {
                float radius = 10.f;
                auto x = randf(0, 800);
                auto y = randf(0, 600);
                auto w = (float)window.getSize().x;
                auto h = (float)window.getSize().y;
                for(int a = 0; a < 3; a++)
                {
                    auto deg = randf(0, 360);
                    pointList.push_back((Vertex){-1 + 2.f * (x + radius * cos(deg * 0.0174532925)) / w, 1 - 2.f * (y + radius * sin(deg * 0.0174532925)) / h, 0.f});
                    pointList.push_back((Vertex){randf(0, 1), randf(0, 1), randf(0, 1)});
                }
                vertex.setData(pointList.data(), pointList.size() * 3 * sizeof(float));
            }
        if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && !ImGui::IsAnyItemHovered() && click)
        {
            auto x = (float)event.mouseButton.x;
            auto y = (float)event.mouseButton.y;
            auto w = (float)window.getSize().x;
            auto h = (float)window.getSize().y;
            // mapping coordinate
            tmp.push_back((Vertex){-1 + 2.f * x / w, 1 - 2.f * y / h, 0.f});
            tmp.push_back((Vertex){randf(0, 1), randf(0, 1), randf(0, 1)});
            if(tmp.size() == 3*2)
            {
                for(auto &i : tmp) pointList.push_back(i);
                vertex.setData(pointList.data(), pointList.size() * 3 * sizeof(float));
                tmp.clear();
            }
        }
        click = false;

        // Update

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.bind();
        vertexArray.bind();
        glDrawArrays(GL_TRIANGLES, 0, pointList.size()/2);
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