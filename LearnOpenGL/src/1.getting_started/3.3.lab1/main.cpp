#include <iostream>
#include <algorithm>
#include <cmath>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <glad/glad.h>
#include <SFML/OpenGL.hpp>

#include <imgui.h>
#include <imgui-SFML.h>

void init()
{
    setvbuf(stdin, nullptr, _IONBF, 0);
    setvbuf(stdout, nullptr, _IONBF, 0);
}
// Vertex Shader
const char *vertexShaderSource = R"glsl(
#version 450 core
layout (location = 0) in vec3 aPos;

uniform vec4 posOffset;
uniform float rotate;

void main()
{
    float rotateDeg = rotate * 0.0174532925;
    mat2 rot = mat2(
        cos(rotateDeg), -sin(rotateDeg),
        sin(rotateDeg),  cos(rotateDeg)
    );
    vec2 pos = rot * aPos.xy;
    gl_Position = vec4(pos.x, pos.y, aPos.z, 1.0) + posOffset;
}
)glsl";
// Fragment Shader
const char* fragmentShaderSoucre = R"glsl(
#version 450 core
out vec4 FragColor;

uniform vec4 ourColor;

void main()
{
    FragColor = ourColor;
}
)glsl";

uint32_t LoadShader(GLenum type, const char* src)
{
    uint32_t shader;
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, log);
        printf("Error Shader %s compile error\n%s\n", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment", log);
        return 0;
    }
    return shader;
}

uint32_t LinkShaderProgram(uint32_t vertex, uint32_t fragment)
{
    uint32_t program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    //
    int success;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, log);
        printf("Error Shader Linking error\n%s\n", log);
        return 0;
    }
    return program;
}

float vertices[] = {
    0.5f, 0.5f, 0.0f,  // 右上角
    0.5f, -0.5f, 0.0f, // 右下角
    -0.5f, -0.5f, 0.0f, // 左下角
    -0.5f, 0.5f, 0.0f  // 左上角
};

uint32_t indices[] = {
    0, 1, 3, // 第一個三角形
    1, 2, 3  // 第二個三角形
};

int main()
{
    init();
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, sf::ContextSettings(24, 8, 4, 4, 4));
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

    // Load shader
    uint32_t vertexShader = LoadShader(GL_VERTEX_SHADER, vertexShaderSource);
    uint32_t fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentShaderSoucre);
    uint32_t program;
    if(vertexShader && fragmentShader)
    {
        program = LinkShaderProgram(vertexShader, fragmentShader);
        glUseProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    else
    {
        return 1;
    }
    // Load vertices
    // VAO
    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // VBO
    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // EBO
    uint32_t ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // Vertex Attributes
    uint32_t aPos_attrib_index = glGetAttribLocation(program, "aPos");
    glVertexAttribPointer(aPos_attrib_index, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(aPos_attrib_index);

    int vertexColor = glGetUniformLocation(program, "ourColor");
    int posOffset = glGetUniformLocation(program, "posOffset");
    int rotateLocation = glGetUniformLocation(program, "rotate");

    sf::Clock deltaClock;
    sf::Clock clk;
    float t, green;
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
        ImGui::Begin("3.3.lab1");
        static bool wire_mode = false;
        ImGui::Checkbox("Wire Mode", &wire_mode);
        // Color
        static float freq = 1.f;
        static float color[4];
        ImGui::DragFloat("Freq", &freq);
        ImGui::Separator();
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

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(wire_mode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Pass the Uniforms
        t = clk.getElapsedTime().asSeconds();
        green = (sin(t*freq) / 2.f) + 0.5f;
        glUseProgram(program);
        glUniform4f(vertexColor, 0.0f, green, 0.0f, 1.0f);
        glUniform4f(posOffset, off[0], off[1], off[2], off[3]);
        glUniform1f(rotateLocation, rotate_degree); // degree to radian

        // Draw elements
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(uint32_t), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

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
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(program);

    return 0;
}