#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

#include "shader.h"
#include "buffer.h"

#define WIRE_MODE 0

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}

float vertices[] = {
    0.5f, 0.5f, 0.0f,   // 右上角
    0.5f, -0.5f, 0.0f,  // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
    -0.5f, 0.5f, 0.0f,   // 左上角
    // Color
    1.f, 0.f, 0.f,
    0.f, 1.f, 0.f,
    0.f, 0.f, 1.f,
    1.f, 1.f, 1.f
};

uint32_t indices[] = {
    0, 1, 3, // 第一個三角形
    1, 2, 3  // 第二個三角形
};

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

    // Load shader
    Shader shader("a.vert", "a.frag");
    uint32_t program = shader.getShaderID();
    // Load vertices
    // VAO
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // TODO: These will be inside the VertexArray
    // VBO
    BufferLayout bufferLayout = {
        {ShaderDataType::Float3, "aPos"},
        {ShaderDataType::Float3, "color"}
    };
    VertexBuffer vertex(vertices, sizeof(vertices));
    vertex.setLayout(bufferLayout);
    vertex.bind();
    // EBO
    IndexBuffer index(indices, sizeof(indices)/sizeof(uint32_t));
    index.bind();

    // Specify Vertex Attribute
    // glBindAttribLocation(program, 0, "aPos");
    uint32_t aPos_attrib_index = glGetAttribLocation(program, "aPos");
    printf("aPos location = %d\n", aPos_attrib_index);
    glVertexAttribPointer(
        aPos_attrib_index, // Location
        3,                 // size
        GL_FLOAT,          // type
        GL_FALSE,          // normalize?
        3 * sizeof(float), // stride
        (void *)0          // offsets
    );
    glEnableVertexAttribArray(aPos_attrib_index);

    uint32_t color_attrib_index = glGetAttribLocation(program, "color");
    glVertexAttribPointer(color_attrib_index, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)(12 * sizeof(float)));
    glEnableVertexAttribArray(color_attrib_index);

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
        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        shader.bind();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        shader.unbind();

        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        window.pushGLStates();
        {
            ImGui::SFML::Render(window);
        }
        window.popGLStates();
        window.display();
    }

    // release resources...
    glDeleteVertexArrays(1, &vao);
}